#include <cctype>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include <GenCode.h>

GenCode::GenCode(IR ir)
{
	m_ir = ir;
}

void GenCode::optimize()
{
}

void GenCode::gen_asm()
{
	first_pass();

	emit_constants();
	emit_instructions();
}

int GenCode::compile(const char* file_name)
{
	std::string cmd = "nasm -felf64 emit.asm -o emit.o; ld emit.o -o ";
	cmd += file_name;

	int return_code = system(cmd.c_str());
	system("rm -f emit.asm emit.o");
	return return_code;
}

int GenCode::write_asm_to_file(const char* file_name)
{
	assert(file_name != NULL && "write_asm_to_file()");

	std::fstream file;
	file.open(file_name, std::fstream::out);

	if (!file.is_open())
		return -1;

	printf("write to file:\n%s\n", m_asm.c_str());

	for (unsigned int i = 0; i < m_asm.length(); i++)
		file.put(m_asm[i]);

	file.close();
	return 0;
}

static unsigned int cons_index = 0;
void GenCode::first_pass()
{
	// Get constants
	Constant cons;
	
	std::string text = m_ir.text;
	unsigned int occurence = 1;
	for (unsigned int i = 0; i < text.length(); i++) {
		char c = text[i];

		if (c == '"') {
			cons.type = 0;
			cons.occurence = occurence++;
			cons.str = get_string(&i);
			cons.name = "const" + std::to_string(cons_index++);

			m_constants.push_back(cons);
		} else if (isdigit(c)) {
			
		}
	}
}

void GenCode::emit_constants()
{
	m_asm += "section .data\n";

	for (Constant cons : m_constants) {
		m_asm += "\t";
		m_asm += cons.name + ": db '";
		m_asm += cons.str + "', 10\n";

		m_asm += "\t";
		m_asm += cons.name + "_len: equ $-" + cons.name + "\n";
	}
}

void GenCode::emit_constant(Constant cons)
{
	m_asm += "\tmov rax, 1\n";
	m_asm += "\tmov rdi, 1\n";
	m_asm += "\tmov rsi, " + cons.name + "\n";
	m_asm += "\tmov rdx, " + cons.name + "_len\n";
	m_asm += "\tsyscall\n";
}

void GenCode::emit_instructions()
{
	m_asm += "section .text\n";
	m_asm += "\tglobal _start\n";
	m_asm += "_start:\n";

	unsigned int str_cons_occurence = 1;
	for (std::size_t i = 0; i < m_ir.code.size(); i++) {
		char op = m_ir.code[i];

		Constant cons;
		switch (op)
		{
		case 0x00:
			cons = get_const_from_occurence(str_cons_occurence++);
			emit_constant(cons);
			break;
		case 0x0b:
			m_asm += "\tmov rax, 60\n";
			m_asm += "\txor rdi, rdi\n";
			m_asm += "\tsyscall\n";
			break;
		}
	}
}

Constant GenCode::get_const_from_occurence(unsigned int occur)
{
	for (Constant cons : m_constants) {
		if (cons.occurence == (int)occur)
			return cons;
	}

	Constant invalid;
	return invalid;
}

std::string GenCode::get_string(unsigned int* i)
{
	assert(*i + 1 < m_ir.text.length() && "get_string()");

	std::string ret;
	std::string text = m_ir.text;
	(*i)++;	// Ignore initial '"'

	for (;*i < text.length(); (*i)++) {
		char c = text[*i];
		
		if (c == '"')
			break;
		else
			ret += c;
	}

	printf("string: %s\n", ret.c_str());

	(*i)++;
	return ret;
}
