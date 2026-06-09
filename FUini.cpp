// ������������ ������������� ��
#include "stdafx.h"
#include "FUini.h"
#include "Consts.h"
#include "Bus.h"
#include "Console.h"
#include "StrGen.h"
#include "Lex.h"
#include "Find.h"
#include "List.h"
#include "GraphTransf.h"
#include "ALU.h"
#include "InOut.h"
#include "Neuro.h"
#include "NetModeling.h"
#include "SchedulerEventser.h"
#include "MeanShift.h"
#include "Gauss.h"
#include "Gauss.h"
#include "CellularAutomata.h"
#include "IntAlu.h"
#include "Router.h"
#include "Gateway.h"
#include "StreamFloatALU.h"
#include "StreamManager.h"
#include "StreamIntALU.h"
#include "MatPlot.h"
#include "GraphDisplay.h"

using namespace std;


int SizeOfFUType(unsigned int Type) // ����� ������, ���������� �� ������������� ����
{
	switch (Type)
	{
	case 1: return sizeof(BusFU);
	case 2: return sizeof(Console);
	case 3: return sizeof(StrGen);
	case 4: return sizeof(Lex);
	case 5: return sizeof(Find);
	case 6: return sizeof(List);
	case 7: return sizeof(GraphTransf);
	case 8: return sizeof(IntAlu);
	case 9: return sizeof(InOut);
	case 10: return sizeof(AutomatManager);
	case 11: return sizeof(Neuro);
	case 12: return sizeof(NetManager);
	case 13: return sizeof(Scheduler);
	case 14: return sizeof(Eventser);
	case 15: return sizeof(MeanShift);
	case 16: return sizeof(StreamFloatALU);
	case 17: return sizeof(Gauss);
	case 18: return sizeof(ALU);
	case 19: return sizeof(CellularAutomata);
	case 20: return sizeof(CellularAutomatManager);
	case 21: return sizeof(Router);
	case 22: return sizeof(Gateway);
	case 23: return sizeof(StreamFloatALU);
	case 24: return sizeof(StreamManager);
	case 25: return sizeof(StreamIntALU);
	case 26: return sizeof(MatPlot);
	case 27: return sizeof(GraphDisplay);
	}
}


FU *ConsIni(FU *Bus, FU *TEmpl)
{
 	return (FU*)new Console(Bus, TEmpl);
}

FU *BusIni(FU *BusContext, FU *TEmpl)
{
	return (FU*)new BusFU(BusContext, TEmpl);
}

FU *LexIni(FU *BusContext, FU *TEmpl)
{
	return (FU*)new Lex(BusContext, TEmpl);
}
FU *StrGenIni(FU *BusContext, FU *TEmpl)
{
	return (FU*)new StrGen(BusContext, TEmpl);
}

FU *FindIni(FU *BusContext, FU *TEmpl)
{
	return (FU*)new Find(BusContext, TEmpl);
}

FU *ListIni(FU *BusContext, FU *TEmpl)
{
	return (FU*)new List(BusContext, TEmpl);
}

FU* GraphTrasfIni(FU* BusContext, FU* TEmpl)
{
	return (FU*) new GraphTransf(BusContext, TEmpl);
}

FU* ALUIni(FU* BusContext, FU* TEmpl)
{
	return (FU*) new ALU(BusContext, TEmpl);
}

FU* InOutIni(FU* BusContext, FU* TEmpl)
{
	return (FU*) new InOut(BusContext, TEmpl);
}

FU* AutomatManagerIni(FU* BusContext, FU* TEmpl)
{
	return (FU*) new AutomatManager(BusContext, TEmpl);
}

FU* NeuroIni(FU* BusContext, FU* TEmpl)
{
	return (FU*) new Neuro(BusContext, TEmpl);
}

FU* NetManagerIni(FU* BusContext, FU* TEmpl)
{
	return (FU*) new NetManager(BusContext, TEmpl);
//	return nullptr;
}

FU* SchedulerIni(FU* BusContext, FU* TEmpl)
{
	return (FU*) new Scheduler(BusContext, TEmpl);
}

FU* EventserIni(FU* BusContext, FU* TEmpl)
{
	return (FU*) new Eventser(BusContext, TEmpl);
}

FU* MeanShiftIni(FU* BusContext, FU* TEmpl)
{
	return (FU*) new MeanShift(BusContext, TEmpl);
}

FU* GaussIni(FU* BusContext, FU* TEmpl)
{
	return (FU*) new Gauss(BusContext, TEmpl);
}

FU* CellularAutomatIni(FU* BusContext, FU* TEmpl)
{
	return (FU*) new CellularAutomata(BusContext, TEmpl);
}

FU* CellularAutomatManagerIni(FU* BusContext, FU* TEmpl)
{
	return (FU*) new CellularAutomatManager(BusContext, TEmpl);
}

FU* IntAluIni(FU* BusContext, FU* TEmpl)
{
	return (FU*) new IntAlu(BusContext, TEmpl);
}

FU* RouterIni(FU* BusContext, FU* TEmpl)
{
	return (FU*) new Router(BusContext, TEmpl);
}

FU* GatewayIni(FU* BusContext, FU* TEmpl)
{
	return (FU*) new Gateway(BusContext, TEmpl);
}

FU* StreamFloatALUIni(FU* BusContext, FU* TEmpl)
{
	return (FU*) new StreamFloatALU(BusContext, TEmpl);
}

FU* StreamFloatALUManagerIni(FU* BusContext, FU* TEmpl)
{
	return (FU*) new StreamManager(BusContext, TEmpl);
}

FU* StreamIntALUIni(FU* BusContext, FU* TEmpl)
{
	return (FU*) new StreamIntALU(BusContext, TEmpl);
}

FU* MatPlotIni(FU* BusContext, FU* TEmpl)
{
	return (FU*) new MatPlot(BusContext, TEmpl);
}

FU* GraphDisplayIni(FU* BusContext, FU* TEmpl)
{
	return (FU*) new GraphDisplay(BusContext, TEmpl);
}

