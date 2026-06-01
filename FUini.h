// ������������ ������������� ��
#pragma once
#include "Consts.h"
#include <array>

int SizeOfFUType(unsigned int Type); // ����� ������, ���������� �� ������������� ����

FU *BusIni(FU *BusContext, FU *TEmpl);
FU *ConsIni(FU *BusContext, FU *TEmpl);
FU *LexIni(FU *BusContext, FU *TEmpl);
FU *StrGenIni(FU *BusContext, FU *TEmpl);
FU *LexIni(FU *BusContext, FU *TEmpl);
FU *FindIni(FU *BusContext, FU *TEmpl);
FU* ListIni(FU* BusContext, FU* TEmpl);
FU* GraphTrasfIni(FU* BusContext, FU* TEmpl);
FU* ALUIni(FU* BusContext, FU* TEmpl);
FU* InOutIni(FU* BusContext, FU* TEmpl);
FU* AutomatManagerIni(FU* BusContext, FU* TEmpl);
FU* NeuroIni(FU* BusContext, FU* TEmpl);
FU* NetManagerIni(FU* BusContext, FU* TEmpl);
FU* SchedulerIni(FU* BusContext, FU* TEmpl);
FU* EventserIni(FU* BusContext, FU* TEmpl);
FU* MeanShiftIni(FU* BusContext, FU* TEmpl);
FU* GaussIni(FU* BusContext, FU* TEmpl);
FU* CellularAutomatIni(FU* BusContext, FU* TEmpl);
FU* CellularAutomatManagerIni(FU* BusContext, FU* TEmpl);
FU* IntAluIni(FU* BusContext, FU* TEmpl);
FU* RouterIni(FU* BusContext, FU* TEmpl);
FU* GatewayIni(FU* BusContext, FU* TEmpl);
FU* StreamFloatALUIni(FU* BusContext, FU* TEmpl);
FU* StreamFloatALUManagerIni(FU* BusContext, FU* TEmpl);
FU* StreamIntALUIni(FU* BusContext, FU* TEmpl);
FU* StreamIntALUManagerIni(FU* BusContext, FU* TEmpl);
FU* MatPlotIni(FU* BusContext, FU* TEmpl);
FU* GraphDisplayIni(FU* BusContext, FU* TEmpl);
FU* IndexFileIni(FU* BusContext, FU* TEmpl);

const int NFUTypes = 27;

typedef FU* (*FUiniProg)(FU*, FU*);

class FuFabric // ������� �� �������� ��
{
private:
	 array<FUiniProg, NFUTypes> FUTypes = {BusIni, ConsIni, StrGenIni, LexIni, FindIni,
		ListIni, GraphTrasfIni, IntAluIni, InOutIni, AutomatManagerIni, NeuroIni,
		NetManagerIni,SchedulerIni,EventserIni, MeanShiftIni, StreamFloatALUIni, GaussIni, ALUIni,
		CellularAutomatIni, CellularAutomatManagerIni, RouterIni, GatewayIni, StreamFloatALUManagerIni,
		StreamIntALUIni, MatPlotIni, GraphDisplayIni, IndexFileIni };
	 bool FErr = false; // ���� ������ ������� ��
public:
	int GetFuTypeCount() // ������ ���������� ����� ��
	{
		return FUTypes.size();
	};
	bool GerFErr() // ������ ���� ������ �������� ��
	{ return FErr; };
	FU* MakeFu(int t, void* BusUk = nullptr, void* Templ = nullptr) // ������� �� �� ������� ����
	{
		if (t<0 || t>FUTypes.size())
		{
			FErr = true;
			return nullptr;
		}
		return FUTypes[t]((FU*)BusUk, (FU*)Templ);
	};

	FU* MakeFu(void* Templ) // ������� �� �� �������
	{
		if (((FU*)Templ)->GetFuType()<0 || ((FU*)Templ)->GetFuType() > FUTypes.size())
		{
			FErr = true;
			return nullptr;
		}
		return FUTypes[((FU*)Templ)->FUtype](((FU*)Templ), (FU*)Templ);
	};
};
