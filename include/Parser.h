#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

#include <AST.h>
#include <Lexer.h>

class Parser
{
public:

	Parser(const char* file_name);

	void 		display_AST();
	void 		parse_syntax();

	int 		lexer_error() { return m_lexer_status; }
	AST			get_AST() { return m_ast; }

private:

	void		display_factor(Factor f);
	void		display_term(Term term);
	void		display_expr(Expr expr);
	void		display_print(PrintStmt* stmt);
	void		emit_error(unsigned int line, const char* fmt, ...);

	bool		Expect(unsigned int line, const char* type);

	Factor		FuncFactor();
	Term		FuncTerm();
	Expr		Expression();
	ExprList	ExpressionList();
	Stmt		Statement();
	void		FuncLine();

private:

	int			m_lexer_status = 0;

	std::string	m_file_name;
	Lexer		m_lexer;
	AST			m_ast;

};

#endif
