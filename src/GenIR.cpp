#include <cassert>
#include <iostream>

#include <GenIR.h>

GenIR::GenIR(AST ast)
{
	for (std::size_t i = 0; i < ast.lines.size(); i++) {
		Line l = ast.lines[i];
		Stmt s = l.stmt;

		if (s.end)
			emit_end();
		else if (s.let != NULL)
			emit_let(s);
		else if (s.print != NULL)
			emit_print_string(s);
	}
}

void GenIR::emit_factor(Factor f)
{
	if (f.evaled) {
		m_ir.text += std::to_string(f.eval);
		return;
	}

	m_ir.text += f.var;
}

void GenIR::emit_term(Term term)
{
	if (term.evaled) {
		m_ir.text += std::to_string(term.eval);
		return;
	}

	emit_factor(term.first);

	for (Factor f : term.optional) {
		m_ir.text += f.op;
		emit_factor(f);
	}
}

void GenIR::emit_expr(Expr expr)
{
	if (expr.evaled) {
		m_ir.text += std::to_string(expr.eval);
		return;
	}

	emit_term(expr.first);

	for (Term t : expr.optional) {
		m_ir.text += t.op;
		emit_term(t);
	}
}

void GenIR::emit_let(Stmt stmt)
{
	assert(stmt.let != NULL && "emit_let()");

	m_ir.code.push_back(0x06);

	m_ir.text += stmt.let->id + "=";
	emit_expr(stmt.let->expr);
	m_ir.text += "\n";
}

void GenIR::emit_print_string(Stmt stmt)
{
	assert(stmt.print != NULL && "emit_print_string()");

	m_ir.code.push_back(0x00);

	m_ir.text += "print ";
	if (stmt.print->list.first.is_str) {
		m_ir.text += '\"';
		m_ir.text += stmt.print->list.first.str;
		m_ir.text += "\"\n";
	} else {
		bool evaled = stmt.print->list.first.expr->evaled;
		if (evaled)
			m_ir.text += '"';
		emit_expr(*(stmt.print->list.first.expr));
		if (evaled)
			m_ir.text += '"';
		m_ir.text += '\n';
	}
}

void GenIR::emit_end()
{
	m_ir.code.push_back(0x0b);
	m_ir.text += "end\n";
}
