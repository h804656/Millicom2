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

	// Compiler output buffer. Each entry is the head text of one IP
	// ("<atr> <T>:<value>") with no trailing pointer fields; IpBufWrite
	// computes those on serialization.
	vector<string> CapsIps;
	// Pending atr value for the next IpBufEmit call (set via IpBufSetAtr).
	long int CapsPendingAtr = 0;
	// Stashed Load for the single-operand emit path -- IntAlu (AleResult)
	// can't carry string/double types so we keep an independent copy.
	LoadPoint CapsStashLoad = { 0, nullptr };
	// Set by IpBufPendingMakeFU when the parser sees `FUType` so the next
	// IpBufEmitStashed call ALSO emits a `1001 I:<value>` MakeFU IP. The
	// flag clears after the emit.
	bool IpBufPendingMakeFU = false;

	// Sentinel atr codes the OAP layer can emit via IpBufSetAtr+IpBufEmit to
	// delimit sub-capsules in the linear CapsIps buffer. IpBufWrite picks
	// them up at serialization time to lay out main + sub-capsules with the
	// correct DInd refs / capsule chain links. The markers are dropped from
	// the emitted .ind.
	static constexpr long int SubCapOpenAtr  = -1000;
	static constexpr long int SubCapCloseAtr = -1001;
	// Sub-capsule nesting depth. IpBufEmit/IpBufEmitStashed/AleEmit only
	// live-dispatch (run the IP immediately so e.g. `Cons.OutLn="x"` prints
	// at parse-time) when depth==0. Inside `>{...}` or `={...}` bodies, the
	// IPs are pure row-body content and must NOT execute until the row is
	// actually matched and dispatched at runtime.
	int SubCapDepth = 0;
	// In-memory IC reconstruction. While CapsIps holds serialized text for
	// the .ind output, we ALSO build a parallel IC (vector<ip>) for each
	// open sub-cap so the parent IP can be live-dispatched at its closing
	// `}` with a real `{TIC, IC}` Load. Without this, e.g. `L.Set=>{...}`
	// would live-dispatch as `L.Set(0)` and leave L empty at parse time --
	// only the .ind output would be correct.
	vector<vector<ip>*> IcStack;
	vector<long int>    ParentAtrStack;
	long int            PendingParentAtr = 0;

	// User-FU registration state. When `NewFU={Mnemo="X" FUType=Y}` is parsed,
	// the Mnemo MnemoTable row sets PendingFuNameMode so the next IpBufStashLoad
	// captures the user FU's name; the FUType row sets IpBufPendingMakeFU so the
	// next IpBufEmitStashed emits a MakeFU IP AND inserts a MnemoTable row for
	// the new FU. After that, `<X>.<MK>` resolves through the standard
	// MnemoTable.FindAnd path -- no fallback lookup MK needed.
	bool       PendingFuNameMode = false;
	std::string PendingFuName;
	void*      LexFuPtr          = nullptr;
	FU*        MnemoTableFu      = nullptr; // captured from Sender on PendingFuNameModeSet
	// Deferred MakeFU state. Set by the FUType= field inside `NewFU={...}`;
	// the actual MakeFU + MnemoTable-row insertion runs at the NewFU close
	// `}` instead of inline during the body emit. This way the body IPs are
	// clean Delphi-style fields (no synthetic `1001 I:type` IP) -- the IC
	// dispatched to CreateNewFU.FindOr at close time contains only the
	// field IPs Delphi would emit.
	bool       PendingMakeFU     = false;
	long int   PendingMakeFUType = 0;
	// Per-open-sub-cap: index in CapsIps of the parent IP entry (one before
	// the SubCapOpen marker). Used at SubCapClose to rewrite the NewFU
	// parent entry into a `1001 I:<fuType>` MakeFU IP when didMakeFU=true,
	// and truncate the sub-cap markers + body fields out of CapsIps so the
	// emitted .ind is replayable on a fresh executor.
	vector<size_t> CapsIpsParentIdxStack;
	// Atr rebase table for serialization: each user NewFU live-dispatches into
	// a high compile-time FU index (e.g. 47), but a fresh reload only has the
	// Bus stub (0) and the Bus (1), so user FUs land at 2, 3, ... in order of
	// MakeFU emission. We record (compileBase, reloadBase) for each user FU
	// when its NewFU body closes, then rewrite emitted atrs from compile-MK
	// space to reload-MK space at CapsIps push time. The live atrs used by
	// in-compile dispatch stay in compile-MK space -- only the serialized
	// strings are rebased.
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
	// Operands accumulate into AleVals; operators into AleOps. When a new
	// operator arrives at precedence <= the top of AleOps, the top is popped
	// and applied to the top two operands. `(` pushes a paren sentinel onto
	// AleOps that blocks popping; `)` pops down to (and through) the sentinel.
	// AleEmit drains AleOps, leaving the final value on AleVals.
	vector<long int> AleVals;
	vector<int>      AleOps;   // 1=Add 2=Sub 3=Mul 4=Div 5=Mod 100=`(` sentinel
	// At MakeFU emit, register the user FU in MnemoTable so a subsequent
	// `<name>.<MK>` lookup goes through normal FindAnd. The row is built by
	// borrowing the MkTable.Set IP from a built-in peer of the same FU type.
	void addUserFuMnemoRow(const std::string& name, int type, long range);
	// Programmatically inject user-visible mnemo rows for the bootstrap FUs
	// (MnemoTable, Stack) so user OAP code can address them by name. Doing
	// this in C++ sidesteps Delphi's "duplicate MkTable.Set=X! across rows"
	// limit: we reuse the existing ListPeer row's MkTable.Set IP at runtime
	// instead of adding a second row that references the same table.
	void InjectBuiltinMnemos();
private:
	void FUTypesIni();
};