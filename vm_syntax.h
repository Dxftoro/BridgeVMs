#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

struct Instruction {
	int opcode;
	std::string lexem;
};

Instruction instrTable[] = {
	{INC,	"inc"},
	{DEC,	"dec"},
	{ADD,	"add"},
	{SUB,	"sub"},
	{MUL,	"mul"},
	{DIV,	"div"},
	{RET,	"ret"},
	{INT,	"int"},
	{PUSH,	"push"},
	{RPUSH,	"rpush"},
	{POP,	"pop"},
	{MOV,	"mov"},
	{XOR,	"xor"},
	{RES,	"res"},

	{RAX,	"rax"},
	{RBX,	"rbx"},
	{RCX,	"rcx"},
	{RDX,	"rdx"},
	{REX,	"rex"},
	{RFX,	"rfx"},
	{RGX,	"rgx"},
	{FHX,	"fhx"}
};

class SRMParser {
private:
	std::string code;
	SRM* machine;

public:
	SRMParser(SRM* machine) {
		this->machine = machine;
		code = "";
	}
	~SRMParser() {
		machine = nullptr;
		delete machine;
	}

	void SetMachine(SRM* machine) {
		this->machine = machine;
	}
	void SetCode(std::string code) {
		this->code = code;
	}

	int* Parse(std::string code) {
		std::istringstream strin(code);
		std::vector<std::string> instructions;

		std::string tmp;
		int tableSize = sizeof(instrTable) / sizeof(Instruction);
		int instrCount = 0;

		while (!strin.eof() && strin >> tmp) instrCount++;
		strin.seekg(0);
		
		int* bytes = new int[instrCount];
		for (int i = 0; i < instrCount;) {
			strin >> tmp;
			bool found = false;

			for (int k = 0; k < tableSize; k++) {
				if (tmp == instrTable[k].lexem) {
					bytes[i] = instrTable[k].opcode;
					i++;

					found = true;
					break;
				}
			}

			if (!found) {
				try {
					int res = std::stoi(tmp);
					bytes[i] = res;
					i++;
				}
				catch (std::exception& e) {
					std::cout << "Unknown lexem: " << "\"" << tmp << "\"" << std::endl;
					return nullptr;
				}
			}
		}

		return bytes;
	}

	void RunMachine() {
		int* bytes = Parse(this->code);

		if (bytes != nullptr) {
			std::cout << "SRM " << machine << " terminated with code " <<
				machine->HandleOpcode(bytes) << "." << std::endl;
		}
		else std::cout << "Failed to run SRM " << machine << "!" << std::endl;
	}
};