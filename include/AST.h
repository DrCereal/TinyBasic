#ifndef AST_H
#define AST_H

#include <vector>

struct Expr;

struct Factor
{
	bool		valid = false;

	std::string op = "";
	std::string	var = "";
	std::string	num = "";
	Expr*		expr = NULL;
};

struct Term
{
	bool					valid = false;

	std::string				op = "";
	Factor					first;
	std::vector<Factor>		optional;
};

struct Expr
{
	bool				valid = false;

	Term 				first;
	std::vector<Term>	optional;
};

struct StrExpr
{
	bool		is_str;

	std::string	str;
	Expr*		expr;
};

struct ExprList
{
	bool					valid = false;

	StrExpr					first;
	std::vector<StrExpr>	optional;
};

struct PrintStmt
{
	ExprList	list;
};

struct LetStmt
{
	std::string 	id;
	Expr			expr;
};

struct Stmt
{
	bool			valid = false;
	unsigned int	line = 0;

	bool			end = false;
	PrintStmt*		print = NULL;
	LetStmt*		let = NULL;
};

struct Line
{
	int				number = -1;
	Stmt			stmt;
};

struct AST
{
	std::vector<Line>	lines;
};

#endif
