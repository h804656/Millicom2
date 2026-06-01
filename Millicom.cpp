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
#include "IndexFile.h"
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
	std::string indPath; //= "C:\\Users\\hacker\\Downloads\\millicom\\oap2\\CompileCC.ind";
	vector<string> lexInputs; // = {"CompileCC-self.oap"};
	if (argc < 2 && !indPath.size())
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

	if(indPath.empty()) indPath = argv[1];
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

	if (haveOutFile)
	{
		// Emission mirrors Delphi main.pas:480-493, which (post-compile) CLI-injects
		// `GatewayFile.FileNameSet=...` then `Bus={CapsManager.IndexVectPopMk=GatewayFile.IndexVectWrite}`.
		// We inject the same shape against the IndexFile FU, split into Delphi's two phases:
		//   1. BUILD  -- CapsList.IndexVectPopMk = IndexFile.IndexVectFromList  (CapsManager role)
		//   2. WRITE  -- IndexFile.IndexVectWrite                               (GatewayFile role)
		// The serialization layout lives in the FU primitives (as it does on the Delphi/Pascal
		// side); only the orchestration is bus-call injection.
		IndexFile* idx = new IndexFile(&Bus, nullptr);
		Bus.FUs.push_back(idx);
		long idxAddr = Bus.FUMkRange * (long)(Bus.FUs.size() - 1);
		idx->FUMkGlobalAdr = idxAddr;
		idx->FileName = outFile;                 // == GatewayFile.FileNameSet
		if (Bus.capsListFu != nullptr) {
			long buildMk = idxAddr + 22;         // IndexFile.IndexVectFromList
			Bus.capsListFu->ProgFU(256, { Cint, &buildMk }, nullptr); // phase 1: CapsList.IndexVectPopMk=...
		}
		idx->ProgFU(idxAddr + 21, { Cint, nullptr }, nullptr);        // phase 2: IndexFile.IndexVectWrite
	}

	return 0;
}
