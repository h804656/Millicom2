// Конфертер из JSON в OA и обратно

#include "JSON-OAConeverter.h"

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