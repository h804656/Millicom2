// Конфертер из JSON в OA и обратно

#include "JSON-OAConeverter.h"
#include "Bus.h"
#include "List.h"
#include <sstream>
#include <vector>

void JSON_OAConeverter::Recurs(LoadPoint Load, ofstream& F, string Tab)
{
	F << "\n" << Tab << "[" << Tab;
	bool FComma = false; // Флаг занятой
	for (auto i : *Load.IC())
	{
		if (!FComma)
			FComma = true;
		else
			F << ",";
		F << "\n" << Tab;
		F << Tab << "{";
		//ip* IPTable = AtrSearchIP(MnemoCaps, i.atr);
		ip* IPCaps = AtrSearchIP(MnemoCaps, i.atr);
		ip* IPTable = nullptr;
		if(IPCaps ==nullptr && MnemoTable==nullptr)
			F << "\"" << i.atr << "\":";
		else
		{
			ip* IPCaps = AtrSearchIP(MnemoCaps, i.atr);
			if (IPCaps != nullptr)
				F << "\"" << IPCaps->Load.toStr() << "\":";
		}

		if (i.Load.Point == nullptr)
			F << "nil ";
		else if (!i.Load.isScalar())
			Recurs(i.Load, F, Tab + "    ");
		else
			if (i.Load.isStr())
				F << "\"" << i.Load.toStr() << "\"";
			else
				F << i.Load.toStr();
		F << "}";
	}
	F << "\n" << Tab << "]\n";
}
/*
void JSON_OAConeverter::Recurs(LoadPoint Load, ofstream &F, string Tab)
{
	F << "\n" << Tab << "[\n" << Tab;
	bool FComma = false; // Флаг занятой
	for (auto i : *Load.IC())
	{
		if (!FComma)
			FComma = true;
		else {
			F << ",\n" << Tab;
			F << "{" << " \"";
			ip* IP = AtrSearchIP(MnemoCaps, i.atr);
			if (MnemoCaps == nullptr || IP==nullptr)
				F << i.atr;
			else
			{
					F << "\"" << IP->Load.toStr() << "\"";
			}				
			F << "\":";
		}
		if (i.Load.Point == nullptr)
			F << "nil ";
		else if (i.Load.isScalar())
		{
			if (i.Load.isStr())
				F << "\"";
			F << i.Load.toStr();
			if (i.Load.isStr())
				F << "\"";
			F << "}";
		}
		else
		{
			Recurs(i.Load, F, Tab + "    ");
			F << "}";
		}
	}
	F <<"\n" << Tab << "]\n";
}
*/

void JSON_OAConeverter::ProgFU(long int MK, LoadPoint Load, FU* Sender)
{
	MK %= FUMkRange;
	switch (MK)
	{
	case 1: //MnemoCapsSet Установить указатель на ИС с мнемониками
		if (Load.isIC())
			MnemoCaps = (IC_type)Load.Point;
		break;
	case 2: //MnemoTableSet Установить указатель на таблицу переменных
		if (Load.isIC())
			MnemoTable = (IC_type)Load.Point;
		break;
	case 3: //GraphSet Установить указатель на ОА-граф для конвертации
		if (Load.isIC())
			Graph = Load;
		break;
	case 4: //FOutNameSet Установить файл для вывода json-текста
		FOutName = Load.toStr();
		break;
	case 5: //FInNameSet Установить файл для ввода json-файла
		FInName = Load.toStr();
		break;
	case 20: //FOpenErrProgSet Установить указатель на программу обработки ошибки открытия файла
		FOpenErrProg = (IC_type) Load.Point;
		break;
	case 10: // ToJson Конвертировать в JSON на входе имя файла
	{
		ofstream FOut; // Файл для вывода резульатата конверсии
		if (Load.isEmpty())
			FOut.open(FOutName);
		else
			FOut.open(Load.toStr());
		
		if (!FOut.is_open())
		{
			ProgExec(FOpenErrProg);
			break;
		}
		FOut << "{\n";
		Recurs(Graph, FOut, "");
		FOut.close();
		FOut << "\n}";
		break;
	}
	case 11: // ToOA Конвертировать в ОА из JSON
	{
		ifstream FIn; // Файл для вывода резульатата конверсии
		if (Load.isEmpty())
			FIn.open(FInName);
		else
			FIn.open(Load.toStr());
		FIn.close();
		break;
	}
	default:
		CommonMk(MK, Load, Sender);
		break;
	}
}

void JSON_OAConeverter::ToJson(LoadPoint Graph, string FileName) // Конфертировать из ОА в JSON
{
	ofstream FOut(FileName);
	Recurs(Graph, FOut, "");
	FOut.close();
}

LoadPoint JSON_OAConeverter::FromJSON(string FileName) // Конфертировать из JSPN в ОА
{
	ifstream FIn(FileName);
	FIn.close();
	return {0, nullptr};
}

// === .ind serializer ===
void JSON_OAConeverter::flattenCapsLevel(void* levelIC, vector<CapsEntry>& out)
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

void JSON_OAConeverter::rebuildCapsFromList(List* cl)
{
	bool any = false;
	for (auto grp : cl->ListHead)
		if (grp != nullptr && !grp->empty()) { any = true; break; }
	if (!any) return;
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

void JSON_OAConeverter::buildIndexVector(void* busPtr)
{
	builtRows.clear();
	if (CapsListFu != nullptr) rebuildCapsFromList((List*)CapsListFu);
	BusFU* bus = (BusFU*)busPtr;
	int M = (int)CapsEntries.size();
	std::vector<bool> skip(M, false);
	for (int i = 0; i < M; i++) {
		if (!CapsEntries[i].isMarker && CapsEntries[i].isNewFuParent
			&& i + 1 < M && CapsEntries[i + 1].isMarker
			&& CapsEntries[i + 1].atr == SubCapOpenAtr) {
			if (CapsEntries[i].newFuType == 0) skip[i] = true;
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
		if (!groups[c].empty()) capStart[c] = outIdx[groups[c][0]];
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
			int f1 = (gi << 2) + 2;
			int f2 = 11;
			for (size_t k = 1; k < parentSrc.size() + 1 && k < (size_t)nCaps; k++) {
				if (parentSrc[k - 1] == src && !groups[k].empty()) {
					f1 = (capStart[k] << 2) + 0;
					f2 = 1;
					break;
				}
			}
			if (CapsEntries[src].isIcPtr && CapsEntries[src].sharedSrcIdx >= 0) {
				int fk = childCapOf[CapsEntries[src].sharedSrcIdx];
				if (fk >= 1 && fk < nCaps && !groups[fk].empty()) { f1 = (capStart[fk] << 2) + 0; f2 = 1; }
			}
			int f3 = (j == n - 1) ? -1 : ((outIdx[g[j + 1]]) << 2);
			int f4 = (j == 0) ? -1 : ((outIdx[g[j - 1]]) << 2);
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
				if (dt == Dchar) { row << "C:" << ld.toStr() << "\""; }
				else if (dt == Dstring) { row << "S:" << ld.toStr() << "\""; }
				else if (dt == Dbool) { row << "B:" << (ld.toBool() ? "T" : "F"); }
				else if (dt == Dfloat || dt == Ddouble) { row << "D:" << ld.toDouble(); }
				else { row << "I:" << bus->rebaseAtr(ld.toInt()); }
			}
			row << " " << f1 << " " << f2 << " " << f3 << " " << f4;
			rowBySlot[gi] = row.str();
		}
	}
	for (auto& r : rowBySlot) builtRows.push_back(r);
}

void JSON_OAConeverter::IndexVectWrite(const string& path)
{
	ofstream out(path);
	if (!out) return;
	out << builtRows.size() << "\n";
	for (auto& r : builtRows) out << r << "\n";
}
