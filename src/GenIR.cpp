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
		else if (s.print != NULL)
			emit_print_string(l.stmt);
	}
}

void GenIR::emit_print_string(Stmt stmt)
{
	assert(stmt.print != NULL && "emit_print_string()");

	m_ir.code.push_back(0x00);
	m_ir.text += "print \"";

	m_ir.text += stmt.print->list.first.str;
	m_ir.text += "\"\n";
}

void GenIR::emit_end()
{
	m_ir.code.push_back(0x0b);
	m_ir.text += "end\n";
}
