#include <iostream>
#include <fstream>
#include <sstream>
#include "virtual_machine.h"
#include "vm_syntax.h"
using namespace std;

int main(int argc, char* argv[]) {
	SRM srmach;
	SRMParser parser(&srmach);

	string path;

	if (argc <= 1) path = "code.txt";
	else path = argv[1];

	ifstream fin(path);
	if (!fin) {
		cerr << "Can't open file \"" << path << "\"!" << endl;
		return -1;
	}

	ostringstream strout;

	strout << fin.rdbuf();
	fin.close();

	string code = strout.str();
	parser.SetCode(code);
	parser.RunMachine();

	return 0;
}