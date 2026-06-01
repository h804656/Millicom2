// IndexFile FU -- the .ind serializer, extracted from BusFU so emission is
// not on the main Bus FU (mirrors the Delphi GatewayFile FU). Holds the
// capsule buffer (CapsEntries), rebuilds it from the OAP CapsList, and writes
// the .ind file. The FU-range rebase (rebaseAtr / UserFuRanges) stays on BusFU
// because it is populated during FU creation; IndexVectWrite calls back into
// the Bus for it.
#pragma once
#include "Consts.h"
#include <vector>
#include <string>
#include <map>

class List;

class IndexFile : public FU {
public:
	struct CapsEntry {
		long int atr;
		LoadPoint load;
		bool isMarker;
		bool isNewFuParent = false;
		long int newFuType = 0;
		std::string newFuName; // Mnemo of a NewFU declaration (for emit-time identification)
		// Shared-IC reference: a 2nd+ occurrence of an aliased IC pointer. Mirrors Delphi's
		// implicit sharing (IndexVectFromList, MainUnit.pas:694) -- the IC is emitted ONCE
		// (first occurrence, as an Object sub-IC); repeats become IcPointer(-15) caps whose
		// f1 points to the first occurrence's sub-IC head slot, instead of duplicating it.
		bool isIcPtr = false;
		int  sharedSrcIdx = -1; // CapsEntries index of the first occurrence's parent line
	};
	std::vector<CapsEntry> CapsEntries;
	std::map<void*,int> _firstSeenIC; // pointer -> CapsEntries index of first emission (per rebuild)
	static constexpr long int SubCapOpenAtr  = -1000;
	static constexpr long int SubCapCloseAtr = -1001;
	static constexpr long int IcPointerAtr   = -15; // shared-IC reference atr (Delphi IcPointer)
	FU* CapsListFu = nullptr;
	std::string FileName; // output .ind path (set via mk 25 FileNameSet, like Delphi GatewayFile)
	std::vector<std::string> builtRows; // index vector built by buildIndexVector(), one .ind line each

	void rebuildCapsFromList(List* cl);
	void flattenCapsLevel(void* levelIC, std::vector<CapsEntry>& out);
	// Delphi-faithful split: buildIndexVector() == CapsManager's IndexVectFromList (mk 140) --
	// walk the caps tree, assign indices, compute f1/f3/f4, format each row (IcToStr). It produces
	// `builtRows` (no file I/O). IndexVectWrite() == GatewayFile's mk 21 -- pure byte writer that
	// dumps the pre-built rows to FileName. The two are driven by separate injected bus-calls.
	void buildIndexVector();                       // build the index vector into `builtRows`
	void IndexVectWrite(const std::string& path);  // write `builtRows` to a .ind file

	void ProgFU(long int MK, LoadPoint Load, FU* Sender = nullptr) override;
	FU* Copy() override { return new IndexFile(Bus, this); }
	FU* TypeCopy() override { return new IndexFile(Bus, nullptr); }
	IndexFile(FU* BusContext, FU* Templ) { Bus = BusContext; FUtype = 26; }
};
