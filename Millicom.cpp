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

static void Usage()
{
	cerr << "Usage: millicom <index_file> [program_file] [--out-dir <dir>]" << endl;
	cerr << "  <index_file>       argv[1]: the .ind to load (the compiler/program)." << endl;
	cerr << "  <program_file>     argv[2] (optional): a single program file fed into" << endl;
	cerr << "                     Lex.Lexing (parsed/run live through the loaded compiler)." << endl;
	cerr << "  --out-dir <dir>    Directory prepended to any OAP-emitted files (a .ind via" << endl;
	cerr << "                     MnemoTable.IndVectWrite, MnemoTable.json via JsonSave)." << endl;
}

int main(int argc, char* argv[])
{
	std::string indPath; //= "C:\\Users\\hacker\\Downloads\\millicom\\oap2\\CompileCC.ind";
	if (argc < 2 && !indPath.size())
	{
		Usage();
		return 1;
	}
	else {
#if defined(_WIN32)
		SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
		_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
		_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif
	}

	if (argc >= 2) {
		string a1 = argv[1];
		if (a1 == "--help" || a1 == "-h") { Usage(); return 0; }
	}

	if(indPath.empty()) indPath = argv[1];

	// argv[1] is the index (.ind). argv[2], if present, is the single program file.
	string lexFile;
	for (int i = 2; i < argc; i++)
	{
		string a = argv[i];
		if (a == "--out-dir" && i + 1 < argc)
		{
			string d = argv[++i];
			if (!d.empty() && d.back() != '\\' && d.back() != '/') d += '\\';
			JSON_OAConeverter::OutDir = d;
		}
		else if (a == "--help" || a == "-h")
		{
			Usage();
			return 0;
		}
		else if (!a.empty() && a[0] != '-' && lexFile.empty())
		{
			lexFile = a; // the single positional program file (argv[2])
		}
		else
		{
			cerr << "Unknown or unexpected argument: " << a << endl;
			Usage();
			return 1;
		}
	}
	ifstream f(indPath);
	if (!f)
	{
		cerr << "Error: file not found: " << indPath << endl;
		return 1;
	}
	f.close();

	BusFU Bus;

	const char* busArgv[3] = { argv[0], argv[1], lexFile.c_str() };
	int busArgc = lexFile.empty() ? 2 : 3;
	Bus.ProgFU(200, { Cint, &busArgc });
	Bus.ProgFU(203, { Cchar, (char**)busArgv });

	string STR = indPath;
	Bus.ProgFU(10, { Cstring, &STR }); // load+execute the .ind; its baked tail lexes argv[2]
	return 0;
}
