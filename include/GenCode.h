#ifndef GENCODE_H
#define GENCODE_h

#include <string>
#include <vector>

#include <GenIR.h>

struct Variable
{
};

// Type:
// 0 - String

struct Constant
{
	char		type = -1;
	int			occurence = -1;

	std::string	str;
	std::string name;
};

class GenCode
{
public:

	GenCode(IR ir);

	void		optimize();
	void		gen_asm();

	int 		compile(const char* file_name);
	int			write_asm_to_file(const char* file_name);

private:

	void		first_pass();
	void		emit_constants();
	void		emit_constant(Constant cons);
	void		emit_instructions();

	Constant	get_const_from_occurence(unsigned int occur);
	std::string	get_string(unsigned int* i);

private:

	IR			m_ir;
	std::string	m_asm;

	std::vector<Constant>	m_constants;
	std::vector<Variable>	m_variables;

};

#endif
