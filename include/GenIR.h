#ifndef GENIR_H
#define GENIR_H

#include <AST.h>

/*
 *	IR codes:
 *		00 - print string
 *		01 - print number
 *		02 - print variable
 *		06 - let
 *		0B - end
 */

struct IR
{
	std::vector<char>	code;
	std::string 		text;
};

class GenIR
{
public:

	GenIR(AST ast);

	void 	optimize_IR();

	IR		get_IR() { return m_ir; }

private:

	void	emit_factor(Factor f);
	void	emit_term(Term term);
	void	emit_expr(Expr expr);
	void	emit_let(Stmt stmt);
	void 	emit_print_string(Stmt stmt);
	void 	emit_end();

private:

	IR	m_ir;

};

#endif
