// �� ����
#pragma once
#include "FUini.h"
#include <vector>
#include <unordered_map>
#include <string>

//const int NFUTypes = 27;

typedef FU*(*FUiniArr[NFUTypes])(FU*, FU*);

class BusFU : public FU {
public:
	BusFU(FU* BusContext, FU* Templ) : FU(BusContext) { Bus = BusContext; FUTypesIni(); FUtype = 0; };
	BusFU() : FU() { Bus = this; FUTypesIni(); };
	void ProgFU(long int MK, LoadPoint Load, FU* Sender = nullptr) override;
	FU* Copy() override; // ��������� ����������� ��
	FU* TypeCopy() override; // ������� �� ������ �� ���� (�� ������� ��������
	FuFabric FUTypes;
//	FU* (*FUTypes[NFUTypes])(FU*, FU*) = {BusIni, ConsIni, StrGenIni, LexIni, FindIni,
//		ListIni, GraphTrasfIni, IntAluIni, InOutIni, AutomatManagerIni, NeuroIni,
//		NetManagerIni,SchedulerIni,EventserIni, MeanShiftIni, StreamFloatALUIni, GaussIni, ALUIni,
//		CellularAutomatIni, CellularAutomatManagerIni, RouterIni, GatewayIni, StreamFloatALUManagerIni, 
//		StreamIntALUIni, StreamIntALUManagerIni, MatPlotIni };
	vector <FU *> FUs; // ������ ���������� �� ��������� ��
	FU * FUTempl = nullptr; // ��������� �� �������� ������� ��
	long int FUMkRange = 1000; // �������� �� ��� ������� ��
	long int FUTypeCorrect= fuTypeCorrect; // ��������� ������ ���� �� (��� ������������ �� ������ ��-������)
	bool InterpretatorMode = false; // ����� �������������� (���� �����������, �� ����������� �� InterpretatorExec
	long int Ind = 0, Ind2 = 0; // ������� ��, ����������� �����
	int argc = 0; // ���������� ���������� ��������� ������
	int argInd = -1; // ������ ���������
	vector<string> argv; // ��������� ��������� ������

	// FU-range rebase: populated during FU creation (case 1 MakeFU); read by the
	// IndexFile serializer to remap self-host dual-instance dispatch MKs back to
	// canonical FU indices. Stays on Bus because it is a creation-time concern.
	vector<std::pair<long, long>> UserFuRanges;
	long NextReloadFuIdx = 2;
	long rebaseAtr(long atr) const {
		if (atr <= 0) return atr;
		for (auto& r : UserFuRanges) {
			if (atr >= r.first && atr < r.first + FUMkRange)
				return r.second + (atr - r.first);
		}
		return atr;
	}
	bool capsMakeFuJustFired = false;
	bool ConsumeCapsMakeFu() override { bool r = capsMakeFuJustFired; capsMakeFuJustFired = false; return r; }
	// .ind emission logic lives entirely in the IndexFile FU (off the Bus), driven from
	// Millicom.cpp. The Bus keeps only: (1) rebaseAtr/UserFuRanges (FU-creation-time
	// data IndexFile reads), and (2) a 1-line CapsList pointer-capture hook (mk 287) --
	// it must stay here because the .ind is Lexica-built (`Bus.CapsListRegister` baked
	// in; Lexica has no IndexFile FU) and FUName is not restored on load, so the active
	// CapsList can only be identified by the runtime Sender of that dispatch.
	FU* capsListFu = nullptr;
private:
	void FUTypesIni();
};