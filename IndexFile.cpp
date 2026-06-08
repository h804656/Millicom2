#include "stdafx.h"
#include "Consts.h"
#include "IndexFile.h"
#include "Bus.h"
#include "List.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

void IndexFile::flattenCapsLevel(void* levelIC, vector<CapsEntry>& out)
{
	IC_type level = (IC_type)levelIC;
	if (level == nullptr) return;
	for (auto& e : *level) {
		if (e.atr == SubCapOpenAtr || e.atr == SubCapCloseAtr) {
			CapsEntry ce; ce.atr = e.atr; ce.load = { 0, nullptr };
			ce.isMarker = true; ce.isNewFuParent = false; ce.newFuType = 0;
			out.push_back(ce);
		}
		else if (e.Load.Point != nullptr && e.Load.isIC()) {
			auto seen = _firstSeenIC.find(e.Load.Point);
			if (seen != _firstSeenIC.end() && e.atr > 0) {
				CapsEntry ip; ip.atr = e.atr; ip.load = { 0, nullptr };
				ip.isMarker = false; ip.isNewFuParent = false; ip.newFuType = 0;
				ip.isIcPtr = true; ip.sharedSrcIdx = seen->second;
				out.push_back(ip);
			}
			else {
				if (seen == _firstSeenIC.end()) _firstSeenIC[e.Load.Point] = (int)out.size();
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
		}
		else {
			CapsEntry ce; ce.atr = e.atr; ce.load = e.Load.Clone();
			ce.isMarker = false; ce.isNewFuParent = false; ce.newFuType = 0;
			out.push_back(ce);
		}
	}
}

void IndexFile::rebuildCapsFromList(List* cl)
{
	bool any = false;
	for (auto grp : cl->ListHead)
		if (grp != nullptr && !grp->empty()) { any = true; break; }
	if (!any) return; // not populated yet -> keep native CapsEntries
	CapsEntries.clear();
	_firstSeenIC.clear();
	for (auto grp : cl->ListHead)
		flattenCapsLevel((void*)grp, CapsEntries);
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
		if (found && !fuName.empty()) {
			int p = i - 1;
			while (p >= 0 && CapsEntries[p].isMarker) p--;
			if (p >= 0) { CapsEntries[p].isNewFuParent = true; CapsEntries[p].newFuType = fuType; CapsEntries[p].newFuName = fuName; }
		}
	}
}

void IndexFile::buildIndexVector()
{
	builtRows.clear();
	if (CapsListFu != nullptr) rebuildCapsFromList((List*)CapsListFu);
	BusFU* bus = (BusFU*)Bus; // for rebaseAtr / UserFuRanges (populated during FU creation)
	int M = (int)CapsEntries.size();
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
			}
			else if (CapsEntries[i].atr == SubCapCloseAtr) {
				if (stack.size() > 1) stack.pop_back();
			}
		}
		else {
			capId[i] = stack.back();
		}
	}
	int nCaps = nextCapId + 1;
	std::vector<std::vector<int>> groups(nCaps);
	for (int i = 0; i < M; i++)
		if (!CapsEntries[i].isMarker && !skip[i]) groups[capId[i]].push_back(i);
	int N = 0;
	for (int i = 0; i < M; i++)
		if (!CapsEntries[i].isMarker && !skip[i]) outIdx[i] = N++;
	std::vector<int> capStart(nCaps, 0);
	for (int c = 0; c < nCaps; c++)
		if (!groups[c].empty()) capStart[c] = outIdx[groups[c][0]]; // sub-IC head slot (owner's f1 target)
	std::vector<int> childCapOf(M, -1);
	for (int k = 1; k <= nextCapId; k++)
		if ((size_t)(k - 1) < parentSrc.size() && parentSrc[k - 1] >= 0 && parentSrc[k - 1] < M)
			childCapOf[parentSrc[k - 1]] = k;
	std::vector<std::string> rowBySlot(N);
	for (int c = 0; c < nCaps; c++) {
		auto& g = groups[c];
		int n = (int)g.size();
		for (int j = 0; j < n; j++) {
			int src = g[j];
			int gi = outIdx[src];
			int f1 = (gi << 2) + 2; // default: load is the entry's own inline value (offset 2)
			int f2 = 11;            // ConstPointType (loader ignores f2; kept for fidelity)
			for (size_t k = 1; k < parentSrc.size() + 1 && k < (size_t)nCaps; k++) {
				if (parentSrc[k - 1] == src && !groups[k].empty()) {
					f1 = (capStart[k] << 2) + 0; // load points to the sub-IC head (offset 0)
					f2 = 1;                      // IcPointType
					break;
				}
			}
			// Shared-IC reference: point f1 at the FIRST occurrence's sub-IC head (offset 0,
			// IcPointType) instead of inlining -- mirrors Delphi's implicit IcPointer sharing.
			if (CapsEntries[src].isIcPtr && CapsEntries[src].sharedSrcIdx >= 0) {
				int fk = childCapOf[CapsEntries[src].sharedSrcIdx];
				if (fk >= 1 && fk < nCaps && !groups[fk].empty()) { f1 = (capStart[fk] << 2) + 0; f2 = 1; }
			}
			int f3 = (j == n - 1) ? -1 : ((outIdx[g[j + 1]]) << 2); // next sibling in this IC
			int f4 = (j == 0) ? -1 : ((outIdx[g[j - 1]]) << 2); // prev sibling in this IC
			if (!CapsEntries[src].isNewFuParent && !CapsEntries[src].isIcPtr && CapsEntries[src].atr < 0 && f1 == ((gi << 2) + 2)) {
				auto& kld = CapsEntries[src].load;
				unsigned int kdt = kld.Type >> 1;
				if (kdt != Dstring && kdt != Dchar && kdt != Dbool && kdt != Dfloat && kdt != Ddouble && kld.toInt() == 0)
					f1 = -1;
			}
			if (!CapsEntries[src].isNewFuParent && !CapsEntries[src].isIcPtr
				&& CapsEntries[src].load.Point == nullptr && f1 == ((gi << 2) + 2))
				f1 = -1;
			std::ostringstream row;
			if (CapsEntries[src].isNewFuParent) {
				row << 1001 << " I:" << CapsEntries[src].newFuType;
			}
			else {
				long int outAtr = bus->rebaseAtr(CapsEntries[src].atr);
				auto& ld = CapsEntries[src].load;
				unsigned int dt = ld.Type >> 1;
				row << outAtr << " ";
				if (dt == Dchar) {
					row << "C:" << ld.toStr() << "\"";
				}
				else if (dt == Dstring) {
					row << "S:" << ld.toStr() << "\"";
				}
				else if (dt == Dbool) {
					row << "B:" << (ld.toBool() ? "T" : "F");
				}
				else if (dt == Dfloat || dt == Ddouble) {
					row << "D:" << ld.toDouble();
				}
				else {
					row << "I:" << bus->rebaseAtr(ld.toInt());
				}
			}
			row << " " << f1 << " " << f2 << " " << f3 << " " << f4;
			rowBySlot[gi] = row.str();
		}
	}
	for (auto& r : rowBySlot) builtRows.push_back(r);
}

void IndexFile::IndexVectWrite(const string& path)
{
	ofstream out(path);
	if (!out) return;
	out << builtRows.size() << "\n";
	for (auto& r : builtRows) out << r << "\n";
}

void IndexFile::ProgFU(long int MK, LoadPoint Load, FU* Sender)
{
	MK %= FUMkRange;
	switch (MK)
	{
	case 21: // IndexVectWrite (GatewayFile role) -- write the pre-built rows to FileName.
		IndexVectWrite(FileName);
		break;
	case 22: // IndexVectFromList (CapsManager role) -- the caps accumulator forwarded itself
		if (Load.isFU()) CapsListFu = (FU*)Load.Point;
		else if (Sender != nullptr) CapsListFu = Sender;
		buildIndexVector();
		break;
	case 25: // FileNameSet -- set the output .ind path (Delphi GatewayFile idiom)
		FileName = Load.toStr();
		break;
	case 287: // CapsListRegister -- capture the OAP CapsList FU pointer. When dispatched
		if (CapsListFu == nullptr) {
			if (Load.isFU()) CapsListFu = (FU*)Load.Point;
			else if (Sender != nullptr) CapsListFu = Sender;
		}
		break;
	default:
		CommonMk(MK, Load);
		break;
	}
}
