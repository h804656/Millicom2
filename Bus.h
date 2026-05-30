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

	struct CapsEntry {
		long int atr;
		LoadPoint load;
		bool isMarker;
		bool isNewFuParent = false;
		long int newFuType = 0;
	};

	vector<CapsEntry> CapsEntries;

	// Sentinel atr codes delimiting sub-capsules in the flat CapsEntries form.
	// The OAP builds CapsList nested (Push/LevelPrevAdd); rebuildCapsFromList ->
	// flattenCapsLevel re-introduces these markers so the serializer is unchanged.
	static constexpr long int SubCapOpenAtr  = -1000;
	static constexpr long int SubCapCloseAtr = -1001;

	// Atr rebase table for serialization: each user NewFU live-dispatches into
	// a high compile-time FU index (e.g. 47), but a fresh reload only has the
	// Bus stub (0) and the Bus (1), so user FUs land at 2, 3, ... in order of
	// MakeFU emission. We record (compileBase, reloadBase) for each user FU
	// when its NewFU body closes, then rewrite emitted atrs from compile-MK
	// space to reload-MK space at serialize time (IpBufWrite). The live atrs
	// used by in-compile dispatch stay in compile-MK space.
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
	// IpBuf->OAP migration: the OAP-side `CapsList` (a FUListNew) mirrors
	// CapsEntries as a flat list of {atr,load} ips. The OAP registers it once
	// by dispatching CapsListRegister (Bus mk 287) from CapsList's context, so
	// the Bus holds a direct pointer -- no FU scan, no sentinel. At serialize
	// time rebuildCapsFromList() rebuilds CapsEntries from its ips so the
	// existing serializer runs unchanged.
	FU* CapsListFu = nullptr;
	// Set true when a native MakeFU fires at a sub-cap close; CapsList.MarkLastCopyOutMk
	// consumes it to skip the OAP sub-cap live-dispatch for NewFU bodies (the native
	// MakeFU already created the FU; a second dispatch would create a spurious one).
	bool capsMakeFuJustFired = false;
	bool ConsumeCapsMakeFu() override { bool r = capsMakeFuJustFired; capsMakeFuJustFired = false; return r; }
	void rebuildCapsFromList(class List* cl);
	// Flatten one nested CapsList level into flat CapsEntries (with -1000/-1001
	// markers around nested sub-capsules). The OAP now builds CapsList nested via
	// Push/LevelPrevAdd (like the canonical ListSintezator), so this is the inverse
	// of the old buildIcFromEntries -- nesting is done in OAP, this only flattens
	// for the existing serializer. Handles literal flat markers too (mixed paths).
	void flattenCapsLevel(void* levelIC, vector<CapsEntry>& out);
private:
	void FUTypesIni();
};