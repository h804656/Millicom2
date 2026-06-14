// Конфертер из JSON в OA и обратно

#pragma once
#include "Consts.h"
#include "Search.h"
#include <string>
#include <fstream>
#include <vector>
#include <map>

using namespace std;
class List;
class JSON_OAConeverter : public FU {
private:
	IC_type MnemoCaps = nullptr;  // Указатель на капсулу с мнемониками
	IC_type MnemoTable = nullptr; // Указатель на таблицу переменных
	LoadPoint Graph ={ 0, nullptr}; // Указатель на ОА-граф
//	ofstream FOut; // Файл для вывода резульатата конверсии
//	ifstream FIn; // Файл для ввода json
	string FOutName, FInName; // Имена выходного и входного файлов
	IC_type FOpenErrProg = nullptr; // Программа для ошибки открытия файла
	void Recurs(LoadPoint Load, ofstream& F, string Tab=""); // Рекурсивная часть перевода в JSON 
	Search Searcher; // Устройство поиска
public:
	void ProgFU(long int MK, LoadPoint Load, FU* Sender = nullptr);
	void FOutNameSet(string Fname) { FOutName = Fname; };
	void FInNameSet(string Fname) { FInName = Fname; };
	void ToJson(LoadPoint Graph, string FileName); // Конфертировать из ОА в JSON
	LoadPoint FromJSON(string FileName); // Конфертировать из JSPN в ОА
	void MnemoCapsSet(LoadPoint LP) 
	{ MnemoCaps = (IC_type) LP.Point; };
	void MnemoTableSet(LoadPoint LP) { MnemoTable = (IC_type)LP.Point; };
	// --- .ind serializer (merged from the former IndexFile FU) ---
	struct CapsEntry {
		long int atr; LoadPoint load; bool isMarker;
		bool isNewFuParent = false; long int newFuType = 0; std::string newFuName;
		bool isIcPtr = false; int sharedSrcIdx = -1;
	};
	std::vector<CapsEntry> CapsEntries;
	std::map<void*,int> _firstSeenIC;
	std::string IndFileName;
	std::vector<std::string> builtRows;
	FU* CapsListFu = nullptr;
	static constexpr long int SubCapOpenAtr = -1000;
	static constexpr long int SubCapCloseAtr = -1001;
	void flattenCapsLevel(void* levelIC, std::vector<CapsEntry>& out);
	void rebuildCapsFromList(class List* cl);
	void buildIndexVector(void* busPtr);
	void IndexVectWrite(const std::string& path);
};