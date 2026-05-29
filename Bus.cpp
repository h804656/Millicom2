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

		case 253: // IpBufWrite -- serialize CapsEntries to a .ind file
			if (Load.Point != nullptr) {
				string path = Load.toStr();
				ofstream out(path);
				if (out) {
					int M = (int)CapsEntries.size();
					// Pre-pass: mark entries inside NewFuParent sub-caps as skipped
					std::vector<bool> skip(M, false);
					for (int i = 0; i < M; i++) {
						if (!CapsEntries[i].isMarker && CapsEntries[i].isNewFuParent
							&& i + 1 < M && CapsEntries[i + 1].isMarker
							&& CapsEntries[i + 1].atr == SubCapOpenAtr) {
							int depth = 0;
							for (int j = i + 1; j < M; j++) {
								skip[j] = true;
								if (CapsEntries[j].isMarker && CapsEntries[j].atr == SubCapOpenAtr) depth++;
								else if (CapsEntries[j].isMarker && CapsEntries[j].atr == SubCapCloseAtr) {
									if (--depth == 0) break;
								}
							}
						}
					}
					std::vector<int> capId(M, 0), outIdx(M, -1);
					std::vector<int>  parentSrc;
					std::vector<int>  stack = { 0 };
					int nextCapId = 0;
					for (int i = 0; i < M; i++) {
						if (skip[i]) continue;
						if (CapsEntries[i].isMarker) {
							if (CapsEntries[i].atr == SubCapOpenAtr) {
								nextCapId++;
								int parentCap = stack.back();
								int p = i - 1;
								while (p >= 0 && (skip[p] || CapsEntries[p].isMarker || capId[p] != parentCap)) p--;
								parentSrc.push_back(p);
								stack.push_back(nextCapId);
							} else if (CapsEntries[i].atr == SubCapCloseAtr) {
								if (stack.size() > 1) stack.pop_back();
							}
						} else {
							capId[i] = stack.back();
						}
					}
					int nCaps = nextCapId + 1;
					std::vector<std::vector<int>> groups(nCaps);
					for (int i = 0; i < M; i++)
						if (!CapsEntries[i].isMarker && !skip[i]) groups[capId[i]].push_back(i);
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
							if (CapsEntries[src].isNewFuParent) {
								out << 1001 << " I:" << CapsEntries[src].newFuType;
							} else {
								long int outAtr = rebaseAtr(CapsEntries[src].atr);
								auto& ld = CapsEntries[src].load;
								unsigned int dt = ld.Type >> 1;
								out << outAtr << " ";
								if (dt == Dstring || dt == Dchar) {
									out << "S:" << ld.toStr() << "\"";
								} else if (dt == Dbool) {
									out << "B:" << (ld.toBool() ? "T" : "F");
								} else if (dt == Dfloat || dt == Ddouble) {
									out << "D:" << ld.toDouble();
								} else {
									out << "I:" << ld.toInt();
								}
							}
							out << " " << f1 << " " << f2
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
			if (CapsPendingAtr == 0) break;
			if (CapsPendingAtr == SubCapCloseAtr) {
				if (SubCapDepth == 1) {
					// Top-level sub-cap close: build IC and live-dispatch
					size_t openIdx = (size_t)-1;
					{
						int depth = 0;
						for (size_t k = CapsEntries.size(); k-- > 0; ) {
							if (CapsEntries[k].isMarker && CapsEntries[k].atr == SubCapCloseAtr)
								depth++;
							else if (CapsEntries[k].isMarker && CapsEntries[k].atr == SubCapOpenAtr) {
								if (depth == 0) { openIdx = k; break; }
								depth--;
							}
						}
					}

					long int parentAtr = 0;
					size_t parentCapsIdx = (size_t)-1;
					if (openIdx != (size_t)-1 && openIdx > 0) {
						parentAtr = CapsEntries[openIdx - 1].atr;
						parentCapsIdx = openIdx - 1;
					}

					vector<ip>* innerIc = (openIdx != (size_t)-1)
						? buildIcFromEntries(openIdx + 1, CapsEntries.size())
						: new vector<ip>();

					bool didMakeFU = false;
					if (PendingMakeFU) {
						long int fuType = PendingMakeFUType;
						LoadPoint typeLp = { Cint, &fuType };
						ProgFU(1, typeLp, Sender);
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
						UserFuRanges.push_back({ realRange, NextReloadFuIdx * FUMkRange });
						NextReloadFuIdx++;
						PendingMakeFU     = false;
						PendingMakeFUType = 0;
						didMakeFU         = true;
					}
					LoadPoint icLoad = { TIC, innerIc };
					if (parentAtr > 0 && !didMakeFU) {
						ProgFU(parentAtr, icLoad, Sender);
					}
					else {
						delete innerIc;
					}
					if (didMakeFU && parentCapsIdx != (size_t)-1) {
						long int fuType = 0;
						for (size_t k = CapsEntries.size(); k-- > parentCapsIdx + 1; ) {
							if (!CapsEntries[k].isMarker && CapsEntries[k].atr == -22) {
								fuType = CapsEntries[k].load.toInt();
								break;
							}
						}
						CapsEntries[parentCapsIdx].isNewFuParent = true;
						CapsEntries[parentCapsIdx].newFuType = fuType;
					}
				}

				if (SubCapDepth > 0) SubCapDepth--;
			}
			CapsEntries.push_back({ CapsPendingAtr, Load.Clone(), CapsPendingAtr == SubCapOpenAtr || CapsPendingAtr == SubCapCloseAtr });
			if (CapsPendingAtr == SubCapOpenAtr) {
				SubCapDepth++;
			}
			break;
		}
		case 258: // CapsStashLoad -- save Load (with type) for the next stashed emit
			CapsStashLoad = Load.Clone();
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
				if (SubCapDepth == 1) {
					PendingMakeFU     = true;
					PendingMakeFUType = CapsStashLoad.toInt();
				}
				IpBufPendingMakeFU = false;
			}
			if (CapsPendingAtr == 0) break;
			CapsEntries.push_back({ CapsPendingAtr, CapsStashLoad.Clone(), false });
			if (SubCapDepth > 0) {
				// Inside a sub-cap body — don't live-dispatch
			}
			else {
				ProgFU(CapsPendingAtr, CapsStashLoad, Sender);
			}
			break;
		}
		case 281: // IpBufPendingMakeFU -- next IpBufEmitStashed will also emit a MakeFU IP
			IpBufPendingMakeFU = true;
			break;
		case 286: // PendingFuNameModeSet -- next stashed string Load becomes the user-FU name
			PendingFuNameMode = true;
			if (MnemoTableFu == nullptr && Sender != nullptr) {
				MnemoTableFu = Sender;
			}
			break;
		default:
			CommonMk(MK, Load);
			break;
		}
}

vector<ip>* BusFU::buildIcFromEntries(size_t from, size_t to)
{
	vector<ip>* ic = new vector<ip>();
	size_t i = from;
	while (i < to) {
		if (CapsEntries[i].isMarker) {
			if (CapsEntries[i].atr == SubCapOpenAtr) {
				int depth = 0;
				size_t closeIdx = to;
				for (size_t j = i + 1; j < to; j++) {
					if (CapsEntries[j].isMarker && CapsEntries[j].atr == SubCapOpenAtr) depth++;
					else if (CapsEntries[j].isMarker && CapsEntries[j].atr == SubCapCloseAtr) {
						if (depth == 0) { closeIdx = j; break; }
						depth--;
					}
				}
				vector<ip>* subIc = buildIcFromEntries(i + 1, closeIdx);
				if (!ic->empty()) {
					ic->back().Load = { TIC, subIc };
				} else {
					delete subIc;
				}
				i = closeIdx + 1;
			} else {
				i++;
			}
		} else {
			ic->push_back({ CapsEntries[i].atr, CapsEntries[i].load.Clone() });
			i++;
		}
	}
	return ic;
}

void BusFU::addUserFuMnemoRow(const std::string& name, int type, long range)
{
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