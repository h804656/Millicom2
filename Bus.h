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

	/// <summary>
	/// //////////////////////////////////////////////////////////// d2-2026
	/// </summary>
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
	FU* capsListFu = nullptr;
private:
	void FUTypesIni();
};