// StrGen.cpp: определяет точку входа для консольного приложения.

#include "stdafx.h"
#include "Consts.h"
#include "Bus.h"
#include "Console.h"
#include "StrGen.h"
#include "Lex.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
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
	cerr << "Usage: millicom <index_file> [--lex-file <path>] [--lex <text>]" << endl;
	cerr << "  --lex-file <path>  After running the index file, feed the contents of <path>" << endl;
	cerr << "                     into Lex.Lexing (i.e. parse it through the loaded compiler)." << endl;
	cerr << "  --lex <text>       Same, but the program text is given inline." << endl;
}

int main(int argc, char* argv[])
{
	std::string indPath;
	if (argc < 2)
	{
		Usage();
		return 1;
	}
	else {
#if defined(_WIN32)
		// Suppress CRT assertion popups and Windows error dialogs; route asserts to
		// stderr instead so test automation isn't blocked by modal boxes.
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

	indPath = argv[1];
	string lexInput;
	bool haveLexInput = false;

	for (int i = 2; i < argc; i++)
	{
		string a = argv[i];
		if (a == "--lex-file" && i + 1 < argc)
		{
			ifstream lf(argv[++i], ios::binary);
			if (!lf)
			{
				cerr << "Error: --lex-file not found: " << argv[i] << endl;
				return 1;
			}
			stringstream ss;
			ss << lf.rdbuf();
			lexInput = ss.str();
			haveLexInput = true;
		}
		else if (a == "--lex" && i + 1 < argc)
		{
			lexInput = argv[++i];
			haveLexInput = true;
		}
		else if (a == "--help" || a == "-h")
		{
			Usage();
			return 0;
		}
		else
		{
			cerr << "Unknown argument: " << a << endl;
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
	string STR;
	StreamFloatALU ALU;

	Bus.ProgFU(200, { Cint, &argc });
	Bus.ProgFU(203, { Cchar, argv });

	STR = indPath;
	Bus.ProgFU(10, { Cstring, &STR });

	if (haveLexInput)
	{
		// Find the Lex FU (FUtype == 3) and call its Lexing MK (=100) on the supplied text.
		long int lexGlobalMk = -1;
		for (size_t i = 2; i < Bus.FUs.size(); i++) // 0,1 are self/Bus stubs
		{
			if (Bus.FUs[i] && Bus.FUs[i]->GetFuType() == 3)
			{
				lexGlobalMk = Bus.FUMkRange * (long int)i;
				break;
			}
		}
		if (lexGlobalMk < 0)
		{
			cerr << "Error: --lex-file given but no Lex FU was registered by the .ind. "
				"Did the index file load a compiler that creates a Lex FU?" << endl;
			return 2;
		}
		Bus.ProgFU(lexGlobalMk + 100, { Cstring, &lexInput });
	}

	return 0;
}
