#include <cstdlib>
#include <iostream>

#include <Parser.h>
#include <GenIR.h>
#include <GenCode.h>

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cout << "invalid usage\n";
		exit(1);
	}

	// Front-end
	Parser parser(argv[1]);
	if (parser.lexer_error() < 0) {
		std::cout << "problem opening file!\n";
		exit(1);
	}

	parser.parse_syntax();
	parser.display_AST();

	AST ast = parser.get_AST();

	// Middle-end
	GenIR gen_ir(ast);
	IR ir = gen_ir.get_IR();

	printf("\nIR\n%s\n", ir.text.c_str());

	// Back-end
	GenCode gen_code(ir);
	gen_code.optimize();
	gen_code.gen_asm();

	if (gen_code.write_asm_to_file("emit.asm") < 0) {
		printf("failed to write asm to file\n");
		return -1;
	}
	if (gen_code.compile("a.out") < 0) {
		printf("failed to compile program\n");
		return -1;
	}
	
	return 0;
}
