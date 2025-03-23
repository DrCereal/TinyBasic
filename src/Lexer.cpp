#include <cassert>
#include <cctype>
#include <iostream>

#include <Lexer.h>

Lexer::Lexer()
{
}

void Lexer::display_tokens()
{
	assert(m_tokens.size() > 0 && "display_tokens()");

	for (std::size_t i = 0; i < m_tokens.size(); i++) {
		Token t = m_tokens[i];
		std::cout << "line: " << t.line << '\n';
		std::cout << "type: " << t.type << '\n';
		std::cout << "data: (" << t.data << ")\n";
	}
}

void Lexer::unget_token()
{
	if (m_tokens_index > 0)
		m_tokens_index--;
}

int Lexer::open_file(const char* file_name)
{
	assert(file_name != NULL && "open_file()");

	m_file.open(file_name);
	if (!m_file.is_open())
		return -1;
	m_file_name = file_name;

	get_tokens();
	m_file.close();

	// temporary
	display_tokens();

	return 0;
}

Token Lexer::get_token()
{
	assert(m_tokens.size() > 0 && "get_token()");
	assert(m_tokens_index < m_tokens.size() && "get_token()");

	Token ret = m_tokens[m_tokens_index];
	if (m_tokens_index + 1 != m_tokens.size())
		m_tokens_index++;
	return ret;
}

bool Lexer::is_eot()
{
	return m_tokens_index == (m_tokens.size() - 1);
}

Token Lexer::peek_token()
{
	assert(m_tokens.size() > 0 && "peek_token()");
	assert(m_tokens_index < m_tokens.size() && "peek_token()");
	return m_tokens[m_tokens_index];
}

void Lexer::get_tokens()
{
	assert(m_file.is_open() && "get_tokens()");

	Token t;
	while (!m_file.eof()) {

		if (!std::getline(m_file, m_line))
			break;
		m_cur_line++;

		unsigned int i = 0;
		while (i < m_line.length()) {
			char c = m_line[i];
			t.type = "invalid";

			if (isalpha(c)) {
				t = get_identifier(&i);
				t = check_keyword(t);
			} else if (isdigit(c)) {
				t = get_number(&i);
			} else if (c == '"') {
				t = get_string(&i);
			} else if (c == '\n') {
				t.type = "newline";
				t.data = '\n';
			} else if (isspace(c)) {
				i++;	// Eat
				continue;
			} else {
				t = get_character(&i);
			}
			
			assert(t.type != "" && "get_tokens()");

			t.line = m_cur_line;
			m_tokens.push_back(t);
		}

		if (peek_token().type != "newline") {
			t.type = "newline";
			t.data = "\n";
			t.line = m_cur_line;

			m_tokens.push_back(t);
		}
	}
}

void Lexer::emit_error(unsigned int line, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	printf("%s::%d::ERROR: ", m_file_name.c_str(), line);
	vprintf(fmt, args);
	printf("\n");

	va_end(args);
}

static const char* g_keywords[] = {"END", "LET", "PRINT"};
Token Lexer::check_keyword(Token t)
{
	for (unsigned int i = 0; i < 3; i++) {
		if (t.data == g_keywords[i]) {
			t.type = "keyword";
			break;
		}
	}

	return t;
}

Token Lexer::get_character(unsigned int* i)
{
	assert(m_line != "" && "get_character()");
	assert(*i < m_line.length() && "get_character()");

	Token ret;
	ret.type = "invalid";

	char c = m_line[(*i)++];
	ret.data = c;
	switch (c) {
	case '=':
		ret.type = "equal";
		break;
	case '+':
		ret.type = "plus";
		break;
	case '-':
		ret.type = "minus";
		break;
	case '*':
		ret.type = "mult";
		break;
	case '/':
		ret.type = "div";
		break;
	}

	return ret;
}

Token Lexer::get_identifier(unsigned int* i)
{
	assert(m_file.is_open() && "get_identifier()");
	assert(m_line != "" && "get_identifier()");
	assert(*i < m_line.length() && "get_identifier()");

	Token ret;
	ret.type = "identifier";

	while (*i < m_line.length()) {
		char c = m_line[*i];

		if (isalnum(c)) {
			ret.data += c;
			(*i)++;
		} else {
			break;
		}
	}

	return ret;
}

Token Lexer::get_number(unsigned int* i)
{
	assert(m_file.is_open() && "get_number()");
	assert(m_line != "" && "get_number()");
	assert(*i < m_line.length() && "get_number()");

	Token ret;
	ret.type = "number";

	while (*i < m_line.length()) {
		char c = m_line[*i];

		if (isdigit(c)) {
			ret.data += c;
			(*i)++;
		} else {
			break;
		}
	}

	return ret;
}

Token Lexer::get_string(unsigned int* i)
{
	assert(m_file.is_open() && "get_string()");
	assert(m_line != "" && "get_string()");
	assert(*i < m_line.length() && "get_string()");

	Token ret;
	ret.type = "string";

	(*i)++;		// Eat starting '"'
	while (*i < m_line.length()) {
		char c = m_line[*i];

		if (c != '"') {
			ret.data += c;
			(*i)++;		// Eat
		} else {
			(*i)++;		// Eat
			return ret;
		}
	}

	emit_error(m_cur_line, "expected closing '\"' before end of line!");
	return ret;
}
