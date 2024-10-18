#pragma once
#include <iostream>
#include <string>
#include <stdarg.h>
#define SM_STACK_SIZE 32
#define RM_REGCNT	  8

enum VMResult {
	VM_RESULT_SUCCES,
	VM_RESULT_FAILURE,
	VM_MEMORY_ACCESS_ERROR,
	VM_DIVISION_BY_ZERO
};

enum Opcodes {
	INC,
	DEC,
	ADD,
	SUB,
	MUL,
	DIV,
	RET,
	INT,

	PUSH,
	RPUSH,
	POP,
	MOV,
	XOR,
	RES
};

std::string opcodeNames[] {
	"INC",
	"DEC",
	"ADD",
	"SUB",
	"MUL",
	"DIV",
	"RET",
	"INT",

	"PUSH",
	"RPUSH",
	"POP",
	"MOV",
	"XOR",
	"RES"
};

enum Registers {
	RAX,
	RBX,
	RCX,
	RDX,
	REX,
	RFX,
	RGX,
	FHX
};

class AM {
public:
	int* ptr;
	int accumulator;

	AM() {
		ptr = new int;
		accumulator = 0;
	}
	~AM() { ptr = nullptr; }

	VMResult HandleOpcode(int* bytes) {
		ptr = bytes;

		while (true) {
			int opcode = *ptr;

			switch (opcode) {
			
			case INC:
				accumulator++;
				break;
			case DEC:
				accumulator--;
				break;

			case ADD: {
				ptr++;
				int arg = *ptr;
				accumulator += arg;
				break;
			}
			
			case SUB: {
				ptr++;
				int arg = *ptr;
				accumulator -= arg;
				break;
			}

			case INT:
				std::cout << accumulator << std::endl;
				break;

			case RET:
				return VM_RESULT_SUCCES;
			default:
				return VM_RESULT_FAILURE;
			}

			ptr++;
		}

		return VM_RESULT_SUCCES;
	}
};

class SM {
private:
	int stack[SM_STACK_SIZE];
	int* opptr, *sptr;
	int res;

	void Push(int data) {
		*sptr = data;
		sptr++;
	}

	int Pop() {
		sptr--;
		return *sptr;
	}

public:

	SM() {
		memset(stack, 0, SM_STACK_SIZE);
		opptr = new int;
		sptr = &stack[0];
		res = 0;
	}
	~SM() {
		opptr = nullptr;
		sptr = nullptr;
	}

	void Debug(int opcode) {
		std::cout << opcodeNames[opcode] << ": ";
		for (int i = 0; i < SM_STACK_SIZE; i++) {
			std::cout << "|" << stack[i];
		}
		std::cout << "|" << std::endl;
	}

	VMResult HandleOpcode(int* bytes) {
		opptr = bytes;

		while (true) {
			if (sptr == nullptr) return VM_MEMORY_ACCESS_ERROR;
			int opcode = *opptr;

			switch (opcode) {

			case RES: {
				this->res = Pop();
				break;
			}
			case PUSH: {
				opptr++;
				int arg = *opptr;
				Push(arg);
				break;
			}
			case INC: {
				*sptr += 1;
				break;
			}
			case DEC: {
				*sptr -= 1;
				break;
			}
			case ADD: {
				int a = Pop(), b = Pop();
				Push(b + a);
				break;
			}
			case SUB: {
				int a = Pop(), b = Pop();
				Push(b - a);
				break;
			}
			case MUL: {
				int a = Pop(), b = Pop();
				Push(b * a);
				break;
			}
			case DIV: {
				int a = Pop(), b = Pop();
				if (a) Push(b / a);
				else return VM_DIVISION_BY_ZERO;
				break;
			}

			case INT:
				std::cout << res << std::endl;
				break;
			case RET:
				return VM_RESULT_SUCCES;
			default:
				return VM_RESULT_FAILURE;
			}
			opptr++;
		}

		return VM_RESULT_SUCCES;
	}
};

class SRM {
private:
	int registers[RM_REGCNT];
	int stack[SM_STACK_SIZE];
	int* ptr, * stackTop;

	void Push(int data) {
		*stackTop = data;
		stackTop++;
	}

	int Pop() {
		stackTop--;
		return *stackTop;
	}

public:
	SRM() {
		memset(registers, 0, RM_REGCNT);
		memset(stack, 0, SM_STACK_SIZE);
		ptr = nullptr;
		stackTop = &stack[0];
	}
	~SRM() {
		ptr = nullptr;
		stackTop = nullptr;
	}

	void DebugStack(int opcode) {
		std::cout << opcodeNames[opcode] << ": ";
		for (int i = 0; i < SM_STACK_SIZE; i++) {
			std::cout << "|" << stack[i];
		}
		std::cout << "|" << std::endl;
	}

	void CheckReg(int reg) {
		if (reg < 0 || reg >= RM_REGCNT) registers[FHX] = 1;
		else registers[FHX] = 0;
	}

	VMResult HandleOpcode(int* bytes) {
		ptr = bytes;

		while (true) {
			if (stackTop == nullptr || registers[FHX]) return VM_MEMORY_ACCESS_ERROR;
			int opcode = *ptr;

			switch (opcode) {

			case MOV: {
				ptr++;
				CheckReg(*ptr);
				registers[*ptr] = *(ptr + 1);
				ptr++;

				break;
			}

			case INC: {
				ptr++;
				CheckReg(*ptr);
				registers[*ptr]++;

				break;
			}

			case DEC: {
				ptr++;
				CheckReg(*ptr);
				registers[*ptr]--;

				break;
			}

			case ADD: {
				ptr++;
				CheckReg(*ptr);
				CheckReg(*(ptr + 1));

				registers[*ptr] += registers[*(ptr + 1)];
				ptr++;
				break;
			}

			case SUB: {
				ptr++;
				CheckReg(*ptr);
				CheckReg(*(ptr + 1));

				registers[*ptr] -= registers[*(ptr + 1)];
				ptr++;
				break;
			}

			case MUL: {
				ptr++;
				CheckReg(*ptr);
				CheckReg(*(ptr + 1));

				registers[*ptr] *= registers[*(ptr + 1)];
				ptr++;
				break;
			}

			case DIV: {
				ptr++;
				CheckReg(*ptr);
				CheckReg(*(ptr + 1));

				if (registers[*(ptr + 1)] == 0) return VM_DIVISION_BY_ZERO;
				registers[*ptr] /= registers[*(ptr + 1)];
				ptr++;
				break;
			}

			case XOR: {
				ptr++;
				CheckReg(*ptr);
				CheckReg(*(ptr + 1));

				registers[*ptr] ^= registers[*(ptr + 1)];
				ptr++;
				break;
			}

			case PUSH: {
				ptr++;
				Push(*ptr);
				break;
			}
			case RPUSH: {
				ptr++;
				CheckReg(*ptr);
				Push(registers[*ptr]);

				break;
			}

			case POP: {
				ptr++;
				CheckReg(*ptr);
				registers[*ptr] = Pop();
				break;
			}

			case INT: { //INT [REG]
				ptr++;
				CheckReg(*ptr);
				std::cout << registers[*ptr] << std::endl;
				break;
			}
			case RET:
				return VM_RESULT_SUCCES;
			default:
				return VM_RESULT_FAILURE;
			}

			ptr++;
		}
	}
};