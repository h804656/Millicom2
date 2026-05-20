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
			// Use the FU's own FUMkGlobalAdr so MkBegRange overrides are
			// reflected, falling back to the computed default when no
			// FUs are registered (FUMkGlobalAdr defaults to 0).
			Load.Write(FUs.size() > 1 ? FUs.back()->FUMkGlobalAdr
			                          : FUMkRange * (FUs.size() - 1));
			break;
		case 41: // LastFUMkRangeOutMk ������ �� � ������� ��-��������� ���������� ��
			{
				long int t = FUs.size() > 1 ? FUs.back()->FUMkGlobalAdr
				                            : FUMkRange * (FUs.size() - 1);
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
			// Set to true by the NewFU rewrite path so we skip the close-marker
			// write at the bottom of this case (we already rewrote the parent
			// IP and dropped the body, so the close marker isn't needed).
			bool skipEmit = false;
			// Sub-capsule close: pop the IC built up while we were inside,
			// then dispatch (or nest) the parent IP that opened it. Do this
			// BEFORE emitting the close marker / decrementing SubCapDepth so
			// the live-dispatch happens at the right outer depth.
			if (CapsPendingAtr == SubCapCloseAtr) {
				if (!IcStack.empty() && !ParentAtrStack.empty()) {
					vector<ip>* innerIc = IcStack.back();
					IcStack.pop_back();
					long int parentAtr = ParentAtrStack.back();
					ParentAtrStack.pop_back();
					size_t parentCapsIdx = CapsIpsParentIdxStack.empty()
						? (size_t)-1
						: CapsIpsParentIdxStack.back();
					if (!CapsIpsParentIdxStack.empty()) CapsIpsParentIdxStack.pop_back();
					LoadPoint icLoad = { TIC, innerIc };
					if (IcStack.empty()) {
						// Back to top level. If this close finishes a
						// `NewFU={...}` body (PendingMakeFU was set by the
						// FUType= field), do the actual MakeFU + MnemoTable
						// registration now -- the FU has to exist before
						// the parent IP (CreateNewFU.FindOr) dispatches,
						// since any post-NewFU user code expects to address
						// the new FU by name.
						bool didMakeFU = false;
						if (PendingMakeFU) {
							long int fuType = PendingMakeFUType;
							LoadPoint typeLp = { Cint, &fuType };
							ProgFU(1, typeLp, Sender);
							// Honor MkBegRange if present in the body IC. The
							// inner-MnemoTable's MkBegRange row dispatches
							// Atr=MkBegRange, where MkBegRange is the Delphi
							// built-in atr code -20 (verified empirically by
							// dumping a `NewFU={... MkBegRange=N ...}` IC).
							long int mkBegRange = 0;
							if (innerIc) {
								for (auto& ipEntry : *innerIc) {
									if (ipEntry.atr == -20) {
										mkBegRange = ipEntry.Load.toInt();
										break;
									}
								}
							}
							long int realRange =
								mkBegRange > 0
									? mkBegRange
									: (long)(FUs.size() - 1) * FUMkRange;
							if (mkBegRange > 0)
								FUs.back()->FUMkGlobalAdr = mkBegRange;
							if (!PendingFuName.empty()) {
								addUserFuMnemoRow(PendingFuName, (int)fuType, realRange);
								PendingFuName.clear();
							}
							// Record the compile-time → reload-time MK range
							// mapping for this user FU so subsequent emits get
							// rebased at serialization time.
							UserFuRanges.push_back({ realRange, NextReloadFuIdx * FUMkRange });
							NextReloadFuIdx++;
							PendingMakeFU     = false;
							PendingMakeFUType = 0;
							didMakeFU         = true;
						}
						// Only live-dispatch when the parent is a positive
						// FU.MK atr (negative atrs like -6 are pure markers).
						// Skip the dispatch if we just handled this as a
						// NewFU body -- CreateNewFU.FindOr's FUType row body
						// would otherwise fire MainBus.Create and double-
						// create the FU at an unwanted index.
						if (parentAtr > 0 && !didMakeFU) {
							ProgFU(parentAtr, icLoad, Sender);
						}
						else {
							delete innerIc;
						}
						// NewFU rewrite: replace the parent IP's CapsIps entry
						// (atr=CreateNewFU.FindOr, I:0 + sub-cap pointer) with
						// a synthetic `1001 I:<fuType>` Bus.MakeFU IP, and drop
						// the open marker + body field IPs we accumulated for
						// this sub-cap. CompileCC.oap:226-228 documents this
						// as the intended emit shape for NewFU. Without it the
						// produced .ind only replays when the full CompileCC
						// compiler is also loaded.
						if (didMakeFU
							&& parentCapsIdx != (size_t)-1
							&& parentCapsIdx < CapsIps.size())
						{
							long int fuType = PendingMakeFUType;
							// PendingMakeFUType was just zeroed; recover the
							// value from the load that ran ProgFU above.
							// Simpler: pull it back from the inner IC's -22
							// FUType entry. innerIc has been deleted, so we
							// instead recover from the CapsIps entries we are
							// about to truncate. Find the most recent "-22 I:"
							// line in [parentCapsIdx+1 .. end).
							for (size_t k = CapsIps.size(); k-- > parentCapsIdx + 1; ) {
								const std::string& s = CapsIps[k];
								if (s.compare(0, 6, "-22 I:") == 0) {
									fuType = std::strtol(s.c_str() + 6, nullptr, 10);
									break;
								}
							}
							std::ostringstream rew;
							rew << 1001 << " I:" << fuType;
							CapsIps[parentCapsIdx] = rew.str();
							// Drop everything after the parent IP (open marker,
							// body field IPs). The close marker hasn't been
							// pushed yet -- skip emitting it below.
							CapsIps.resize(parentCapsIdx + 1);
							// Tell the post-block emit path to skip the close
							// marker push (its body has been folded into the
							// rewritten parent IP above).
							skipEmit = true;
						}
					}
					else {
						// Still nested: the parent IP becomes a normal entry
						// in the outer IC with Load pointing at innerIc.
						IcStack.back()->push_back({ parentAtr, TIC, innerIc });
					}
				}
				if (SubCapDepth > 0) SubCapDepth--;
			}
			if (skipEmit) break;
			ostringstream os;
			os << rebaseAtr(CapsPendingAtr) << " ";
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
			if (CapsPendingAtr == SubCapOpenAtr) {
				// Open a fresh IC for the body; the most recent non-marker
				// IpBufEmit atr is this sub-cap's parent.
				SubCapDepth++;
				ParentAtrStack.push_back(PendingParentAtr);
				IcStack.push_back(new vector<ip>());
				// Track the parent IP's CapsIps index so SubCapClose can
				// rewrite it into `1001 I:<fuType>` when this turns out to
				// be a NewFU body. CapsIps just got the open-marker push;
				// the parent IP is the entry immediately before that.
				CapsIpsParentIdxStack.push_back(CapsIps.size() >= 2 ? CapsIps.size() - 2 : 0);
				PendingParentAtr = 0;
			}
			else if (CapsPendingAtr != SubCapCloseAtr) {
				// Regular non-marker IpBufEmit -- this IP is a candidate
				// parent for a sub-cap that may open on the next call.
				PendingParentAtr = CapsPendingAtr;
			}
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
				// FUType= field inside `NewFU={...}`: stash the type code so
				// the deferred MakeFU at the NewFU close can create the FU
				// and register its MnemoTable row. The body IP itself is
				// emitted via the normal path (atr=FUType, Load=type), so
				// the .ind output looks like a Delphi-compiled NewFU body
				// instead of carrying a synthetic `1001 I:<type>` MakeFU IP.
				//
				// Depth guard: CompileCC.oap's FUType MnemoTable row fires
				// IpBufPendingMakeFU=0 as a side effect EVERY time "FUType"
				// is looked up by name -- including when it appears as a
				// value RHS inside an unrelated row body (e.g. the
				// MnemoTable.Set= multi-row that rebuilds its own FUType
				// row). Only honor the flag when we're directly inside a
				// NewFU={...} body (SubCapDepth == 1); deeper depth means
				// nested context and the flag is spurious. Without this
				// guard the deferred MakeFU at the unrelated row's close
				// runs with garbage state and crashes.
				if (SubCapDepth == 1) {
					PendingMakeFU     = true;
					PendingMakeFUType = CapsStashLoad.toInt();
				}
				IpBufPendingMakeFU = false;
				// fall through to the regular emit below
			}
			if (CapsPendingAtr == 0) break;
			ostringstream os;
			os << rebaseAtr(CapsPendingAtr) << " ";
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
			// Inside a sub-cap body: push the IP into the in-construction IC
			// so the parent can dispatch with real content at its close.
			// Top-level (depth 0): live-dispatch so e.g. `Cons.OutLn="x"`
			// prints at parse-time.
			if (SubCapDepth > 0 && !IcStack.empty()) {
				IcStack.back()->push_back({ CapsPendingAtr, CapsStashLoad.Type, CapsStashLoad.Point });
			}
			else {
				ProgFU(CapsPendingAtr, CapsStashLoad, Sender);
			}
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
				os << rebaseAtr(CapsPendingAtr) << " I:" << result;
				CapsIps.push_back(os.str());
				// Inside a row-body sub-cap: push to the IC so the parent's
				// close-time dispatch sees the computed value. Top level:
				// live-dispatch as `Cons.OutLn=A+B` -> prints the result.
				if (SubCapDepth > 0 && !IcStack.empty()) {
					IcStack.back()->push_back({ CapsPendingAtr, Cint, new long int(result) });
				}
				else {
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

// SHIM: programmatic mnemo injection for built-in FUs (MnemoTable, Lex).
//
// Why this isn't done in CompileCC.oap:
//   The Delphi bootstrap compiler (E:\miemp\Millicom\Millicom\) silently
//   truncates the produced .ind file when CompileCC.oap contains more
//   than ~5 `MkTable.Set=Y!` forward-reference rows TOTAL across the
//   whole source -- not per-table, not per-target -- regardless of
//   whether the references resolve to the same or different targets.
//   The peer rows we already emit (Bus, Cons, IntAluPeer, ListPeer,
//   LexPeer) sit right at that limit. Adding rows to expose MnemoTable
//   and Lex by name pushes past it and the resulting .ind is unusable.
//
// What we tried and rejected:
//   Restructuring the registry to CompileNew6-style hierarchical
//   MkList + MkListFind (one parent table, child rows looked up
//   dynamically) is the right long-term shape and on paper fits under
//   the quota. Empirically Delphi truncated on that variant too -- the
//   references needed to wire the lookup itself put us back over the
//   same limit.
//
// Conclusion: this is a known shim until we self-host and drop Delphi.
// Once the C++ executor compiles CompileCC.oap end-to-end, redo this
// as hierarchical MkList + MkListFind in pure OAP and delete this
// function (plus its call site in Millicom.cpp).
void BusFU::InjectBuiltinMnemos()
{
	// CompileCC.oap declares Stack first then MnemoTable (both FUListNew),
	// so MnemoTable is the SECOND FUtype=5 instance. MnemoTableFu must be
	// set before any addUserFuMnemoRow call -- that's where rows append.
	int mnemoIdx = -1;
	int listCount = 0;
	for (size_t i = 2; i < FUs.size(); i++) {
		if (FUs[i] && FUs[i]->GetFuType() == 5) {
			listCount++;
			if (listCount == 2) { mnemoIdx = (int)i; break; }
		}
	}
	if (mnemoIdx < 0) return;
	MnemoTableFu = FUs[mnemoIdx];

	addUserFuMnemoRow("MnemoTable", 5, (long)mnemoIdx * FUMkRange);
	// Expose the bootstrap Lex (FUtype=3) so user code can reach
	// Lex.UnicAtrSet / UnicMkSet / SendToReceiver / etc. by name. Same
	// path as MnemoTable: addUserFuMnemoRow peer-matches against the
	// LexPeer row CompileCC.oap declares with MkTable.Set=LexMkTable.
	for (size_t i = 2; i < FUs.size(); i++) {
		if (FUs[i] && FUs[i]->GetFuType() == 3) {
			addUserFuMnemoRow("Lex", 3, (long)i * FUMkRange);
			break;
		}
	}
}

FU* BusFU::TypeCopy() // ������� �� ������ �� ���� (�� ������� ��������
{
	return new BusFU(Bus, nullptr);
}