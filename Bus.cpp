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
					// A re-declared Bus (FUType 0) is skipped from the emitted .ind and maps to
					// the implicit canonical Bus slot (1); it must NOT consume a user-FU slot.
					// All other FUs take slots 2,3,4,... matching the loader placing the .ind's
					// FU declarations after [0]=self and [1]=Bus.
					if (fuType == 0) {
						UserFuRanges.push_back({ range, FUMkRange });
					} else {
						UserFuRanges.push_back({ range, NextReloadFuIdx * FUMkRange });
						NextReloadFuIdx++;
					}
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

		// mk 253 (IpBufWrite serializer) removed from the Bus -- emission now lives in
		// the IndexFile FU, driven from Millicom.cpp.
		case 287: // CapsListRegister -- thin hook: capture the active CapsList FU pointer
			if (capsListFu == nullptr && Sender != nullptr) capsListFu = Sender;
			break;
		default:
			CommonMk(MK, Load);
			break;
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