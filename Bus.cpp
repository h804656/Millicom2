#include "stdafx.h"
#include "consts.h"
#include "Bus.h"
#include "FUini.h"
#include "Lex.h"
#include "List.h"
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "IndFileConvertOld.h"

using namespace std;

void BusFU::FUTypesIni()
{
	FUs.push_back(nullptr);
	FUs.push_back(nullptr);
	FUTypeCorrect = 0;
}

void BusFU::ProgFU(long int MK, LoadPoint Load, FU* Sender)
{
	ICVect* ipVect; // ��������� �� ������ ��
	if (MK >= FUMkRange*2)
	{
		int FU_num = MK / FUMkRange;
		if(FU_num<FUs.size())
			FUs[FU_num]->ProgFU(MK, Load, this);
	}
	else
		switch (MK%FUMkRange)
		{
		case 0: // �����
	    	for (int i = 1; i < FUs.size(); i++)
				delete FUs[i];
			FUs.clear();
			FUs.push_back(this);
			FUs.push_back(this); // ������ �� - ��� ��� Bus
			break;
		case 1: // MakeFU ������� ��
			FUs.push_back(FUTypes.MakeFu(Load.toInt(),this, FUTempl));
			FUs.back()->FUMkGlobalAdr = FUMkRange * (FUs.size() - 1); // ���������� ������ ����������� ��������� ��
			break;
		case 5: // ProgExec ��������� ��������� �� ��
			ProgExec((vector<ip>*)Load.Point);
			break;
		case 10: // FileOldProgExec ��������� ��������� �� �����
			ipVect = ConvIndOld(*(string*)Load.Point);
			if(ipVect->size()>0) ProgExec( (*ipVect)[0], 0, this,nullptr);
			break;
		case 20: // NFUOut

			Load.Write((int)FUs.size());
			break;
		case 21: // NFUOutMk
		{
			long int t= FUs.size();
			MkExec(Load, {Cint,&t});
			break;
		}
		case 22: // LastFuContextOut 
			Load.Write(FUs.back());
			break;
		case 23: // LastFuContextOutMk ������ �� � ���������� ���������� ���������� ��
			MkExec(Load, {TFU, FUs.back()});
			break;
		case 25: // LastFUNameSet ���������� ��� ���������� ���������� ��
			if (FUs.size() > 1)
				FUs.back()->FUName = Load.toStr();
			break;
		case 40: // LastFUMkRangeOut ������ ������ ��-��������� ���������� ��
			Load.Write(FUMkRange*(FUs.size() - 1));
			break;
		case 41: // LastFUMkRangeOutMk ������ �� � ������� ��-��������� ���������� �� 
			{
				long int t = FUMkRange * (FUs.size() - 1);
				MkExec(Load, { Cint, &t });
			}
			break;

		case 45: // FuContextOut ������ �������� �� �� ��� ��
			if (Ind >= FUs.size() || Ind < 0) break;
			Load.Write(FUs[Load.toInt() / FUMkRange]);
			break;
		case 46: // FuContextOutMk ������ �� � ���������� �� �� ��� ��
			if (Ind >= FUs.size() || Ind<0) break;
			MkExec(Load, { TFU, &FUs[Ind] });
			break;
		case 47: // FuContestFormInd1ToInd2OutMk ��������������� ������ �� � ����������� �� � ������� 1 �� ������� 2
			if (Ind >= FUs.size() || Ind < 0 || Ind2 >= FUs.size() || Ind2 < 0) break;
			for (int i = Ind; i <= Ind2; i++)
				MkExec(Load, { TFU,&FUs[i] });
			break;
		case 48: // FuContestFormInd1ToEndOutMk ��������������� ������ �� � ����������� �� � ������� 1 �� ����� ������ ��
			if (Ind >= FUs.size() || Ind < 0) break;
			for (int i = Ind; i <= FUs.size(); i++)
				MkExec(Load, { TFU,&FUs[i] });
			break;
		case 50: // IndSet ���������� ������ ��
			Ind = Load.toInt(0);
			break;
		case 51: // Ind2Set ���������� ������ ������ ��
			Ind2 = Load.toInt(0);
			break;
		case 52: // IndByMkSet ���������� ������ �� �� ��
			Ind = Load.toInt(0) / FUMkRange;
			break;
		case 53: // Ind2ByMkSet ���������� ������ ������ �� �� ��
			Ind2 = Load.toInt(0) / FUMkRange;
			break;
		case 54: // IndAdd ��������� ������ ��
			Ind += Load.toInt(1);
			break;
		case 55: // Ind2Add ��������� ������ ������ ��
			Ind2 += Load.toInt(1);
			break;

		case 100: // MkExec ��������� ���� �� (� �������� ������ �� ��)
			if (Load.Point!=nullptr && Load.isIP())
				ProgFU(((ip*)Load.Point)->atr, ((ip*)Load.Point)->Load);
			break;
		case 105: //InterpretatorModeSet ���������� ����� ��������������
			InterpretatorMode = Load.toBool(true);
			break;
		case 106: // InterpretatorProgExec ��������� ���������, ���� ����� ��������������
			if (InterpretatorMode)
				ProgExec(Load);
			break;
		case 107: // CompilyatorProgExec ��������� ���������, ���� ����� �����������
			if (!InterpretatorMode)
				ProgExec(Load);
			break;
		case 155:// FUTypeCorrectSet ���������� ��������� ������ ���� �� (��� �������� ��-��������� �� ������ ��-���������)
			FUTypeCorrect = Load.toInt();
			break;

		case 200: // ArgcSet ���������� ���������� ���������� ��������� ������
			argc = Load.toInt();
			break;
		case 201: // ArgcOut ������ ���������� ���������� ��������� ������
			Load.Write(argc);
			break;
		case 202: // ArgcOutMk ������ �� � ����������� ���������� ��������� ������
			MkExec(Load, {Cint,&argc});
			break;
		case 203: // ArgvSet ���������� ��������� ��������� ������
			if (argc <= 0) break;
			if (Load.isChar()) {
				argInd = 0;
				for (int i = 0; i < argc; i++)
				{
					argv.push_back(((char**)Load.Point)[i]);
				}
			}
			break;
		case 204: // ArgvOut ������ ���������� ���������� ��������� ������
			if (argc >= 0 && argc < argv.size())
				Load.Write(argv[argInd]);
			break;
		case 207: // ArgvOutMk ������ �� � ����������� ���������� ��������� ������
			if (argc >= 0 && argc < argv.size())
				MkExec(Load, { Cstring, &argv[argInd] });
			break;
		case 206: // ArgByIndOut ������ �������� ��������� ������ �� �������
			if (argc >= 0 && argc < argv.size())
				Load.Write(argv[argInd]);
			break;
		case 208: // ArgByIndOutMk  ������ �� � ���������� ��������� ������ �� �������
			if (argInd >= 0 && argInd < argv.size())
				MkExec(Load, { Cstring, &argv[argInd] });
			break;
		case 210: // ArgIndSet ���������� ������ ���������
			argInd = Load.toInt();
			break;
		case 215: // ArgcLessExec ���������, ���� ���������� ���������� ������ �������� � ��������
			if (Load.toInt() &&	Load.toInt() > argc)
					ProgExec(Prog);
			break;
		case 216: // ArgcBiggerEq ���������, ���� ���������� ���������� ������ ��� ����� �������� � ��������
			if (Load.toInt() && Load.toInt() <= argc)
				ProgExec(Prog);
			break;

		// CompileCC.oap accumulates IP head-strings into CapsIps via
		// IpBufEmit / IpBufEmitStashed / AleEmit; IpBufWrite serializes the
		// buffer to a .ind file compatible with IndFileConvertOld.
		case 253: // IpBufWrite -- serialize CapsIps to a .ind file
			if (Load.Point != nullptr) {
				string path = Load.toStr();
				ofstream out(path);
				if (out) {
					// CapsIps is a linear emission. Sub-capsules are delimited
					// by sentinel marker IPs (SubCapOpenAtr / SubCapCloseAtr)
					// that the OAP layer emits at `{` and `}`. We make two
					// passes: pass 1 assigns each non-marker IP to a capsule
					// (0=main, 1+=sub-capsules in encounter order), records
					// the parent IP just before each open, and computes the
					// total output index for each IP (skipping markers). Pass
					// 2 writes the IPs with f1/f3/f4 derived from the capsule
					// layout.
					int M = (int)CapsIps.size();
					// Per-source-index data, only valid when !isMarker[i]:
					//   capId[i] = which capsule the IP belongs to
					//   outIdx[i] = global output index (linearized over
					//               main then capsule 1, 2, ...)
					std::vector<int> capId(M, 0), outIdx(M, -1);
					std::vector<bool> isMarker(M, false);
					std::vector<int>  parentSrc; // source index of each sub-cap's parent IP
					std::vector<int>  stack = { 0 };
					int nextCapId = 0;
					for (int i = 0; i < M; i++) {
						// Sniff the head string's leading int (the atr).
						long atr = std::strtol(CapsIps[i].c_str(), nullptr, 10);
						if (atr == SubCapOpenAtr) {
							isMarker[i] = true;
							nextCapId++;
							// Parent of this sub-cap is the most recent non-marker
							// IP in the enclosing capsule.
							int parentCap = stack.back();
							int p = i - 1;
							while (p >= 0 && (isMarker[p] || capId[p] != parentCap)) p--;
							parentSrc.push_back(p);
							stack.push_back(nextCapId);
						} else if (atr == SubCapCloseAtr) {
							isMarker[i] = true;
							if (stack.size() > 1) stack.pop_back();
						} else {
							capId[i] = stack.back();
						}
					}
					// Group non-marker IPs by capsule, in source order.
					int nCaps = nextCapId + 1;
					std::vector<std::vector<int>> groups(nCaps);
					for (int i = 0; i < M; i++)
						if (!isMarker[i]) groups[capId[i]].push_back(i);
					// Assign linearized output indices: main (cap 0) first,
					// then sub-caps in encounter order.
					int N = 0;
					std::vector<int> capStart(nCaps);
					for (int c = 0; c < nCaps; c++) {
						capStart[c] = N;
						for (int src : groups[c]) outIdx[src] = N++;
					}
					out << N << "\n";
					for (int c = 0; c < nCaps; c++) {
						auto& g = groups[c];
						int n = (int)g.size();
						for (int j = 0; j < n; j++) {
							int src = g[j];
							int gi  = outIdx[src];
							// f1 default = (gi<<2)+2; if this IP is a sub-cap
							// parent, override to point at sub-cap's first IP.
							int f1 = (gi << 2) + 2;
							for (size_t k = 1; k < parentSrc.size() + 1 && k < (size_t)nCaps; k++) {
								if (parentSrc[k - 1] == src && !groups[k].empty()) {
									f1 = (capStart[k] << 2) + 0;
									break;
								}
							}
							int f2 = 11;
							int f3 = (j == n - 1) ? -1 : ((outIdx[g[j + 1]]) << 2);
							int f4 = (j == 0)     ? -1 : ((outIdx[g[j - 1]]) << 2);
							out << CapsIps[src] << " " << f1 << " " << f2
							    << " " << f3 << " " << f4 << "\n";
						}
					}
				}
			}
			break;
		case 256: // IpBufSetAtr -- buffer the atr for the next IpBufEmit
			CapsPendingAtr = Load.toInt();
			break;
		case 257: // CapsEmit -- emit one IP using the buffered atr + Load value
		{
			// Guard: atr=0 means no FU.MK was resolved on the LHS (Atr= path),
			// so skip the emit. CompileCC.oap zeroes MkCalc at end-of-statement
			// so this naturally drops non-FU statements.
			if (CapsPendingAtr == 0) break;
			// Track sub-capsule nesting via the open/close sentinel markers so
			// the live-dispatch paths can suppress execution of row-body IPs.
			if (CapsPendingAtr == SubCapCloseAtr && SubCapDepth > 0) SubCapDepth--;
			ostringstream os;
			os << CapsPendingAtr << " ";
			unsigned int dt = Load.Type >> 1;
			if (dt == Dstring || dt == Dchar) {
				os << "S:" << Load.toStr() << "\"";
			}
			else if (dt == Dbool) {
				os << "B:" << (Load.toBool() ? "T" : "F");
			}
			else if (dt == Dfloat || dt == Ddouble) {
				os << "D:" << Load.toDouble();
			}
			else {
				// default to int (covers Cint, Cchar-as-int, Tvoid etc.)
				os << "I:" << Load.toInt();
			}
			CapsIps.push_back(os.str());
			if (CapsPendingAtr == SubCapOpenAtr) SubCapDepth++;
			break;
		}
		case 258: // CapsStashLoad -- save Load (with type) for the next stashed emit
			CapsStashLoad = Load.Clone();
			// If the Mnemo MnemoTable row just set PendingFuNameMode, the
			// first string Load we see is the user-FU's name. Capture it.
			if (PendingFuNameMode) {
				unsigned int dt = Load.Type >> 1;
				if (dt == Dstring || dt == Dchar) {
					PendingFuName = Load.toStr();
					PendingFuNameMode = false;
				}
			}
			break;
		case 259: // CapsEmitStashed -- emit using the saved CapsStashLoad value
		{
			if (IpBufPendingMakeFU) {
				// FUType= path inside a NewFU block: emit a MakeFU IP using the
				// stashed Load's integer value, regardless of CapsPendingAtr.
				ostringstream os;
				os << (FUMkRange + 1) << " I:" << CapsStashLoad.toInt();
				CapsIps.push_back(os.str());
				IpBufPendingMakeFU = false;
				// LIVE: actually create the FU on the bus so subsequent user
				// `<name>.<MK>=value` parses dispatch into a real backing FU.
				long fuType = CapsStashLoad.toInt();
				LoadPoint typeLp = { Cint, &fuType };
				ProgFU(1, typeLp, Sender); // recursive MakeFU
				long realRange = (long)(FUs.size() - 1) * FUMkRange;
				// Register MnemoTable row with the REAL runtime FU range.
				if (!PendingFuName.empty()) {
					addUserFuMnemoRow(PendingFuName, (int)fuType, realRange);
					PendingFuName.clear();
				}
				break;
			}
			if (CapsPendingAtr == 0) break;
			ostringstream os;
			os << CapsPendingAtr << " ";
			unsigned int dt = CapsStashLoad.Type >> 1;
			if (dt == Dstring || dt == Dchar) {
				os << "S:" << CapsStashLoad.toStr() << "\"";
			}
			else if (dt == Dbool) {
				os << "B:" << (CapsStashLoad.toBool() ? "T" : "F");
			}
			else if (dt == Dfloat || dt == Ddouble) {
				os << "D:" << CapsStashLoad.toDouble();
			}
			else {
				os << "I:" << CapsStashLoad.toInt();
			}
			CapsIps.push_back(os.str());
			// LIVE: dispatch the IP so the program executes as it's compiled.
			// Suppressed inside `>{...}` / `={...}` row bodies (SubCapDepth>0)
			// since those IPs belong to the row and should only fire when the
			// row is matched at runtime.
			if (SubCapDepth == 0)
				ProgFU(CapsPendingAtr, CapsStashLoad, Sender);
			break;
		}
		// ALE compile-time shunting-yard evaluator.
		case 270: // AleReset -- start fresh per-statement
			AleVals.clear();
			AleOps.clear();
			break;
		case 271: // AleOpSet -- operator arrives. Pop higher/equal-precedence ops first.
		{
			int newOp = (int)Load.toInt();
			auto prec = [](int op) {
				if (op == 1 || op == 2) return 1;            // + -
				if (op == 3 || op == 4 || op == 5) return 2; // * / %
				return 0;
			};
			while (!AleOps.empty() && AleOps.back() != 100
				&& prec(AleOps.back()) >= prec(newOp)) {
				int op = AleOps.back(); AleOps.pop_back();
				if (AleVals.size() < 2) break;
				long int b = AleVals.back(); AleVals.pop_back();
				long int a = AleVals.back(); AleVals.pop_back();
				long int r = 0;
				switch (op) {
					case 1: r = a + b; break;
					case 2: r = a - b; break;
					case 3: r = a * b; break;
					case 4: r = b ? (a / b) : 0; break;
					case 5: r = b ? (a % b) : 0; break;
				}
				AleVals.push_back(r);
			}
			AleOps.push_back(newOp);
			break;
		}
		case 272: // AleApply -- operand arrives, push to operand stack
			AleVals.push_back(Load.toInt());
			break;
		case 273: // AleParenIn -- push paren sentinel; blocks operator-popping
			AleOps.push_back(100);
			break;
		case 274: // AleParenOut -- drain ops down to and including the matching `(`
		{
			while (!AleOps.empty() && AleOps.back() != 100) {
				int op = AleOps.back(); AleOps.pop_back();
				if (AleVals.size() < 2) break;
				long int b = AleVals.back(); AleVals.pop_back();
				long int a = AleVals.back(); AleVals.pop_back();
				long int r = 0;
				switch (op) {
					case 1: r = a + b; break;
					case 2: r = a - b; break;
					case 3: r = a * b; break;
					case 4: r = b ? (a / b) : 0; break;
					case 5: r = b ? (a % b) : 0; break;
				}
				AleVals.push_back(r);
			}
			if (!AleOps.empty()) AleOps.pop_back(); // pop the `(` sentinel
			break;
		}
		case 281: // IpBufPendingMakeFU -- next IpBufEmitStashed will also emit a MakeFU IP
			IpBufPendingMakeFU = true;
			break;
		case 286: // PendingFuNameModeSet -- next stashed string Load becomes the user-FU name
			PendingFuNameMode = true;
			// Sender of this MK IS MnemoTable (it's called from the Mnemo row
			// body in MnemoTable). Capture its pointer on first call so
			// IpBufEmitStashed can insert the user-FU row directly later.
			if (MnemoTableFu == nullptr && Sender != nullptr) {
				MnemoTableFu = Sender;
			}
			break;
		case 275: // AleEmit -- drain remaining ops; with Load!=0 dispatch the
			// result via Load as MK (for compile-time consumers like a
			// bracketed var-init `A=[1+Z]`), otherwise emit `<atr> I:<result>`
			// when CapsPendingAtr is set.
		{
			long int loadMk = Load.toInt();
			if (loadMk == 0 && CapsPendingAtr == 0) {
				AleVals.clear(); AleOps.clear(); break;
			}
			while (!AleOps.empty()) {
				int op = AleOps.back(); AleOps.pop_back();
				if (op == 100) continue;
				if (AleVals.size() < 2) break;
				long int b = AleVals.back(); AleVals.pop_back();
				long int a = AleVals.back(); AleVals.pop_back();
				long int r = 0;
				switch (op) {
					case 1: r = a + b; break;
					case 2: r = a - b; break;
					case 3: r = a * b; break;
					case 4: r = b ? (a / b) : 0; break;
					case 5: r = b ? (a % b) : 0; break;
				}
				AleVals.push_back(r);
			}
			long int result = AleVals.empty() ? 0 : AleVals.back();
			if (loadMk != 0) {
				MkExec(Load, { Cint, &result });
			} else {
				ostringstream os;
				os << CapsPendingAtr << " I:" << result;
				CapsIps.push_back(os.str());
				// LIVE: dispatch the result so e.g. `Cons.OutLn=A+B` prints.
				// Suppressed inside row-body sub-capsules (see SubCapDepth note).
				if (SubCapDepth == 0) {
					LoadPoint resultLp = { Cint, &result };
					ProgFU(CapsPendingAtr, resultLp, Sender);
				}
			}
			AleVals.clear(); AleOps.clear();
			break;
		}

		default:
			CommonMk(MK, Load);
			break;
		}
}

void BusFU::addUserFuMnemoRow(const std::string& name, int type, long range)
{
	// At MakeFU emit time, register the user FU in MnemoTable so subsequent
	// `<name>.<MK>` lookups go through the normal MnemoTable.FindAnd path --
	// the same path used for built-in mnemos like `Cons` and `Bus`. We extract
	// the MkTable.Set IP from a built-in peer of the same FU type (Cons for
	// FUConsNew, Bus for FUBusNew, ...) so the user FU's row dispatches with
	// the right method table.
	if (MnemoTableFu == nullptr) return;
	// Lazy-resolve Lex pointer by FUtype (Lex = FUtype 3).
	if (LexFuPtr == nullptr) {
		for (size_t i = 2; i < FUs.size(); i++) {
			if (FUs[i] && FUs[i]->GetFuType() == 3) {
				LexFuPtr = FUs[i];
				break;
			}
		}
		if (LexFuPtr == nullptr) return;
	}
	int lexFuNum = -1;
	for (size_t i = 2; i < FUs.size(); i++)
		if (FUs[i] == (FU*)LexFuPtr) { lexFuNum = (int)i; break; }
	if (lexFuNum < 0) return;
	long lexBase = (long)lexFuNum * FUMkRange;
	long lexSendMk = lexBase + 70; // Lex.SendToReceiver MK
	List* mt = (List*)MnemoTableFu;
	if (mt->ListHead.empty() || mt->ListHead.back() == nullptr) return;
	IC_type rows = mt->ListHead.back();
	// Find a built-in peer FU of the same type (e.g. Cons at FU#=2 for
	// FUConsNew users) and use ITS range to locate the right MnemoTable row.
	long peerRange = -1;
	for (size_t i = 2; i < FUs.size(); i++) {
		if (FUs[i] && FUs[i]->GetFuType() == type) {
			peerRange = (long)i * FUMkRange;
			break;
		}
	}
	if (peerRange < 0) return;
	ip srcMkTableIp{ 0, 0, nullptr };
	bool foundPeer = false;
	for (auto& rowIp : *rows) {
		if (rowIp.Load.Point == nullptr) continue;
		IC_type fields = (IC_type)rowIp.Load.Point;
		bool rowMatches = false;
		for (auto& f : *fields) {
			if (f.atr != lexSendMk || f.Load.Point == nullptr) continue;
			IC_type sub = nullptr;
			unsigned int loadDt = f.Load.Type >> 1;
			if (loadDt == DIC) sub = (IC_type)f.Load.Point;
			else if (loadDt == DIP) {
				ip* tip = (ip*)f.Load.Point;
				if (tip && (tip->Load.Type >> 1) == DIC)
					sub = (IC_type)tip->Load.Point;
			}
			if (!sub) continue;
			for (auto& s : *sub) {
				if (s.atr == -300 && s.Load.toInt() == peerRange) {
					rowMatches = true; break;
				}
			}
			if (rowMatches) break;
		}
		if (!rowMatches) continue;
		// Pluck the MkTable.Set IP (positive atr, non-Lex, *Set MK).
		for (auto& f : *fields) {
			if (f.atr <= 0) continue;
			long fuNum = f.atr / FUMkRange;
			if (fuNum == lexFuNum) continue;
			if (f.atr % FUMkRange != 1) continue;
			srcMkTableIp = f;
			foundPeer = true;
			break;
		}
		if (foundPeer) break;
	}
	if (!foundPeer) return;
	// Build the new row: { Mnemo=<name>, MkTable.Set=<peer's table>,
	// Lex.SendToReceiver={FU=<range>} }.
	IC_type newRow = new vector<ip>();
	// Mnemo=<name>
	newRow->push_back({ -2 /*Mnemo*/, Cstring, new std::string(name) });
	// MkTable.Set=<peer's table-load> -- share the Load by value (same Point).
	newRow->push_back({ srcMkTableIp.atr, srcMkTableIp.Load.Type, srcMkTableIp.Load.Point });
	// Lex.SendToReceiver={FU=<range>}: load is a TIC pointing at the
	// sub-IC (same structure as the built-in mnemo rows produce).
	IC_type fuSub = new vector<ip>();
	fuSub->push_back({ -300 /*FU*/, Cint, new long int(range) });
	newRow->push_back({ lexSendMk, TIC, fuSub });
	rows->push_back({ -6 /*LineAtr*/, TIC, newRow });
}

FU* BusFU::Copy() // ��������� ����������� ��
{
	return new BusFU(Bus, this);
}

FU* BusFU::TypeCopy() // ������� �� ������ �� ���� (�� ������� ��������
{
	return new BusFU(Bus, nullptr);
}