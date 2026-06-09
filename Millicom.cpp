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
	cerr << "Usage: millicom <index_file> [--lex-file <path>] [--lex <text>]" << endl;
	cerr << "  --lex-file <path>  After running the index file, feed the contents of <path>" << endl;
	cerr << "                     into Lex.Lexing (i.e. parse it through the loaded compiler)." << endl;
	cerr << "  --lex <text>       Same, but the program text is given inline." << endl;
	cerr << "  --run <path>       Compile & run <path>, printing only the program's output" << endl;
	cerr << "                     (the compiler's own state-trace is suppressed)." << endl;
	cerr << "  --out-dir <dir>    Directory prepended to both emitted files: the .ind" << endl;
	cerr << "                     (IndVectWrite) and MnemoTable.json (JsonSave)." << endl;
}

int main(int argc, char* argv[])
{
	std::string indPath; //= "C:\\Users\\hacker\\Downloads\\millicom\\oap2\\CompileCC.ind";
	vector<string> lexInputs; //= {"oap2\\CompileCC-self.oap"};
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
	for (string& s : lexInputs) {
		ifstream lf(s, ios::binary);
		if (!lf)
		{
			cerr << "Error: --lex-file not found: " << s << endl;
			return 1;
		}
		stringstream ss;
		ss << lf.rdbuf();
		s = ss.str();
	}

	if (argc >= 2) {
		string a1 = argv[1];
		if (a1 == "--help" || a1 == "-h") { Usage(); return 0; }
	}

	if(indPath.empty()) indPath = argv[1];
	string outFile;
	bool haveOutFile = false;
	bool runMode = false;

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
		else if (a == "--run" && i + 1 < argc)
		{
			ifstream rf(argv[++i], ios::binary);
			if (!rf)
			{
				cerr << "Error: --run file not found: " << argv[i] << endl;
				return 1;
			}
			stringstream ss;
			ss << rf.rdbuf();
			lexInputs.push_back(ss.str());
			runMode = true;
		}
		else if (a == "--out-file" && i + 1 < argc)
		{
			outFile = argv[++i];
			haveOutFile = true;
		}
		else if (a == "--out-dir" && i + 1 < argc)
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

	string runTempOut;
	if (runMode)
	{
		for (size_t i = 0; i < Bus.FUs.size(); i++)
			if (Bus.FUs[i] && Bus.FUs[i]->GetFuType() == 1)
				static_cast<Console*>(Bus.FUs[i])->Quiet = true;
		if (!haveOutFile)
		{
			if (!JSON_OAConeverter::OutDir.empty())
				runTempOut = "run.tmp.ind";
			else
				runTempOut = indPath + ".run.tmp.ind";
			outFile = runTempOut;
			haveOutFile = true;
		}
	}

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
		for (auto& chunk : lexInputs)
		{
			Bus.ProgFU(lexGlobalMk + 100, { Cstring, &chunk });
		}
	}

	if (haveOutFile && List::sCapsList != nullptr)
	{
		List* cl = (List*)List::sCapsList;
		cl->ProgFU(609, { Cstring, &outFile }); // IndFileNameSet
		cl->ProgFU(612, { Cint, nullptr });     // build (IndVectFromList)
		cl->ProgFU(613, { Cint, nullptr });     // write (IndVectWrite)
	}

	if (runMode && !runTempOut.empty())
	{
		BusFU RunBus;
		RunBus.ProgFU(200, { Cint, &argc });
		RunBus.ProgFU(203, { Cchar, argv });
		string rp = JSON_OAConeverter::OutDir + runTempOut;
		RunBus.ProgFU(10, { Cstring, &rp });
		std::remove(rp.c_str());
	}

	return 0;
}
