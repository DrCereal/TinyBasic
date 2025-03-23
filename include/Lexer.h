#ifndef LEXER_H
#define LEXER_H

#include <cstdarg>
#include <fstream>
#include <string>
#include <vector>

struct Token
{
	unsigned int	line = 0;

	std::string		type;
	std::string		data;
};

class Lexer
{
public:
	Lexer();

	void display_tokens();
	void unget_token();

	bool	is_eot();
	int 	open_file(const char* file_name);
	Token	get_token();
	Token 	peek_token();

private:

	void get_tokens();
	void emit_error(unsigned int line, const char* fmt, ...);

	Token check_keyword(Token t);
	Token get_character(unsigned int* i);
	Token get_identifier(unsigned int* i);
	Token get_number(unsigned int* i);
	Token get_string(unsigned int* i);

private:
	unsigned int		m_cur_line = 0;
	unsigned int		m_tokens_index = 0;

	std::ifstream		m_file;
	std::string			m_line;
	std::string			m_file_name;
	std::vector<Token>	m_tokens;
};

#endif
