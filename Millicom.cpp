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
	// --lex-file and --lex may appear multiple times; each chunk is fed
	// to Lex.Lexing in argv order. The self-host bootstrap relies on this:
	// `--lex-file BootstrapCC.oap --lex-file CompileCC.oap` first
	// populates MnemoTable with built-in atr names, then runs the rest.
	vector<string> lexInputs;
	string outFile;
	bool haveOutFile = false;

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
			lexInputs.push_back(ss.str());
		}
		else if (a == "--lex" && i + 1 < argc)
		{
			lexInputs.push_back(argv[++i]);
		}
		else if (a == "--out-file" && i + 1 < argc)
		{
			outFile = argv[++i];
			haveOutFile = true;
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

	// After the .ind bootstraps the compiler, inject mnemo rows for the
	// bootstrap List FUs (Stack, MnemoTable) so user OAP code can address
	// them by name. Delphi's compile of CompileCC.oap can't carry these
	// rows directly because it can't tolerate two `MkTable.Set=X!` lines
	// referencing the same table; doing it from C++ at load time bypasses
	// that constraint.
	Bus.InjectBuiltinMnemos();

	if (!lexInputs.empty())
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
		// Feed each chunk in argv order. Lex.Lexing (case 100) appends " \n"
		// internally to flush any pending token between chunks.
		for (auto& chunk : lexInputs)
		{
			Bus.ProgFU(lexGlobalMk + 100, { Cstring, &chunk });
		}
	}

	if (haveOutFile)
	{
		// After the Lex pass, dump the compiler's accumulated capsule buffer
		// (Bus MKs 250..) to <path> as a .ind file. This is how CompileCC.oap
		// emits its compiled output.
		Bus.ProgFU(1253, { Cstring, &outFile });
	}

	return 0;
}
