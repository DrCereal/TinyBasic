#include <cassert>
#include <cstdarg>
#include <iostream>

#include <Parser.h>

#define VERIFY(a) \
	if ((a).valid == false) { \
		ret.valid = false; \
		return ret; }

Parser::Parser(const char* file_name)
{
	m_file_name = file_name;
	m_lexer_status = m_lexer.open_file(file_name);
}

void Parser::display_AST()
{
	printf("\nAST\n");
	for (std::size_t i = 0; i < m_ast.lines.size(); i++) {
		Line l = m_ast.lines[i];
		printf("line: %i\n", l.number);
		if (l.stmt.print != NULL) {
			display_print(l.stmt.print);
		} else if (l.stmt.let != NULL) {
			printf("\t%s=\n", l.stmt.let->id.c_str());
			display_expr(l.stmt.let->expr);
		} else if (l.stmt.end) {
			printf("\tend\n");
		}
	}
}

void Parser::parse_syntax()
{
	while (!m_lexer.is_eot())
		FuncLine();
}

void Parser::display_factor(Factor f)
{
	if (f.var != "")
		printf("\t\t%s\n", f.var.c_str());
	else if (f.num != "")
		printf("\t\t%s\n", f.num.c_str());
}

void Parser::display_term(Term term)
{
	display_factor(term.first);
	for (Factor f : term.optional) {
		printf("\t\t%s\n", f.op.c_str());
		display_factor(f);
	}
}

void Parser::display_expr(Expr expr)
{
	display_term(expr.first);
	for (Term term : expr.optional) {
		printf("\t%s\n", term.op.c_str());
		display_term(term);
	}
}

void Parser::display_print(PrintStmt* stmt)
{
	if (stmt->list.first.is_str) {
		printf("\tprint: %s\n", stmt->list.first.str.c_str());
	} else {
		printf("\tprint:\n");
		display_expr(*(stmt->list.first.expr));
	}
}

void Parser::emit_error(unsigned int line, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	printf("%s::%d::ERROR: ", m_file_name.c_str(), line);
	vprintf(fmt, args);
	printf("\n");

	va_end(args);
}

bool Parser::Expect(unsigned int line, const char* type)
{
	Token t = m_lexer.peek_token();
	if (t.type != type) {
		emit_error(line, "expected '%s' got '%s'!", type, t.data.c_str());
		return false;
	}

	return true;
}

Factor Parser::FuncFactor()
{
	Factor ret;
	ret.valid = true;
	Token t = m_lexer.get_token();

	// TODO: Parens around expression
	if (t.type == "identifier") {
		ret.var = t.data;
	} else if (t.type == "number") {
		ret.num = t.data;
	} else {
		m_lexer.unget_token();
		ret.valid = false;
		return ret;
	}

	return ret;
}

Term Parser::FuncTerm()
{
	Term ret;
	ret.valid = true;
	ret.first = FuncFactor();

	VERIFY(ret.first);

	while (!m_lexer.is_eot()) {
		Token t = m_lexer.peek_token();
		if (t.type == "mult" || t.type == "div") {
			m_lexer.get_token();	// Eat

			Factor f = FuncFactor();
			VERIFY(f);

			f.op = t.data;
			ret.optional.push_back(f);
		} else {
			break;
		}
	}

	return ret;
}

Expr Parser::Expression()
{
	Expr ret;
	ret.valid = true;

	Token t = m_lexer.peek_token();
	if (t.type == "plus" || t.type == "minus") {
		m_lexer.get_token();	// Eat
		ret.first.op = t.data;
	}

	ret.first = FuncTerm();
	if (ret.first.valid == false) {
		ret.valid = false;
		return ret;
	}

	while (!m_lexer.is_eot()) {
		Token t = m_lexer.peek_token();
		if (t.type == "plus" || t.type == "minus") {
			m_lexer.get_token();	// Eat

			Term term = FuncTerm();
			VERIFY(term);

			term.op = t.data;
			ret.optional.push_back(term);
		} else {
			break;
		}
	}

	return ret;
}

ExprList Parser::ExpressionList()
{
	ExprList ret;
	ret.valid = true;

	Token t = m_lexer.peek_token();
	if (t.type == "string") {
		ret.first.is_str = true;
		ret.first.str = t.data;

		m_lexer.get_token();	// Eat
	} else {
		ret.first.is_str = false;
		ret.first.expr = new Expr();
		*(ret.first.expr) = Expression();

		VERIFY(*(ret.first.expr));
	}
	//TODO: Else, expression.

	// TODO: Rest of expression list.

	return ret;
}

Stmt Parser::Statement()
{
	Stmt ret;
	ret.valid = true;
	
	Token t = m_lexer.peek_token();
	ret.line = t.line;

	assert(t.type == "keyword" && "Statement()");

	if (t.data == "PRINT") {
		m_lexer.get_token();	// Eat keyword

		ret.print = new PrintStmt();
		ret.print->list = ExpressionList();

		VERIFY(ret.print->list);
	} else if (t.data == "LET") {
		m_lexer.get_token();

		ret.let = new LetStmt();
		
		if (!Expect(ret.line, "identifier")) {
			ret.valid = false;
			return ret;	
		}
		t = m_lexer.get_token();

		if (!Expect(ret.line, "equal")) {
			ret.valid = false;
			return ret;
		}
		m_lexer.get_token();	// Eat

		ret.let->id = t.data;
		ret.let->expr = Expression();

		VERIFY(ret.let->expr);
	} else if (t.data == "END") {
		m_lexer.get_token();	// Eat keyword
		ret.end = true;
	}

	if (Expect(ret.line, "newline"))
		m_lexer.get_token();	// Eat new line

	return ret;
}

void Parser::FuncLine()
{
	Line l;
	
	Token t = m_lexer.peek_token();
	if (t.type == "number") {
		l.number = std::stoi(t.data);
		m_lexer.get_token();	// Eat
	}

	l.stmt = Statement();

	if (l.stmt.valid == false) {
		emit_error(l.stmt.line, "syntax error!");
		return;
	}

	m_ast.lines.push_back(l);
}
