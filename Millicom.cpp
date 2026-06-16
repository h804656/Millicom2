// StrGen.cpp: определяет точку входа для консольного приложения.

#include "stdafx.h"
#include "Consts.h"
#include "Bus.h"
#include "List.h"
#include "Console.h"
#include "StrGen.h"
#include "Lex.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>
#include "MeanShift.h"
#include "ALU.h"
#include "StreamFloatALU.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <crtdbg.h>
#endif

using namespace std;

int main(int argc, char* argv[]) // argv[1] = .oap to lex, argv[2] = .ind to run(optional)
{
#if defined(_WIN32) && 1
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif

	std::string indPath; //= "C:\\Users\\hacker\\Downloads\\millicom\\oap2\\CompileCC.ind"; // FORCE override

	if (indPath.empty()) {
		if (argc >= 3) {
			indPath = argv[2];
		}
		else {
			indPath = "CompileCC.ind";
		}
	}
	ifstream f(indPath);
	if (!f)
	{
		cerr << "Error: IND file not found: " << indPath << endl;
		return 1;
	}
	f.close();

	BusFU Bus{};

	Bus.ProgFU(200, { Cint, &argc });
	Bus.ProgFU(203, { Cchar, (char**)argv });

	string STR = indPath;
	Bus.ProgFU(10, { Cstring, &STR }); // load+execute the .ind; its baked tail lexes argv[2]
	return 0;
}
