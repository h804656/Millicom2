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
			{
				long int fuType, mkBeg = 0; bool fromCaps = false;
				std::string name;
				if (Load.isIC() && Load.Point != nullptr) {
					fromCaps = true; fuType = -1;
					for (auto& e : *(IC_type)Load.Point) {
						if (e.atr == -22) fuType = e.Load.toInt();
						else if (e.atr == -20) mkBeg = e.Load.toInt();
						else if (e.atr == -2) {
							unsigned int dt = e.Load.Type >> 1;
							if (dt == Dstring || dt == Dchar) name = e.Load.toStr();
						}
					}
					if (fuType < 0) break;
				} else fuType = Load.toInt();
				FUs.push_back(FUTypes.MakeFu(fuType, this, FUTempl));
				long int range = mkBeg > 0 ? mkBeg : FUMkRange * (FUs.size() - 1);
				FUs.back()->FUMkGlobalAdr = range;
				if (fromCaps) {
					UserFuRanges.push_back({ range, NextReloadFuIdx * FUMkRange });
					NextReloadFuIdx++;
					capsMakeFuJustFired = true; // so the OAP MarkLastCopyOutMk skips dispatching this NewFU sub-cap
				}
			} // ���������� ������ ����������� ��������� ��
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
			if (CapsListFu != nullptr) rebuildCapsFromList((List*)CapsListFu);
			if (Load.Point != nullptr) {
				string path = Load.toStr();
				ofstream out(path);
				if (out) {
					int M = (int)CapsEntries.size();
					// Pre-pass: mark entries inside NewFuParent sub-caps as skipped.
					// Also drop the implicit-Bus declaration: Lexica's CapsManager never
					// serializes the root Bus (its FUType is 0 = FUBusNew). Skipping the
					// PARENT entry too (not just its sub-cap) removes the whole declaration;
					// the group-based f3/f4 relink naturally bridges the gap.
					std::vector<bool> skip(M, false);
					for (int i = 0; i < M; i++) {
						if (!CapsEntries[i].isMarker && CapsEntries[i].isNewFuParent
							&& i + 1 < M && CapsEntries[i + 1].isMarker
							&& CapsEntries[i + 1].atr == SubCapOpenAtr) {
							if (CapsEntries[i].newFuType == 0) skip[i] = true; // implicit Bus root
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
								// (int branch handled below; rebased there)
								} else {
									// Rebase int loads too: dispatch MKs captured during
									// self-host point at the input's dual-instance FU range
									// (e.g. CreateNewFU.FindOr=73228 at input idx 73); rebaseAtr
									// remaps them to the canonical reload index (->16228) so the
									// emitted .ind's wiring targets FUs that actually exist.
									out << "I:" << rebaseAtr(ld.toInt());
								}
							}
							out << " " << f1 << " " << f2
							    << " " << f3 << " " << f4 << "\n";
						}
					}
				}
			}
			break;
		case 287: // CapsListRegister -- capture the OAP CapsList FU pointer
			if (CapsListFu == nullptr && Sender != nullptr) {
				CapsListFu = Sender;
			}
			break;
		default:
			CommonMk(MK, Load);
			break;
		}
}


void BusFU::flattenCapsLevel(void* levelIC, vector<CapsEntry>& out)
{
	IC_type level = (IC_type)levelIC;
	if (level == nullptr) return;
	for (auto& e : *level) {
		if (e.atr == SubCapOpenAtr || e.atr == SubCapCloseAtr) {
			// Literal flat marker (a path not migrated to nested Push/LevelPrevAdd).
			CapsEntry ce; ce.atr = e.atr; ce.load = { 0, nullptr };
			ce.isMarker = true; ce.isNewFuParent = false; ce.newFuType = 0;
			out.push_back(ce);
		}
		else if (e.Load.Point != nullptr && e.Load.isIC()) {
			// Nested sub-capsule: parent line (load cleared) + open marker + body + close.
			CapsEntry pe; pe.atr = e.atr; pe.load = { 0, nullptr };
			pe.isMarker = false; pe.isNewFuParent = false; pe.newFuType = 0;
			out.push_back(pe);
			CapsEntry op; op.atr = SubCapOpenAtr; op.load = { 0, nullptr };
			op.isMarker = true; op.isNewFuParent = false; op.newFuType = 0;
			out.push_back(op);
			flattenCapsLevel(e.Load.Point, out);
			CapsEntry cm; cm.atr = SubCapCloseAtr; cm.load = { 0, nullptr };
			cm.isMarker = true; cm.isNewFuParent = false; cm.newFuType = 0;
			out.push_back(cm);
		}
		else {
			CapsEntry ce; ce.atr = e.atr; ce.load = e.Load.Clone();
			ce.isMarker = false; ce.isNewFuParent = false; ce.newFuType = 0;
			out.push_back(ce);
		}
	}
}

void BusFU::rebuildCapsFromList(List* cl)
{
	bool any = false;
	for (auto grp : cl->ListHead)
		if (grp != nullptr && !grp->empty()) { any = true; break; }
	if (!any) return; // not populated yet -> keep native CapsEntries
	CapsEntries.clear();
	for (auto grp : cl->ListHead)
		flattenCapsLevel((void*)grp, CapsEntries);
	// MakeFU marking (replicates case 257 for the CapsList path): a NewFU body
	// emits its FUType field as an atr==-22 entry inside a -1000/-1001 sub-cap.
	// Mark the parent (non-marker entry just before the -1000) isNewFuParent with
	// newFuType = that value, so the serializer emits `1001 I:<fuType>` and the
	// skip-prepass drops the sub-cap body.
	int M = (int)CapsEntries.size();
	for (int i = 0; i < M; i++) {
		if (!(CapsEntries[i].isMarker && CapsEntries[i].atr == SubCapOpenAtr)) continue;
		long fuType = 0; bool found = false; int depth = 0; std::string fuName;
		for (int j = i + 1; j < M; j++) {
			if (CapsEntries[j].isMarker && CapsEntries[j].atr == SubCapOpenAtr) depth++;
			else if (CapsEntries[j].isMarker && CapsEntries[j].atr == SubCapCloseAtr) {
				if (depth == 0) break;
				depth--;
			}
			else if (depth == 0 && !CapsEntries[j].isMarker && CapsEntries[j].atr == -22) {
				fuType = CapsEntries[j].load.toInt(); found = true;
			}
			else if (depth == 0 && !CapsEntries[j].isMarker && CapsEntries[j].atr == -2) {
				unsigned int dt = CapsEntries[j].load.Type >> 1;
				if (dt == Dstring || dt == Dchar) fuName = CapsEntries[j].load.toStr();
			}
		}
		// A real NewFU declaration is `NewFU={Mnemo="X" FUType=Y}` -- it has BOTH a
		// FUType (-22) AND an FU-name Mnemo (-2) at depth 0 of the sub-cap. A RegPeer
		// match-key row `>{FUType=1 ... FindAnd={Mnemo="Cons"}}` has the FUType but its
		// Mnemo is nested inside FindAnd (depth>0), so fuName stays empty -- requiring a
		// depth-0 Mnemo correctly excludes those false positives (they stay regular caps).
		if (found && !fuName.empty()) {
			int p = i - 1;
			while (p >= 0 && CapsEntries[p].isMarker) p--;
			if (p >= 0) { CapsEntries[p].isNewFuParent = true; CapsEntries[p].newFuType = fuType; CapsEntries[p].newFuName = fuName; }
		}
	}
}

FU* BusFU::Copy() // ��������� ����������� ��
{
	return new BusFU(Bus, this);
}

FU* BusFU::TypeCopy() // ������� �� ������ �� ���� (�� ������� ��������
{
	return new BusFU(Bus, nullptr);
}