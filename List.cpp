// ФУ Список
#include "stdafx.h"
#include "List.h"

using namespace std;
FU* List::sCapsList = nullptr;

void List::ProgFU(long int MK, LoadPoint Load, FU* Sender)
{
	if (PrefixProg != nullptr)
	{
		IC_type t = PrefixProg;
		PrefixProg = nullptr;
		ProgExec(t); // Предварительная программа, вызываемая при приходе любой МК
		PrefixProg = t;
	}
	MK %= FUMkRange;
	if (MkRedirect >= 0) // Переадресация Мк
	{
		long t = MkRedirect;
		MkRedirect = -1;
		MkExec(t + MK, Load, Sender);
		return;
	}
	switch (MK)
	{
	case 0: // Reset
		Searcher.Clear();
		ListHead.clear();// Сделать потом очистку списка
		ListHead.push_back(nullptr);
		break;
	case 1:	// Set
		if (Mode == 0) // Режим работы со списком в формате ИК
		{
			if (ListHead.back() != nullptr && !ListHead.back()->size())
				delete ListHead.back();
			ListHead.back() = (IC_type)Load.Point;
		}
		else if (Mode == 1) // Режим хеширования
		{

		}
		break;
	case 2:// Out Выдать ссылку на список
		Load.Write(ListHead.back());
		break;
	case 3:// OutMk Выдать МК со ссылкой на список
		if (!ListHead.size())
			MkExec(Load, { 0,nullptr });
		else
			MkExec(Load, { TIC,(void*)ListHead.back() });
		break;
	case 4: // RootOutMk Выдать МК с указателем на корневой список
		if (!ListHead.size())
			MkExec(Load, { 0,nullptr });
		else
			MkExec(Load, { TIC,(void*)*ListHead.begin() });
		break;
	case 5:	// MultiLineModeSet
		MultiLineMode = Load.toInt(1);
		break;
	case 6: // MultiListModeSet Установить режим многоуровневого списка для поиска (true по умолчанию)
		MultiListMode = Load.toBool(true);
		break;
	case 7: // MkModeSet Режим выполнения всех МК в ИК-шаблоне (МК-ой считается любой атрибут, индекс которого больше 0) При пустой нагрузке режим устанавливается
		Searcher.MkMode = Load.toBool(true);
		break;
	case 26: // CalcModeSet Установить режим управления поиском с помощью арифметико-логичекого устройства (true по умолчанию)
		Searcher.CalcMode = Load.toBool(true);
		break;
	case 27: // CalcMkSet Установить МК для вычисления АЛВ
		Searcher._CalcMk = Load.toInt();
		break;
	case 8:// EmptyExec Выполнить программу, если список пуст
	case 9:// FullExec Выполнить программу, если список не пуст
	case 14: // OneLineExec Выполнить программу, если в списке только одна строка
	case 13: // MoreOneLineExec Выполнить программу, если в списке больше одной строка
	case 19: // EmptyOrOneLineExec Выполнить программу, если в списке меньше двух строк
		if (MK == 8 && (ListHead.back() == nullptr || ListHead.back()->size() == 0) ||
			MK == 9 && ListHead.size() != 0 && ListHead.back() != nullptr && ListHead.back()->size() > 0 ||
			MK == 14 && ListHead.size() != 0 && ListHead.back() != nullptr && ListHead.back()->size() == 1 ||
			MK == 13 && ListHead.size() != 0 && ListHead.back() != nullptr && ListHead.back()->size() > 1 ||
			MK == 19 && (ListHead.back() == nullptr || ListHead.back()->size() <= 1)
			)
			if (Load.isIC())
				ProgExec(Load);
			else
				ProgExec(Prog);
		else if (Load.Point == nullptr)
			ProgExec(ElseProg);
		break;
	case 10: // SuccessLineProgSet Установить указатель на программу, выполняемую при удачном поиске в линии списка
		Searcher.SuccessProg = (IC_type)Load.Point;
		break;
	case 11: // SuccessAfterLineProgSet Установить указатель на программу, выполняемую при удачном поиска в линии списка после обоработки программ линии
		Searcher.SuccessAfterProg = (IC_type)Load.Point;
		break;
	case 12: // SuссessProgSet Установить указатель на программу, выполняемую в случае удачного поиска во всем  списке
		SuссessProg=Load.Point;
		break;
	case 300: // SuссessProgAdd Добавить в стек указатель на программу, выполняемую в случае удачного поиска во всем  списке
		SuссessProgs.push_back(Load.Point);
		break;
	case 301: // SuссessProgClear
		SuссessProgs.clear();
		break;
	case 302: // FailProgAdd Добавить в стек указатель на программу, выполняемую в случае неудачного поиска во всем списке
		FailProgs.push_back(Load.Point);
		break;
	case 303: // FailProgClear Очистить список указателей на программ, выполняемых в случае неудачного поиска во всем списке
		FailProgs.clear();
		break;
	case 23: // SuссessExec Выполнить программу, если перед этим был удачный поиск
		if (Searcher.Rez)
			ProgExec(Load);
		break;
	case 24: // FailExec Выполнить программу, если перед этим был неудачный поиск
		if (!Searcher.Rez)
			ProgExec(Load);
		break;
	case 15: // FailLineProgSet Установить указатель на программу, выполняемую в случае неудачного поиска в линии списка
		Searcher.FailProg = (IC_type)Load.Point;
		break;
	case 16: // FailAfterLineProgSet Установить указатель на программу, выполняемую в случае неудачного поиска в линии списка после обоработки программ линии
		Searcher.FailAfterProg = (IC_type)Load.Point;
		break;
	case 17: // FailProgSet Установить указатель на программу, выполняемую в случае неудачного поиска во всем  списке
		FailProg=Load.Point;
		break;
	case 18: //  Установить указатель на программу, выполняемую в случае удачного поиска в текущей линии
		SuссessLineProg = Load.Point;
		break;
	case 20: // ProgAtrSet Установить атрибут линии списка по умолчанию
		if (Load.Point != nullptr && Load.isInt())
			Searcher.Prog_atr = Load.toInt();
		break;
	case 21: //LineAtrDef Установить атрибут марки списка по умолчанию
		LineAtr = Load.toInt();
		break;
	case 22: //SubAtrDef Установить атрибут подсписка по умолчанию
		SubAtr = Load.toInt();
		break;
	case 25: // BackOut Вылать входной объект для поиска
		if (Load.Type == TPPoint)
			(*(LoadPoint*)Load.Point) = Searcher.Obj;
		break;
	case 30: // ReceivedOutMk Вылаить МК со входным объектом для поиска
		MkExec(Load, Searcher.Obj);
		break;
	case 31: // ReceivedCopyOut Вылать входной объект для поиска
		Load.Write(Searcher.Obj.Clone());
		//if (Load.Type == TPPoint)
		//	(*(LoadPoint*)Load.Point) = Searcher.Obj.Clone();
		break;
	case 32: // ReceivedCopyOutMk Вылаить МК со входным объектом для поиска
		MkExec(Load, Searcher.Obj.Clone());
		break;

	case 35: // ReceivedLoadOut Выдать нагрузку входного объекта для поиска
		if (Searcher.Obj.isIC())
			Load.Write(((IC_type)(Searcher.Obj.Point))->begin()->Load);
		else if (Searcher.Obj.isIP())
			Load.Write(((ip*)Searcher.Obj.Point)->Load);
//		cout <<"Type: " << ((IC_type)(Searcher.Obj.Point))->begin()->Load.Type;
//		if (Searcher.Obj.isIC())
//			Load.Point = ((IC_type)(Searcher.Obj.Point))->begin()->Load.Point;
//		else if (Searcher.Obj.isIP())
//			Load.Point = ((ip*)(Searcher.Obj.Point))->Load.Point;
		break;
	case 36: // ReceivedLoadOutMk Выдать МК с нагрузкой входного объекта для поиска
		if (Load.isInt())
			if (Searcher.Obj.isIC())
				MkExec(Load, ((IC_type)(Searcher.Obj.Point))->begin()->Load);
			else if (Searcher.Obj.isIP())
				MkExec(Load, ((ip*)(Searcher.Obj.Point))->Load);
		break;
	case 37: // LineCountOut Выдать количество линий в списке
		if (ListHead.back() == nullptr)
			Load.Write(0);
		else
			Load.Write(ListHead.back()->size());
		break;
	case 38: // LineCountOutMK Выдать Мк с количеством линий в списке
	{
		long int t = 0;
		if (ListHead.back() != nullptr)
			t = ListHead.back()->size();
		MkExec(Load, { Cint, &t });
	}
	break;

	case 40: // ModeSet Установить режим работы списка (0 - список на основе ИК, 1 - список на основе хеш-таблицы)
		switch (Load.toInt())
		{
		case 0:
			Mode = 0; LineUk = { nullptr };
			break;
		case 1:
			Mode = 1; LineUk = { &HashLineUk };
			ListHead.back() = &HashListBack; // Добавить псевдосписок
			break;
		}
		break;
	case 41: // HashAtrSet Установить атрибут для хеширования
		HashAtr = Load.toInt();
		Mode = 1; LineUk = { &HashLineUk };
		ListHead.back() = &HashListBack; // Добавить псевдосписок
		break;
	case 100: // RezOut Выдача результата сравнения
		if (Load.isBool())
			Load.Write(Searcher.Rez);
		break;
	case 101: // RezOutMk Выдача МК с результатом сравнения
		MkExec(Load, { Cbool , &Searcher.Rez });
		break;
	case 105: // MkAtrAdd Добавить МК
		if (Load.isInt())
			Searcher.MkAtrAdd(*(int*)Load.Point);
		break;
	case 106: // MkAtrClear Очистить список МК
		Searcher.MkAtrClear();
		break;

	case 115: //MarkLastOut Выдать маркер последней строки
		if (!ListHead.size() || ListHead.back() == nullptr || !ListHead.back()->size()) break;
		Load.Write(&ListHead.back()->back());
		break;
	case 116: //MarkLastOutMk Выдать МК с маркой последней строки
		if (!ListHead.size() || ListHead.back() == nullptr || !ListHead.back()->size()) break;
		MkExec(Load, { TIP, &ListHead.back()->back() });
		break;
	case 117: //MarkPreLastOut Выдать маркер предпоследней строки
		if (!ListHead.size() || ListHead.back() == nullptr || ListHead.back()->size()<2) break;
		Load.Write((ip*)(ListHead.back()->end() -2)._Ptr);
		break;
	case 118: //MarPrekLastOutMk Выдать МК с маркой предпоследней строки
		if (!ListHead.size() || ListHead.back() == nullptr || ListHead.back()->size()<2) break;
		MkExec(Load, { TIP, (void*) (ListHead.back()->end() - 2)._Ptr});
		break;
	case 119: //MarkSet Установить текущую марку
		LineUk.back() = (ip*)Load.Point;
		break;

	case 120: // MarkAtrSet Установить атрибут текущей строки
		if (LineUk.back() != nullptr)
			LineUk.back()->atr = Load.toInt();
		break;
	case 121: // LastMarkAtrSet Установить атрибут последней строки
		if (ListHead.back() != nullptr)
			(ListHead.back()->end() - 1)->atr = Load.toInt();
		break;
	case 122: //MarkAtrOut Выдать маркер текущей строки
		if (LineUk.back() != nullptr)
			Load.Write(LineUk.back()->atr);
		break;
	case 123: //MarkAtrOutMk Выдать МК с маркером текущей строки
		if (LineUk.back() != nullptr)
			MkExec(Load, { Cint, &LineUk.back()->atr });
		break;
	case 124: //LastMarkAtrOut Выдать маркер последней строки
		if (ListHead.back() != nullptr && !ListHead.back()->size())
			Load.Write((ListHead.back()->end() - 1)->atr);
		break;
	case 129: //LastMarkAtrOutMk Выдать МК с маркером последней строки
		if (ListHead.back() != nullptr && !ListHead.back()->size())
			MkExec(Load, { Cint, &(ListHead.back()->end() - 1)->atr });
		break;
	case 125: //  MarkAtrInc Инкремент атрибута текущей строки
		if (LineUk.back() != nullptr)
			LineUk.back()->atr++;
		break;
	case 126: // LastMarkAtrInc Инкремент атрибута последней строки
		if (ListHead.back() != nullptr)
			(ListHead.back()->end() - 1)->atr++;
		break;
	case 127: //  MarkAtrDec Декримент атрибута текущей строки
		if (LineUk.back() != nullptr)
			LineUk.back()->atr--;
		break;
	case 128: // LastMarkAtrDec Декримент атрибута последней строки
		if (ListHead.back() != nullptr)
			(ListHead.back()->end() - 1)->atr--;
		break;
	case 130: // MarkAtrEQExec Запуск программы при нуле/равенстве атрибута текущей строки или равестве c нагрузкой МК
		if (ListHead.back() == nullptr || ListHead.back()->size() == 0 || LineUk.back() == nullptr) break;
		if (Load.isIC()) {
			if (!LineUk.back()->atr)
				ProgExec(Load);
		}
		else if (LineUk.back()->atr == Load.toInt())
			ProgExec(Prog);
		else
			ProgExec(ElseProg);
		break;
	case 131: // LastMarkEQExec Запуск программы при нуле/равенстве атрибута последней строки или равестве c нагрузкой МК
		if (ListHead.back() == nullptr || ListHead.back()->size() == 0) break;
		if (Load.isIC()) {
			if (!(ListHead.back()->end() - 1)->atr)
				ProgExec(Load);
		}
		else if ((ListHead.back()->end() - 1)->atr == Load.toInt())
			ProgExec(Prog);
		else
			ProgExec(ElseProg);
		break;
	case 133: // MarkAtrNEQExec Запуск программы при не нуле/неравенстве атрибута текущей строки
		if (ListHead.back() == nullptr || ListHead.back()->size() == 0 || LineUk.back() == nullptr) break;
		if (Load.isIC()) {
			if (LineUk.back()->atr)
				ProgExec(Load);
		}
		else if (LineUk.back()->atr != Load.toInt())
			ProgExec(Prog);
		else
			ProgExec(ElseProg);
		break;
	case 134: // LastMarkAtrNEQExec Запуск программы при не нуле/неравенстве атрибута последней строки
		if (ListHead.back() == nullptr || ListHead.back()->size() == 0) break;
		if (Load.isIC()) {
			if ((ListHead.back()->end() - 1)->atr)
				ProgExec(Load);
		}
		else if ((ListHead.back()->end() - 1)->atr != Load.toInt())
			ProgExec(Prog);
		else
			ProgExec(ElseProg);
		break;
	case 135: // MarkAtrBiggerExec Запуск программы при атрибуте текущей строки больше (больше 0)
		if (ListHead.back() == nullptr || ListHead.back()->size() == 0 || LineUk.back() == nullptr) break;
		if (Load.isIC()) {
			if (LineUk.back()->atr > 0)
				ProgExec(Load);
		}
		else if (LineUk.back()->atr > Load.toInt())
			ProgExec(Prog);
		else
			ProgExec(ElseProg);
		break;
	case 136: // LastMarkAtrBiggerExec Запуск программы при атрибуте последней строки больше (больше 0)
		if (ListHead.back() == nullptr || ListHead.back()->size() == 0) break;
		if (Load.isIC()) {
			if (!(ListHead.back()->end() - 1)->atr > 0)
				ProgExec(Load);
		}
		else if ((ListHead.back()->end() - 1)->atr > Load.toInt())
			ProgExec(Prog);
		else
			ProgExec(ElseProg);
		break;
	case 137: // MarkAtrBiggerEQExec Запуск программы при атрибуте текущей строки больше или райным 0
		if (ListHead.back() == nullptr || ListHead.back()->size() == 0 || LineUk.back() == nullptr) break;
		if (Load.isIC()) {
			if (LineUk.back()->atr >= 0)
				ProgExec(Load);
		}
		else if (LineUk.back()->atr >= Load.toInt())
			ProgExec(Prog);
		else
			ProgExec(ElseProg);
		break;
	case 138: // LastMarkAtrBiggerEQExec Запуск программы при атрибуте последней строки больше или равным 0
		if (ListHead.back() == nullptr || ListHead.back()->size() == 0) break;
		if (Load.isIC()) {
			if ((ListHead.back()->end() - 1)->atr >= 0)
				ProgExec(Load);
		}
		else if ((ListHead.back()->end() - 1)->atr >= Load.toInt())
			ProgExec(Prog);
		else
			ProgExec(ElseProg);
		break;
	case 139: // MarkAtrLessExec Запуск программы при атрибуте текущей строки меньше 0
		if (ListHead.back() == nullptr || ListHead.back()->size() == 0 || LineUk.back() == nullptr) break;
		if (Load.isIC()) {
			if (LineUk.back()->atr < 0)
				ProgExec(Load);
		}
		else if (LineUk.back()->atr < Load.toInt())
			ProgExec(Prog);
		else
			ProgExec(ElseProg);
		break;
	case 140: // LastMarkAtrLessExec Запуск программы при атрибуте последней строки меньше 0
		if (ListHead.back() == nullptr || ListHead.back()->size() == 0) break;
		if (Load.isIC()) {
			if ((ListHead.back()->end() - 1)->atr < 0)
				ProgExec(Load);
		}
		else if ((ListHead.back()->end() - 1)->atr < Load.toInt())
			ProgExec(Prog);
		else
			ProgExec(ElseProg);
		break;
	case 141: // MarkAtrLessEQExec Запуск программы при атрибуте текущей строки меньше или равным 0
		if (ListHead.back() == nullptr || ListHead.back()->size() == 0 || LineUk.back() == nullptr) break;
		if (Load.isIC()) {
			if (LineUk.back()->atr <= 0)
				ProgExec(Load);
		}
		else if (LineUk.back()->atr <= Load.toInt())
			ProgExec(Prog);
		else
			ProgExec(ElseProg);
		break;
	case 142: // LastMarkAtrLessEQExec Запуск программы при атрибуте последней строки меньше или равным 0
		if (ListHead.back() == nullptr || ListHead.back()->size() == 0) break;
		if (Load.isIC()) {
			if ((ListHead.back()->end() - 1)->atr <= 0)
				ProgExec(Load);
		}
		else if ((ListHead.back()->end() - 1)->atr <= Load.toInt())
			ProgExec(Prog);
		else
			ProgExec(ElseProg);
		break;

	case 150: //LastOut Выдать ссылку на последнюю линию списка
	case 151: //LastPop Выдать ссылку на последнюю линию списка и удалить из списка
	case 152: //LastDel Выдать ссылку на последнюю линию списка и удалить из ИК
		//if (Load.Type == Tvoid)	//!!! Сделать функцию isVoid
		if (!ListHead.size() || ListHead.back() == nullptr || !ListHead.back()->size()) break;
		Load.Write(ListHead.back()->back().Load);
		if (MK == 152) ICDel((void*)ListHead.back()->back().Load.Point);
		if (MK == 151 || MK == 152)
			ListHead.back()->pop_back();
		break;
	case 153: //LastIpOutMk Выдать ссылку на последнюю ИП последней линии
		MkExec(Load, { CIP,(void*)&((IC_type)ListHead.back()->back().Load.Point)->back() });
		break;
	case 154: // LastIpCopyOutMk Выдать МК со ссылкой на последнюю ИП последней линии
		MkExec(Load, { TIC,((IC_type)ListHead.back()->back().Load.Point)->back().Clone() });
		break;
	case 155: //LastOutMK Выдать ссылку на последнюю линию списка
	case 156: //LastPopMK Выдать ссылку на последнюю линию списка и удалить из списка
	case 157: //LastDelMk Выдать ссылку на последнюю линию списка и удалить из ИК
		if (!ListHead.size() || ListHead.back() == nullptr || !ListHead.back()->size()) break;
		MkExec(Load, ListHead.back()->back().Load);
		if (MK == 157) ICDel((void*)ListHead.back()->back().Load.Point);
		if (MK == 156 || MK == 157)
			ListHead.back()->pop_back();
		break;
	case 158: // LastLoadOut Выдать нагрузку последней ИП последней линии
		Load.Write(((IC_type)(ListHead.back()->back().Load.Point))->back().Load);
		//		*(LoadPoint*)Load.Point = ((IC_type)(ListHead.back()->back().Load.Point))->back().Load;
		break;
	case 169: // LastLoadOutMk Выдать МК с нагрузкой последней ИП последней линии
		MkExec(Load, ((IC_type)(ListHead.back()->back().Load.Point))->back().Load);
		break;
	case 257: // IndexVectPopMk -- forward this caps-manager FU to the Load MK
		// (CapsList.IndexVectPopMk=IndexFile.IndexVectWrite): dispatch the Load's
		// global MK with this List as the argument, so IndexFile can serialize it.
		MkExec(Load.toInt(), { TFU, this });
		break;
	case 285: // LastAtrOut Выдать атрибут последней ИП последней линии
		if (!ListHead.size() || ListHead.back()==nullptr || !ListHead.back()->size() || ListHead.back()->back().Load.Point==nullptr || !ListHead.back()->back().Load.IC()->size()) break;
		Load.Write(ListHead.back()->back().Load.IC()->back().atr);
		break;
	case 286: // LastAtrOutMk Выдать МК с атрибутом последней ИП последней линии
		if (!ListHead.size() || ListHead.back() == nullptr || !ListHead.back()->size() || ListHead.back()->back().Load.Point == nullptr || !ListHead.back()->back().Load.IC()->size()) break;
		MkExec(Load, { TAtr, &ListHead.back()->back().Load.IC()->back().atr });
		break;
	case 287: // LineAtrOut Выдать атрибут последней ИП текущей линии
		if (LineUk.back() == nullptr || LineUk.back()->Load.Point == nullptr || LineUk.back()->Load.IC()->size() < 2) break;
		Load.Write(ListHead.back()->back().Load.IC()->back().atr);
		break;
	case 288: // LineAtrOutMk Выдать МК с атрибутом последней ИП текущей линии
		if (LineUk.back() == nullptr || LineUk.back()->Load.Point == nullptr || LineUk.back()->Load.IC()->size() < 2) break;
		MkExec(Load, { Tint, &ListHead.back()->back().Load.IC()->back().atr });
		break;

	case 275: // LineIpTailToHead Переместить последнюю ИП в начало ИК в текущей линии
		if (LineUk.back() == nullptr || LineUk.back()->Load.Point == nullptr || LineUk.back()->Load.IC()->size() < 2) break;
		LineUk.back()->Load.IC()->insert(ListHead.back()->back().Load.IC()->begin(), ListHead.back()->back().Load.IC()->back());
		LineUk.back()->Load.IC()->erase(LineUk.back()->Load.IC()->end() - 1);
		break;
	case 276: // LastIpTailToHead Переместить последнюю ИП в начало ИК в последней линии
		if (!ListHead.size() || !ListHead.back()->size() || ListHead.back()->back().Load.Point == nullptr || ListHead.back()->back().Load.IC()->size() < 2) break;
		ListHead.back()->back().Load.IC()->insert(ListHead.back()->back().Load.IC()->begin(), ListHead.back()->back().Load.IC()->back());
		ListHead.back()->back().Load.IC()->erase(ListHead.back()->back().Load.IC()->end()-1);
		break;
	case 277: // LineIpHeadToTail Переместить первую ИП в конец ИК в текущей линии
		if (LineUk.back() == nullptr || LineUk.back()->Load.Point == nullptr || LineUk.back()->Load.IC()->size() < 2) break;
		LineUk.back()->Load.IC()->push_back(LineUk.back()->Load.IC()->front());
		LineUk.back()->Load.IC()->erase(LineUk.back()->Load.IC()->begin());
		break;
	case 278: // LastIpHeadToTail Переместить Первую ИП в конец ИК в последней линии
		if (!ListHead.size() || !ListHead.back()->size() || ListHead.back()->back().Load.Point == nullptr || ListHead.back()->back().Load.IC()->size() < 2) break;
		  ListHead.back()->back().Load.IC()->push_back( ListHead.back()->back().Load.IC()->front());
		  ListHead.back()->back().Load.IC()->erase(ListHead.back()->back().Load.IC()->begin());
		break;
	case 149: // LastIpSwap Поменять местами две последние ИП последней линии
		if (!ListHead.size() || !ListHead.back()->size() || ListHead.back()->back().Load.Point == nullptr || ListHead.back()->back().Load.IC()->size() < 2) break;
		swap(*(ListHead.back()->back().Load.IC()->back().Load.IC()->end() - 1), *(ListHead.back()->back().Load.IC()->back().Load.IC()->end() - 2));
		break;
	case 279: // LineIpSwap Поменять местами две последние ИП текущей линии
		if (LineUk.back() == nullptr || LineUk.back()->Load.Point==nullptr || LineUk.back()->Load.IC()->size() < 2) break;
		swap(*(LineUk.back()->Load.IC()->end() - 1), *(LineUk.back()->Load.IC()->end() - 2));
		break;
	case 280: // LineLoadAppend Добавить элемент в вектор в нагрузке последней ИП текущей строки
		if (LineUk.back() == nullptr || LineUk.back()->Load.IC()->back().Load.Point == nullptr || LineUk.back()->Load.IC()->back().Load.IC()->size()) break;
		if (LineUk.back()->Load.IC()->back().Load.Point == nullptr)
			LineUk.back()->Load.IC()->back().Load.Point = new vector<LoadPoint>;
		if (!LineUk.back()->Load.IC()->back().Load.isVect()) break;
		LineUk.back()->Load.IC()->back().Load.toVect()->push_back(Load);
		break;
	case 281: // LastLoadAppend Добавить элкскет в вектор в нагрузке последней ИП последней строки
		if (!ListHead.size() || !ListHead.back()->size() || ListHead.back()->back().Load.Point == nullptr ||
			ListHead.back()->back().Load.Point == nullptr || !ListHead.back()->back().Load.IC()->size()) break;
		if (ListHead.back()->back().Load.IC()->back().Load.Point == nullptr)
			ListHead.back()->back().Load.IC()->back().Load.Point = new vector<LoadPoint>;
		if (!ListHead.back()->back().Load.IC()->back().Load.isVect()) break;
		ListHead.back()->back().Load.IC()->back().Load.toVect()->push_back(Load);
		break;
	case 282: // LineToLoadVectWrite Записать нагрузку во все элементы вектора из нагрузки последней ИП текущей линии
		if (LineUk.back() == nullptr || LineUk.back()->Load.IC()->back().Load.Point == nullptr || LineUk.back()->Load.IC()->back().Load.IC()->size()) break;
		if (LineUk.back()->Load.IC()->back().Load.Point == nullptr)
			LineUk.back()->Load.IC()->back().Load.Point = new vector<LoadPoint>;
		if (!LineUk.back()->Load.IC()->back().Load.isVect()) break;
		for (auto& i : *LineUk.back()->Load.IC()->back().Load.toVect())
			i.WriteFromLoad(Load);
		break;
	case 283: // LastToLoadVectWrite Записать нагрузку во все элементы вектора из нагрузки последней ИП последней линии
		if (!ListHead.size() || !ListHead.back()->size() || ListHead.back()->back().Load.Point == nullptr ||
			ListHead.back()->back().Load.Point == nullptr || !ListHead.back()->back().Load.IC()->size()) break;
		if (ListHead.back()->back().Load.IC()->back().Load.Point == nullptr)
			ListHead.back()->back().Load.IC()->back().Load.Point = new vector<LoadPoint>;
		if (!ListHead.back()->back().Load.IC()->back().Load.isVect()) break;
		for (auto& i : *ListHead.back()->back().Load.IC()->back().Load.toVect())
			i.WriteFromLoad(Load);
		break;


	case 188: // LastLoadRefOut Выдать ссылку на Load последней ИП последней строки списка
		// Load.Write(&(IC_type)(ListHead.back()->back().Load.Point))->back().Load);
		break;
	case 198: // LastLoadRefOutMk Выдать МК со ссылкой на Load последней ИП последней строки списка
		MkExec(Load, { TLoad, &((IC_type)(ListHead.back()->back().Load.Point))->back().Load });
		break;
	case 145: //LineIpOutMk Выдать ссылку на последнюю ИП текущей линии
//		if (ListHead.back() == nullptr || !ListHead.back()->size() || LineUk == nullptr) break;
		if (LineUk.back() == nullptr || !((IC_type)LineUk.back()->Load.Point)->size()) break;
		MkExec(Load, { CIP,&((IC_type)LineUk.back()->Load.Point)->back() });
		break;
	case 146: // LineIcCopyOutMk Выдать МК со ссылкой на последнюю ИП текущей линии
		//		MkExec(Load, { CIC,((IC_type)ListHead.back()->back().Load.Point)->back().СloneToIC() });
		break;
	case 147: // LastIpOutMk Выдать ссылку на последнюю ИП текущей линии
		MkExec(Load, { CIP,&((IC_type)ListHead.back()->back().Load.Point)->back() });
		break;
	case 192: // LineCopyAddPrevSet Добавить копию строки и установить ссылку на нее в предыдущую линию
	{
		if (ListHead.back()->size()==0) // Если список пустой, добавить линию
			ListHead.back()->push_back({ LineAtr, TIC, nullptr });
		ListHead.back()->push_back({ LineAtr,Load.Copy()});// ->push_back(LineAtr, TIC, new vector<ip>);
		if (Load.Point == nullptr)
			ListHead.back()->back().Load = { TIC,new vector<ip> };
		ListHead.back()->at(ListHead.back()->size() - 2).Load = ListHead.back()->back().Load;
		break;
	}
	case 163: // LineCopyAddPrevLoadSetLoadMov
	case 159: // LastCopyAddPrevLoadSetLoadMov Добавить копию линии перенести нагрузку в на новую строку и добавить в нагрзуку предыдущей строки ссылку на новую строку 
	{
		LoadPoint t = { 0, nullptr };
		if (!ListHead.empty() && ListHead.back() != nullptr && ListHead.back()->size() && ListHead.back()->back().Load.isIC()
		    && ListHead.back()->back().Load.Point != nullptr && !((IC_type)ListHead.back()->back().Load.Point)->empty())
			t = ((IC_type)ListHead.back()->back().Load.Point)->back().Load;
		if (ListHead.back() == nullptr)
			ListHead.back() = new vector<ip>;
		if (Load.Point == nullptr)
			ListHead.back()->push_back({ LineAtr, TIC, new vector<ip> });
		else
			ListHead.back()->push_back({ LineAtr, TIC, ICCopy(Load).Point });
		if (ListHead.back()->size() > 1 && ListHead.back()->back().Load.Point != nullptr && ListHead.back()->back().Load.isIC()
		    && ListHead.back()->at(ListHead.back()->size() - 2).Load.Point != nullptr
		    && !((IC_type)ListHead.back()->at(ListHead.back()->size() - 2).Load.Point)->empty())
			((IC_type)ListHead.back()->at(ListHead.back()->size() - 2).Load.Point)->back().Load = ListHead.back()->back().Load;
		else if (ListHead.back()->size() == 1) // Добавление в пустой список
		{
			ListHead.back()->insert(ListHead.back()->begin(), {LineAtr, ListHead.back()->back().Load});
		}
		if (MK == 159 && ListHead.back()->back().Load.Point != nullptr && !((IC_type)ListHead.back()->back().Load.Point)->empty())
			((IC_type)ListHead.back()->back().Load.Point)->back().Load = t;
		break;
	}
	case 232: // RowsAppend -- append every row of the Load list (deep copy) into the current list
		if (ListHead.back() == nullptr) ListHead.back() = new vector<ip>;
		if (Load.isIC() && Load.Point != nullptr)
			for (auto& _r : *(IC_type)Load.Point)
				ListHead.back()->push_back({ _r.atr, TIC, ICCopy(_r.Load, true).Point });
		break;
	case 233: // DelFind -- delete the FIRST row whose Mnemo (first field) matches the key (Delphi mk170 DelFindIcLines, MultiLineMode=0)
	{
		if (ListHead.back() != nullptr && Load.Point != nullptr) {
			std::string _kn;
			if (Load.isIC() && ((IC_type)Load.Point)->size() && (*(IC_type)Load.Point)[0].Load.isStr()) _kn = (*(IC_type)Load.Point)[0].Load.toStr();
			else if (Load.Type>>1==DIP && ((ip*)Load.Point)->Load.isStr()) _kn = ((ip*)Load.Point)->Load.toStr();
			if (!_kn.empty()) {
				auto& _rows = *ListHead.back();
				for (size_t _i=0; _i<_rows.size(); _i++) {
					std::string _rn;
					if (_rows[_i].Load.isIC() && ((IC_type)_rows[_i].Load.Point)->size() && (*(IC_type)_rows[_i].Load.Point)[0].Load.isStr()) _rn = (*(IC_type)_rows[_i].Load.Point)[0].Load.toStr();
					if (_rn == _kn) { _rows.erase(_rows.begin()+_i); break; }
				}
			}
		}
	}
	break;
	case 160: // LineAdd Добавить новую строку всписок
		if (ListHead.back() == nullptr)
			ListHead.back() = new vector<ip>;
	    ListHead.back()->push_back({ LineAtr, Load.Clone() });
		break;
	case 161: // LineCopyAdd Добавить копию строки
	case 162: // LineTreeCopyAdd Добавить копию ОА-графа
		if (ListHead.back() == nullptr) ListHead.back() = new vector<ip>;
		if (Load.Point != nullptr)
			ListHead.back()->push_back({ LineAtr, TIC, ICCopy(Load, MK == 162).Point }); // Копирование ИК при МК = 162
		else
			ListHead.back()->push_back({ LineAtr, TIC, new vector<ip> }); // Создание пустой строки
		break;
		//	case 163: //  LineCopyAddPrevLoadSet Добавить линию в список и поместить ссылку на нее в нагрузку последней ИП последней строки
		//	 	if (ListHead.back() == nullptr) ListHead.back() = new vector<ip>;
		//		if(Load.Point==nullptr)
		//			ListHead.back()->push_back({ LineAtr, TIC, new vector<ip> });
		//		else
		//			ListHead.back()->push_back({ LineAtr, TIC, ICCopy(Load) });
		//		if (ListHead.back()->size() > 1)
		//			((IC_type)ListHead.back()->at(ListHead.back()->size() - 2).Load.Point)->back().Load = ListHead.back()->back().Load;
		//		break;
	case 181: // LineFirstLoadOutMk Выдать МК с нагрузкой последней ИП текущей линии
		MkExec(Load, ((IC_type)(LineUk.back()->Load.Point))->begin()->Load);
		break;
	case 164: // LineLoadOutMk Выдать МК с нагрузкой первой ИП текущей линии
		MkExec(Load, ((IC_type)(LineUk.back()->Load.Point))->back().Load);
		break;
	case 165: // LinePopMk Исключить линию списка
		if (ListHead.back() == nullptr || ListHead.back()->size() == 0) break;
		MkExec(Load, { TIC , (void*)ListHead.back()->back().Load.Point });
		ListHead.back()->pop_back();
		break;
	case 166: // LineDelMk Выдать МК со ссылкой на ИК и удалить ИК
	{
		if (ListHead.back() == nullptr || ListHead.back()->size() == 0) break;
		LoadPoint t = ListHead.back()->back().Load;
		ListHead.back()->pop_back();
		MkExec(Load, t);
		ICDel(ListHead.back()->back().Load.Point);
		break;
	}
	case 167: // LineIpCut Удалить последние ИП из текущей линии (в нагрузке количество удаляемых ИП, по умолчанию 1)
	case 168: // LineVarIpCut" Hint = "Удалить последние ИП c пемеменными из текущей линии (в нагрузке количество удаляемых ИП, по умолчанию 1)" }
		if (LineUk.back() != nullptr && LineUk.back()->Load.isIC() && LineUk.back()->Load.Point != nullptr)
		{
			int n;
			if (Load.Point == nullptr || !Load.isInt())
				n = 1;
			else
				n = *(int*)Load.Point;
			while (n-- && ((IC_type)LineUk.back()->Load.Point)->size() != 0)
			{
				((IC_type)LineUk.back()->Load.Point)->resize(((IC_type)LineUk.back()->Load.Point)->size() - 1);
			}
		}
		break;
	case 270: // LineUpDown Сместить указатель текущей строки (в нагрузке величина спещения, при отрицательно смещении движение вверх; по умолчанию движение вверх на одну позицию)
		
		break;
//	case 174: // LastLoadConstVarTypeSet Установить тип константа/переменная для последней нагрузки (по умолчанию переменная)
//	if (ListHead.size() && ListHead.back() != nullptr)
//	{
//		((IC_type)ListHead.back()->back().Load.Point)->back().Load.Type |= 1;
//		if (!Load.toBool(false))
//			((IC_type)ListHead.back()->back().Load.Point)->back().Load.Type -= 1;
//	break;
//	}
	case 170: // LastAttach Конкатенация ИК к последней линии списка
	case 171: // LastTreeCopyAttach Конкатенация копии ИК к последней линии списка
	case 172: // LastCopyAttachLoadMove Добавить ИК и переписать последнюю нагрузку
	{
		if (ListHead.back() == nullptr)
		{
			ListHead.back() = new vector<ip>;
			ListHead.back()->push_back({ LineAtr,{ TIC,new vector<ip>} });

		}
		if (!ListHead.back()->size())
			ListHead.back()->push_back( {LineAtr, TIC, new vector<ip> }); // Добавить пустую строку в список
		if (Load.Point == nullptr)
			((IC_type)ListHead.back()->back().Load.Point)->push_back({ 0,0, nullptr }); // Добавить пустую ИП, если Load==nil
//		else if (Load.isIP() || Load.isIC())
		else if (Load.isIP())
		{
			if (Load.isIP())
			{
				((IC_type)ListHead.back()->back().Load.Point)->push_back(((ip*)Load.Point)->Copy());
				//if (MK == 170 && Load.Type == CIP) { delete (ip*)Load.Point; Load.Point = nullptr; Load.Type = 0; }
			}
			else if (Load.isIC())
			{
				for (auto i : *(IC_type)Load.Point)
					((IC_type)ListHead.back()->back().Load.Point)->push_back(i);
			//	if (MK == 170 && Load.Type == CIP) { delete (ip*)Load.Point; Load.Point = nullptr; Load.Type = 0; }
			}
		}
		else
		{
			if (Load.isIC())
			{
				copy(((IC_type)Load.Point)->begin(), ((IC_type)Load.Point)->end(), inserter(*((IC_type)ListHead.back()->back().Load.Point), ((IC_type)ListHead.back()->back().Load.Point)->end()));
				// if (MK == 170 && Load.Type == CIP) { ICDel(Load.Point); Load.Point = nullptr; Load.Type = 0; }; // ???
				if (MK == 171) // Копирование дерева
				{
					for (auto i = ListHead.back()->back().Load.IC()->begin() + ListHead.back()->back().Load.IC()->size() - Load.IC()->size(); i != ListHead.back()->back().Load.IC()->end(); i++)
					{
						i->Load.Copy(i->Load);
						if (i->Load.isIC() && ((i->Load.Type & 1) == 0 || MK == 171)) // Ссылка на ИС как переменная (ее будем копировать)
							i->Load = ICCopy(i->Load, true);
					}
				}
			}
		}
		break;
	}
	case 173: // LastCopyGrahpAttach
		// ....
		break;
	case 175: // LineAttach Конкатенация ИК к текущей линии списка
	case 176: // LineCopyAttach Конкатенация копии ИК к текущей линии списка
		if (LineUk.back() != nullptr && Load.Point != nullptr)
			if (Load.isIP())
			{
				((IC_type)LineUk.back()->Load.Point)->push_back(*(ip*)Load.Point);
				if (MK == 175 && Load.Type == CIP) { delete (ip*)Load.Point; Load.Point = nullptr; Load.Type = 0; }
			}
			else
			{
				if (Load.isIC())
				{
					copy(((IC_type)Load.Point)->begin(), ((IC_type)Load.Point)->end(), inserter(*((IC_type)LineUk.back()->Load.Point), ((IC_type)LineUk.back()->Load.Point)->end()));
					if (MK == 175 && Load.Type == CIP) { ICDel(Load.Point); Load.Point = nullptr; Load.Type = 0; };
				}
			}
		break;
	case 178: // LineCopyToLast: append all ips of the FOUND row (LineUk) onto the LAST row ip-list (generic clone-found-fields). Shares each ip Load.Point like the legacy addUserFuMnemoRow peer-share.
		if (!LineUk.empty() && LineUk.back() != nullptr && LineUk.back()->Load.Point != nullptr && LineUk.back()->Load.isIC()
			&& !ListHead.empty() && ListHead.back() != nullptr && !ListHead.back()->empty()
			&& ListHead.back()->back().Load.Point != nullptr && ListHead.back()->back().Load.isIC())
		{
			IC_type src178 = (IC_type)LineUk.back()->Load.Point;
			IC_type dst178 = (IC_type)ListHead.back()->back().Load.Point;
			for (auto& f178 : *src178) {
				dst178->push_back(f178);
				if (dst178->back().Load.isIC()) // fresh copy of {FU=..} / MkTable ICs so clones don't share a mutable sub-cap
					dst178->back().Load = ICCopy(dst178->back().Load, false);
			}
		}
		break;
	case 148: // LastSubLoadSet: set value of the last field of the LAST row last-field load IC (deepest nested last value; used to overwrite a copied {FU=peerRange} with the new FU own range).
		if (!ListHead.empty() && ListHead.back() != nullptr && !ListHead.back()->empty()
			&& ListHead.back()->back().Load.Point != nullptr && ListHead.back()->back().Load.isIC()
			&& !((IC_type)ListHead.back()->back().Load.Point)->empty())
		{
			LoadPoint& lastField148 = ((IC_type)ListHead.back()->back().Load.Point)->back().Load;
			if (lastField148.Point != nullptr && lastField148.isIC() && !((IC_type)lastField148.Point)->empty())
				((IC_type)lastField148.Point)->back().Load = Load.Clone();
		}
		break;
	case 179: // LineCopyGrahpAttach
		// ....
		break;
	case 184: // LastIndSet Установить индекс нагрузки в последней ИП последней строки
	{
		if (ListHead.back() == nullptr || ListHead.back()->back().Load.Point == nullptr || !ListHead.back()->back().Load.isIC()) break;
		IC_type t = ((IC_type)ListHead.back()->back().Load.Point);
		if (t->size() == 0) break;
		if (t->back().Load.isVect())
			if (((LoadVect_type)t->back().Load.Point)->size() > abs(Load.toInt()) or -Load.toInt() == ((LoadVect_type)t->back().Load.Point)->size())
			{
				if (t->back().Load.Type == TLoadVect)
					t->back().Load.Type = TLoadVectInd;
				else 
					t->back().Load.Type == CLoadVectInd;
				t->back().Load.Ind = Load.toInt();
			}
			else if (((IC_type)ListHead.back()->back().Load.Point)->back().Load.isIC())
				if (Load.toInt() >= 0 and Load.toInt() < ((IC_type)ListHead.back()->back().Load.Point)->size() * 3) {
					if (ListHead.back()->back().Load.Type == TIC)ListHead.back()->back().Load.Type == TICInd;
					else ListHead.back()->back().Load.Type == CICInd;
					ListHead.back()->back().Load.Ind = Load.toInt();
				}
		break;
	}
	case 183: // LineIndSet Установить индекс нагрузки в последней ИП текушей строки
		if (ListHead.back() == nullptr || ListHead.back()->back().Load.Point == nullptr || !ListHead.back()->back().Load.isIC()) break;
		if (((IC_type)ListHead.back()->back().Load.Point)->size() == 0) break;
		if (LineUk.back() == nullptr) break;
		if (!LineUk.back()->Load.isIC()) break;
		if (!((IC_type)LineUk.back()->Load.Point)->size())break;
		if (((IC_type)LineUk.back()->Load.Point)->back().Load.isVect())
			if (((LoadVect_type)LineUk.back()->Load.Point)->size() > abs(Load.toInt()) or -Load.toInt() == ((LoadVect_type)LineUk.back()->Load.Point)->size()){
				if (((IC_type)(LineUk.back()->Load.Point))->back().Load.Type == TLoadVect) ((IC_type)(LineUk.back()->Load.Point))->back().Load.Type == TLoadVectInd;
				else ((IC_type)(LineUk.back()->Load.Point))->back().Load.Type == CLoadVectInd;
				((IC_type)(LineUk.back()->Load.Point))->back().Load.Ind = Load.toInt();
			}
		if (((IC_type)LineUk.back()->Load.Point)->back().Load.isVect())
			if (Load.toInt() >= 0 and Load.toInt() < ((IC_type)LineUk.back()->Load.Point)->size() * 3) {
			if (((IC_type)(LineUk.back()->Load.Point))->back().Load.Type == TIC) ((IC_type)(LineUk.back()->Load.Point))->back().Load.Type == TICInd;
			else ((IC_type)(LineUk.back()->Load.Point))->back().Load.Type == CICInd;
			((IC_type)(LineUk.back()->Load.Point))->back().Load.Ind = Load.toInt();
		}
		break;
	case 193: // LastLoadTypeSet Установить тип нагрузки последней ИП последней линии
		//if(ListHead.back()!=nullptr && ListHead.back()->back().Load.isNil() && (IC_type)ListHead.back()->back().Load.isIC() && !(IC_type)ListHead.back()->back().Load.isEmptyIC())
		if (ListHead.back() != nullptr)
			((IC_type)ListHead.back()->back().Load.Point)->back().Load.Type = Load.toInt();
		break;
	case 194: // LineLoadTypeSet Установить тип нагрузки последней ИП последней линии
		if (LineUk.back() != nullptr && !(IC_type)LineUk.back()->Load.isNil() && ((IC_type)LineUk.back()->Load.Point)->back().Load.isIC() && !((IC_type)LineUk.back()->Load.Point)->back().Load.isEmptyIC())
			((IC_type)LineUk.back()->Load.Point)->back().Load.Type = Load.toInt();
		break;

	case 185: // LastLoadSet Записать адрес в нагрузку последней ИП последней строки
	case 195: // LineLoadSet Записать адрес в нагрузку последней ИП последней строки
	case 186: // LastLoadCopySet Записать адрес в нагрузку последней ИП последней строки
	case 196: // LineLoadCopySet Записать адрес в нагрузку последней ИП текущей строки
	case 187: // LastPointLoadCopyGraphSet
	case 197: // LinePointLoadCopyGraphSet
	case 189: // LastAtrSet Установить атрибут у последней ИК последней линии 
	case 199: // LineAtrSet Установить атрибут у последней ИК последней линии 
	{	IC_type t;
	if (ListHead.empty() || ListHead.back() == nullptr || ListHead.back()->empty()) break;
	if (ListHead.back()->back().Load.Point == nullptr || !ListHead.back()->back().Load.isIC()) break;
	if (((IC_type)ListHead.back()->back().Load.Point)->size() == 0) break;
	if (MK < 190)
		t = ((IC_type)ListHead.back()->back().Load.Point);
	else if (LineUk.back() != nullptr && LineUk.back()->Load.Point != nullptr && LineUk.back()->Load.isIC())
		break;
	else
		t = (IC_type)LineUk.back()->Load.Point;
	switch (MK)
	{
	case 185:
	case 195:
	//	t->back().Load = Load; break;
		t->back().Load = Load.Clone();	break;
	case 186:
	case 196:
		if (Load.isIC())
			t->back().Load.Copy(Load);
		else
		{
			t->back().Load.Type = Load.Type;
			if(Load.isIC())
				t->back().Load.Point = ICCopy(Load).Point;
			else
				t->back().Load.Copy(Load); // Заглушка!!!
		}
		break;
	case 189:
	case 199:
		//if (Load.Type >> 1 == Dint)
		t->back().atr = Load.toInt();
		break;
		//	case 177: // LastPointLoadCopyGraphSet
				// ...
				//break;
	}

	break;
	}
	case 180: // LineVarTypeSet // Установить тип 'переменная' в последней ИП текущей линии
		if (ListHead.back() == nullptr || !ListHead.back()->size() || \
			LineUk.back() == nullptr || LineUk.back()->Load.Point == nullptr || \
			LineUk.back()->Load.isIC() || !((IC_type)LineUk.back()->Load.Point)->size())
			break;
		((IC_type)LineUk.back()->Load.Point)->back().Load.VarTypeSet(Load.toBool(true));
		break;
	case 182: // LastVarTypeSet // Установить тип 'переменная' в последней ИП последней линии
		if (ListHead.back() == nullptr || !ListHead.back()->size() || \
			!ListHead.back()->back().Load.isIC() || !((IC_type)ListHead.back()->back().Load.Point)->size())
			break;
		((IC_type)ListHead.back()->back().Load.Point)->back().Load.VarTypeSet(Load.toBool(true));
		break;

	case 143: //LastClear Очистить последниюю строку списка
		if (!ListHead.size() || ListHead.back() == nullptr || ListHead.back()->back().Load.Point == 0) break;
		//ICDel(ListHead.back()->back().Load.Point);
		ListHead.back()->back().Load.IC()->clear();
		break;
	case 144: //LineClear Очистить текущую строку списка
		if (LineUk.back() == nullptr || LineUk.back()->Load.Point == nullptr) break;
		//ICDel(LineUk.back()->Load.Point);
		LineUk.back()->Load.IC()->clear();
		break;

	case 190: // LastLoadIcEmptySet Установить ссылку на пустую ИК в нагрузке последней ИП последней строки
		if (ListHead.back() != nullptr && !ListHead.size() && ((IC_type)ListHead.back()->back().Load.isIC()))
			((IC_type)ListHead.back()->back().Load.Point)->back().Load = { TIC, new vector<ip> };
		break;
	case 191: // LineLoadIcEmptySet Установить ссылку на пустую ИК в нагрузке последней ИП текущей строки
		if (LineUk.back() != nullptr && ((IC_type)LineUk.back()->Load.isIC()))
			((IC_type)LineUk.back()->Load.Point)->back().Load = {TIC, new vector<ip>};
		break;
	case 200: // LineToLast Установить текущую строку на последнюю строку
		if (ListHead.back() != nullptr && ListHead.back()->size() > 0 && ListHead.back()->back().Load.Point != nullptr && ListHead.back()->back().Load.isIC())
			LineUk.back() = &ListHead.back()->back();
		break;
	case 205: // LinePush Положить текущую линию в стек
		if (LineNum >= 0 && LineNum < ListHead.back()->size() > 0)
			LineStack.push_back(LineNum);
		break;
	case 206: // LineLastPush Положить последнюю линию в стек
		if (ListHead.back()->size() > 0) LineStack.push_back(ListHead.back()->size() - 1);
		break;
	case 207: // LinePop Взять текущую линию из стека
		if (LineStack.size() && ListHead.back()->size() > LineStack.back() && LineStack.back() >= 0)
		{
			LineNum = LineStack.back();
			LineStack.pop_back();
			LineUk.back() = ListHead.back()->begin()._Ptr + LineNum;
		}
		break;

	case 210: // LastAtrAttach Прикрепить к последней строке ИП с атрибутом из нагрзуки
		if (ListHead.back() == nullptr)
			ListHead.back() = new vector<ip>;
		if (!ListHead.back()->size())
			ListHead.back()->push_back({ LineAtr, TIC, new vector<ip> });
		((IC_type)ListHead.back()->back().Load.Point)->push_back({ Load.toInt() ,0, nullptr});
		break;
	case 211: // LineAtrAttach Прикрепить к текущей строке ИП с атрибутом из нагрзуки
		if (LineUk.back() == nullptr) break;
		if (LineUk.back()->Load.Point == 0)
			LineUk.back()->Load = { TIC, new vector<ip> };
		((IC_type)LineUk.back()->Load.Point)->push_back({ Load.toInt() ,0, nullptr });
		break;
	case 215: // LastLoadBackSet Записать нагрзуку последней ИП последний строки в предыдущие ИП (на входе количество обратных записей)
	{
		if (!ListHead.back() || !ListHead.back()->size() || !ListHead.back()->back().Load.isIC() ||
			((IC_type)ListHead.back()->back().Load.Point)->size()<2) break;
		auto uk = ((IC_type)ListHead.back()->back().Load.Point)->rbegin() + 1;
		//auto uk2 = ((IC_type)ListHead.back()->back().Load.Point)->end() - 2;
		int i = Load.toInt();
		for (; i > 0 && uk != ((IC_type)ListHead.back()->back().Load.Point)->rend(); i--, uk++)
			uk->Load = ((IC_type)ListHead.back()->back().Load.Point)->back().Load.Clone();
		break;
	}
	case 216: // LineLoadBackSet Записать нагрзуку последней ИП некущей строки в предыдущие ИП (на входе количество обратных записей)
	{
		if (LineUk.back() == nullptr || ((IC_type)LineUk.back()->Load.Point)->size() < 2) break;
		auto uk = ((IC_type)LineUk.back()->Load.Point)->end() - 2;
		for (int i = Load.toInt(); i > 0 && uk != ((IC_type)LineUk.back()->Load.Point)->begin() - 1; i++, uk--)
			uk->Load = ((IC_type)LineUk.back()->Load.Point)->back().Load.Clone();
		break;
	}
	case 219: // Receive Установить входной объект без его поиска
		Searcher.Obj = Load;
		break;

	case 220: //FindOrLast
		//if(ListHead!=nullptr)
		Searcher.Template = { 0,nullptr };
		if(ListHead.size() && ListHead.back() != nullptr && ListHead.back()->size())
			Searcher.Template = ListHead.back()->back().Load;
		Searcher.FindOr(Load);
		break;
	case 221: //FindOrLine
		if (LineUk.back() == nullptr) Searcher.Template = { 0,nullptr };
		else Searcher.Template = LineUk.back()->Load;
		Searcher.FindOr(Load);
		break;
	case 222: //FindAndLast
		Searcher.Template = { 0,nullptr };
		if (ListHead.size() && ListHead.back() != nullptr && ListHead.back()->size())
			Searcher.Template = ListHead.back()->back().Load;
		Searcher.FindAnd(Load);
		break;
	case 223: //FindAndLine
		if (LineUk.back() == nullptr) Searcher.Template = { 0,nullptr };
		else Searcher.Template = LineUk.back()->Load;
		Searcher.FindAnd(Load);
		break;
	case 224: //FindXorLast
		Searcher.Template = { 0,nullptr };
		if (ListHead.size() && ListHead.back() != nullptr && ListHead.back()->size())
			Searcher.Template = ListHead.back()->back().Load;
		Searcher.FindXor(Load);
		break;
	case 225: //FindXorLine
		if (LineUk.back() == nullptr) Searcher.Template = { 0,nullptr };
		else Searcher.Template = LineUk.back()->Load;
		Searcher.FindXor(Load); // Заглушка !!!
		break;
	case 226: //FindAndSourceLast
		Searcher.Template = { 0,nullptr };
		if (ListHead.size() && ListHead.back() != nullptr && ListHead.back()->size())
			Searcher.Template = ListHead.back()->back().Load;
		Searcher.FindAndSource(Load);
		break;
	case 227: //FindAndSourceLine
		Searcher.Template = LineUk.back()->Load;
		Searcher.FindAndSource(Load); // Заглушка !!!
		break;

	case 228: // FindOr Поиск ИЛИ
	case 229: // FindAnd Поиск И
	case 230: // FindXor Поиск XOR
	case 231: // FindAndSource Поиск И в источнике
	{
		if (!ListHead.size()) break;
		//	if (DeepStartSearch == 0)
		{
			{
				//DeepStartSearch = ListHead.size();
				LineCount = 0; // счетчик совпадений
				LineNumOld = LineNum;
				LineUk.back() = nullptr; // Ссылка на найденную строку
				LineNum = -1;
			}
			if (ListHead.back() == nullptr)
			{
				Searcher.Template = { 0,nullptr };
				Searcher.FindOr({ 0,nullptr });
				ProgExec(FailProg);
				if (FailProgs.size())
					ProgExec(FailProgs.back());
				break;
			}
			vector<ip> templ;
			if (Load.isAtr() || Load.isInt())
			{
				templ.push_back({ Load.toInt(),{0,nullptr} });
				Load = { CIC, &templ};
			}
			vector<vector<ip>::iterator> i = { ListHead.back()->begin() };
			vector<IC_type> IC = { ListHead.back() }; // Указатель на текущий уровень списка
			long k =-1;
			while (i.size())
			{
				while (i.back() != IC.back()->end())
				{
					k++;
					if (i.back()->atr != LineAtr || i.back()->Load.isNil() || !i.back()->Load.isIC())
						continue;

					Searcher.Template = i.back()->Load;
					switch (MK)
					{
					case 228: // FindOr		
						Searcher.FindOr(Load);
						break;
					case 229: // FindAnd 
						Searcher.FindAnd(Load);
						break;
					case 230: // FindXor
						Searcher.FindXor(Load);
						break;
					case 231: // FindAndSource
						Searcher.FindAndSource(Load);
						break;
					}
					if (Searcher.Rez)
					{
						LineCount++;
						if (LineCount == 1)
						{
							//if (MK % 231 == 1)
							//	LineNum = ListHead.back()->size() - 1;
							//else
							{
								LineNum = k;// Запомнить номер первой строки (k- счетчик номеров строк в том числе и в иерархическом списке)
								LineUk.back() = i.back()._Ptr; // Запомнить найденную строку
							}

						}
						if (MultiLineMode == 0) break;
					}
					ip* t = AtrSearchIP(i.back()->Load.Point, LineAtr);
					if (MultiListMode && t != nullptr) // Попытка перехода на другой уровень списка
					{
						i.back()++; // Сохранить позицию для продолжения поиска на текущем уровне после возвращения	
						IC.push_back((IC_type)t->Load.Point); // Добавить контекст в стек контекстов
						i.push_back(IC.back()->begin()); // Добавить итератор на новый уровень списка
						continue;
					}
					i.back()++;
				}
				i.pop_back();
				IC.pop_back();
				if (Searcher.Rez && MultiListMode == 0) // Если поиск до первого совпадения и совпадение найдено, то выход
					i.clear();
			}
			if (LineCount /* && ListHead.size() == DeepStartSearch */)
			{
				//LineUk = &ListHead.back()->at(LineNumFirst);
				ProgExec(SuссessProg);
				if (SuссessProgs.size())
					ProgExec(SuссessProgs[ListHead.size() - 1]);
				//if (ListHead.size() == DeepStartSearch)
				{ // Обработка превывания по разности строк (например, применяется для определения приоритета АЛВ)
					if (LineNum >= 0 && LineNumOld >= 0) {
						if (LineNum > LineNumOld)  ProgExec(BibberProg);
						if (LineNum < LineNumOld)  ProgExec(LessProg);
						if (LineNum >= LineNumOld) ProgExec(BibberEQProg);
						if (LineNum < LineNumOld)  ProgExec(LessEQProg);
						if (LineNum == LineNumOld) ProgExec(EQProg);
					}
					//DeepStartSearch = 0;
				}
			}
			else
			{
				ProgExec(FailProg);
				if (FailProgs.size())
					ProgExec(FailProgs[ListHead.size() - 1]);
			}

		}
	}
	break;

	// Операции с подсписками
	case 238: // ToRoot Переход к корневому уровню списка
		while (ListHead.size() > 1)
			ListHead.pop_back(); // Возврат к корневому уровню
		break;
	case 240: // Push SubAdd Добавить ИК на новый уровень списока и спуститься на него
	case 241: // PushCopy SubCopyAdd Добавить копию ИК на новый уровень списка и спуститься на него
	case 242: // SubOACopy SubOACopyAdd Добавить копию ОА-графа на новый уровень списка и спуститься на него
	case 243: // PushTied SubRootAdd Добавить ИК на новый уровень списока и спуститься на него оставить в стеке уровней ссылку на нов. список
	case 244: // PushTiedCopy SubRootCopyAdd Добавить копию ИК на новый уровень списка и спуститься на него оставить в стеке уровней ссылку на нов. список
	case 245: // PushTiedOACopy SubRootOACopyAdd Добавить копию ОА-графа на новый уровень списка и спуститься на него оставить в стеке уровней ссылку на нов. список
	case 246: // PushTiedLine Добавить ИК на новый уровень списока и спуститься на него оставить в стеке уровней ссылку на нов. список
	case 247: // PushTiedLineCopy Добавить копию ИК на новый уровень списка и спуститься на него оставить в стеке уровней ссылку на нов. список
	case 248: // PushTiedLineOACopy Добавить копию ОА-графа на новый уровень списка и спуститься на него оставить в стеке уровней ссылку на нов. список
		ListHead.push_back(new vector<ip>);
		if (Load.isIC())
			if (MK == 240 || MK == 243 || MK == 246)
			{
	//			ListHead.back() = new vector<ip>;
				ListHead.back()->push_back({ LineAtr, Load.Clone() });
			}
			else if (MK == 241 || MK == 244 || MK == 247)
			{
		//		ListHead.back() = new vector<ip>;
				ListHead.back()->push_back({ LineAtr, Load.Copy() });
			}
			else
				; // Копия ОА-графа
		if (MK >= 243) //Установить ссылку с предыдущего уровня
		{
		//	if (ListHead.back() == nullptr)
		//		ListHead.back()=new vector<ip>;
			{
				auto prev = (ListHead.size() > 1) ? *(ListHead.end() - 2) : nullptr;
				if (prev != nullptr && !prev->empty() && prev->at(0).Load.isIC()
				    && prev->back().Load.Point != nullptr && !((IC_type)prev->back().Load.Point)->empty())
				{
					if(MK==243 || MK==244 || MK==245)
						((IC_type)prev->back().Load.Point)->back().Load = { TIC, ListHead.back() };
					else
					{
						if(!ListHead.back()->size())
							ListHead.back()->push_back({ LineAtr,TIC, new vector<ip> });
						((IC_type)prev->back().Load.Point)->back().Load = ListHead.back()->back().Load;
					}
				}
			}
		}
//		if (ListHead.back()->size() == 0)
//		{
//			delete ListHead.back();
//			ListHead.back() = nullptr;
//		}
		break;
	
	case 260: // Pop Вытолкнуть подсписок
	case 261: // PopMk Вытолкнуть подсписок и выдать МК с указателем на него
	case 262: // PopMkDel Вытолкнуть подсписок и выдать МК с указателем на него и удалить нагрузку
		{
			void* t = ListHead.back();
			if (ListHead.size() > 1)
				ListHead.pop_back();
			if (MK == 260)
				Load.Write(t);
			else
				MkExec(Load, { CIP,t });
			if (MK == 262)
			{
				ICDel((IC_type)t);
				//delete (IC_type)t;
			}
		}
		break;
	case 249: // LevelPrevAdd: pop current sub-level, attach as prev level's last-line Load (TIC) -- OAP capsule nesting
	{
		if (ListHead.size() > 1)
		{
			IC_type sub = (IC_type)ListHead.back();
			ListHead.pop_back();
			if (ListHead.back() != nullptr && !ListHead.back()->empty())
				ListHead.back()->back().Load = { TIC, sub };
		}
		break;
	}
	case 263: // MarkLastCopyOutMk: dispatch a deep COPY of the last line's ip via Load MK (OAP sub-cap live-dispatch)
	{
		if (ListHead.size() && ListHead.back() != nullptr && ListHead.back()->size() && ListHead.back()->back().atr > 0)
		{
			ip tmp = ListHead.back()->back();
			tmp.Load = ListHead.back()->back().Load.Clone();
			MkExec(Load, { TIP, &tmp });
		}
		break;
	}
	case 265: // ZeroDeepExec Выполнить, если нет подсписков
		if (ListHead.size() == 0)
			ProgExec(Load);
		break;
	case 266: // OneDeepExec Выполнить, если на первом уровне подсписка
		if (ListHead.size() == 1)
			ProgExec(Load);
		break;
	case 267: // MoreOneDeepExec Выполнить, если более, чем на первом уровне подсписка
		if (ListHead.size() >1)
			ProgExec(Load);
		break;
	case 268: //
		break;
	case 250: // SubUp Перейти на уровень выше (на входе количество уровней наверх, по умолчанию 1; если <0, то переместиься в начало списка)
	case 251: // SubUpDel Перейти на уровень выше и уничтожить текущий уровень
	case 252: // SubUpDelGraph Перейти на уровень выше и уничтожить текущий уровень с подсписками
	{
			long int t = Load.toInt(1);
			if (t < 0) { t = ListHead.size() - 1; } // Если <0, то переместиься в начало списка
			for (; ListHead.size() > 1 && t > 0; t--)
			{
				if(MK==251) ICDel(ListHead.back()->back().Load.Point);
				if (MK == 252); //GraphDel(ListHead.back()->back().Load.Point)
				ListHead.pop_back();
				LineUk.pop_back();
			}
			break;
	}
	case 253: // SubDeepOut Выдать номер текущего уровня
		Load.Write(ListHead.size());
		break;
	case 254: // SubDeepOutMk Выдать МК с номером текущего уровня
	{
		long int t = ListHead.size();
		MkExec(MK, {Cint,&t});
		break;
	}
	case 255: // SubDown Перейти на уровень ниже (переход осуществляется на текущей найденной строке) в нагрузке атрибут указателя на подсписок (по умолчанию атрибут строки)
			  // Если в нагрузке IC, то переход текущего списка на этот указатель
		if (Load.isIC()) {
			ListHead.push_back((IC_type)Load.Point);
			LineUk.push_back(nullptr);
		}
		else if (Load.Point == nullptr || Load.isInt() || Load.isAtrMk())
		{
			if (LineUk.back() == nullptr) break;
			ip* t;
			long int tt = Load.toInt();
			if (Load.Point == nullptr)
				t = AtrSearchIP(LineUk.back()->Load.Point, LineAtr);
			else
				t = AtrSearchIP(LineUk.back()->Load.Point, tt);
			//cout << Load.toInt() << endl;
			if (t == nullptr || !t->Load.isIC()) break;
			ListHead.push_back((IC_type)t->Load.Point);
			LineUk.push_back(nullptr);
		}
		break;
	case 256: // SubRoot Перейти к корневому уровную иератхии списка
		while (ListHead.size() > 1)
		{
			ListHead.pop_back();
			LineUk.pop_back();
		}
		break;

	case 290: // CopyAdrCorrectOriginalSet Установить адрес оригинальной ИК для коррекции адресов скопированного ОА-графа
		CopyAdrCorrectOriginal = Load;
		break;
	case 291: // CopyAdrCorrectCopylSet Установить адрес копированной ИК для коррекции адресов скопированного ОА-графа
		CopyAdrCorrectCopy = Load;
		break;
	case 292: // CopyAdrCorrectAdrSet Установить адрес для коррекции адресов скопированного ОА-графа
		AdrToCorrect = Load;
		break;
	case 293: // CopyAdrCorrectAdrOut Выдать скорретированный адрес скопированного ОА-графа
		Load.Write(CopyAdrCorrect(CopyAdrCorrectOriginal, CopyAdrCorrectCopy, AdrToCorrect, {}));
		break;
	case 294: // CopyAdrCorrectAdrOutMk Выдать МК со скорретированным адресом скопированного ОА-графа
		MkExec(Load, CopyAdrCorrect(CopyAdrCorrectOriginal, CopyAdrCorrectCopy, AdrToCorrect, {}));
		break;
	case 295: // LastCopyAdrCorrectAdrSet Устаовить адрес для коррекции скопированного ОА-графа для последней строки
		break;
	case 296: // LineСopyAdrCorrectSet Устаовить адрес для коррекции скопированного ОА-графа для текущей строки
		break;
	case 397: // LineСopyAdrCorrectClear Очистить переменную для копирования
		break;
	case 400: // LineOutMk Выдать МК с найденной линией
		if(LineUk.back()==nullptr)
			MkExec(Load, {0,nullptr});
		else
			MkExec(Load, LineUk.back()->Load);
		break;
	case 401: // LineOut Выдать найденную линиию
		if (LineUk.back() == nullptr)
			Load.Write((void*)nullptr);
		else
			Load.Write(LineUk.back()->Load.Point);
		break;
	case 415: // IpReceaverdOut Выдать найденную ИП
		if (Searcher.IPRezPoint != nullptr)
			Load.Write(Searcher.IPRezPoint);
		break;
	case 416: // IpReceaverdOutMk Выдать МК с найденной ИП
		if (Searcher.IPRezPoint != nullptr)
			MkExec(Load,{CIP, Searcher.IPRezPoint});
		break;
	case 417: // LoadReceavedOut Выдать нагрузку найденной ИП
		if(Searcher.IPRezPoint!=nullptr)
			Load.Write(Searcher.IPRezPoint->Load);
		break;
	case 418: // LoadReceavedOutMk Выдать МК с нагрузкой найденной ИП
		if (Searcher.IPRezPoint != nullptr)
			MkExec(Load, Searcher.IPRezPoint->Load);
		break;
	case 419: // AtrReceivedOut Выдать атрибут наденной ИП
		if (Searcher.IPTemplRezPoint != nullptr)
			Load.Write(Searcher.IPRezPoint->atr);
		break;
	case 420: // AtrReceivedOutMk Выдать МК с атрибутом найденной ИП
		if (Searcher.IPTemplRezPoint != nullptr)
			MkExec(Load, { Cint, &Searcher.IPRezPoint->atr });
		break;
	case 425: // LastIpPopMk
		if (ListHead.back() != nullptr && ListHead.back()->size() > 0)
		{
			ip t = ((IC_type)ListHead.back()->back().Load.Point)->back();
			((IC_type)ListHead.back()->back().Load.Point)->back().Load.Clear();
			((IC_type)ListHead.back()->back().Load.Point)->pop_back();
			MkExec(Load, { TIP, &t });
		}
		break;
	case 405: // IpOut Выдать найденную в источнике ИП
		if (Searcher.IPTemplRezPoint != nullptr)
			Load.Write(Searcher.IPTemplRezPoint);
		break;
	case 406: // IpOutMk Выдать МК с найденной в источнике ИП
		if(Searcher.IPTemplRezPoint!=nullptr)
			MkExec(Load, { CIP, Searcher.IPTemplRezPoint });
		break;
	case 407: // LoadOut Выдать нагрузку найденной в источнике ИП
		if(Searcher.IPTemplRezPoint!=nullptr)
			Load.Write(Searcher.IPTemplRezPoint->Load);
		break;
	case 408: // LoadOutMk Выдать МК с нагрузкой найденной в источнике ИП
		if (Searcher.IPTemplRezPoint != nullptr)
			MkExec(Load, Searcher.IPTemplRezPoint->Load);
		break;
	case 409: // AtrOut Выдать атрибут наденной в источнике ИП
		if (Searcher.IPTemplRezPoint != nullptr)
			Load.Write(Searcher.IPTemplRezPoint->atr);
		break;
	case 410: // AtrOutMk Выдать МК с атрибутом найденной в источнике ИП
		MkExec(Load, { Cint, &Searcher.IPTemplRezPoint->atr });
		break;
		// Программы по срезультатам сравнения номеров строк
	case 430: // IpLastOut Выдать последнюю ИП из найденной строки
		if (Searcher.IPTemplRezPoint != nullptr)
			Load.Write(&IC_type(LineUk.back()->Load.Point)->back());
		break;
	case 431: // IpLastOutMk Выдать МК с последней ИП из найденной строки
		if (Searcher.IPTemplRezPoint != nullptr && IC_type(LineUk.back()->Load.Point)->size() > 0)
			MkExec(Load, { CIP, &IC_type(LineUk.back()->Load.Point)->back()});
	case 432: // IpFurstrOut Выдать последнюю ИП из найденной строки
		if (Searcher.IPTemplRezPoint != nullptr && IC_type(LineUk.back()->Load.Point)->size() > 0)
			Load.Write(&IC_type(LineUk.back()->Load.Point)[0]);
		break;
	case 433: // IpFirstOutMk Выдать МК с последней ИП из найденной строки
		if (Searcher.IPTemplRezPoint != nullptr && IC_type(LineUk.back()->Load.Point)->size() > 1)
			MkExec(Load, { CIP, &(*IC_type(LineUk.back()->Load.Point))[0] });
		break;
	case 434: // IpSecondOut Выдать последнюю ИП из найденной строки
		if (Searcher.IPTemplRezPoint != nullptr && IC_type(LineUk.back()->Load.Point)->size()>1)
			Load.Write(&IC_type(LineUk.back()->Load.Point)[1]);
		break;
	case 435: // IpSecondOutMk Выдать МК с последней ИП из найденной строки
		if (Searcher.IPTemplRezPoint != nullptr && IC_type(LineUk.back()->Load.Point)->size() > 1)
			MkExec(Load, { CIP, &(*IC_type(LineUk.back()->Load.Point))[1] });
		break;
	case 449: // NotEqExec
		if (LineNum >= 0 && LineNumOld >= 0)
			if (LineNumOld != LineNum)
				if (Load.Point != 0)
					ProgExec(Load);
				else
					ProgExec(Prog);
		break;
	case 450: // EqExec
		if (LineNum >= 0 && LineNumOld >= 0)
			if (LineNumOld == LineNum)
				if (Load.Point != 0)
					ProgExec(Load);
				else
					ProgExec(Prog);
		break;
	case 451: // BiggerExec
		if (LineNum >= 0 && LineNumOld >= 0)
			if (LineNumOld < LineNum)
				if (Load.Point != 0)
					ProgExec(Load);
				else
					ProgExec(Prog);
		break;
	case 452: // SmallerExec
		if (LineNum >= 0 && LineNumOld >= 0)
			if (LineNumOld > LineNum)
				if (Load.Point != 0)
					ProgExec(Load);
				else
					ProgExec(Prog);
			break;
	case 453: // BigerEqExec
		if (LineNum >= 0 && LineNumOld >= 0)
			if (LineNumOld <= LineNum)
				if (Load.Point != 0)
					ProgExec(Load);
				else
					ProgExec(Prog);
		break;
	case 454: // SmallerEqExec
		if (LineNum >= 0 && LineNumOld >= 0)
			if (LineNumOld >= LineNum)
				if (Load.Point != 0)
					ProgExec(Load);
				else
					ProgExec(Prog);
		break;
	case 455: // DifferenceOut Выдать различие старой найденной линии и новой
		Load.Write(LineNum - LineNumOld);
		break;
	case 456: // DifferenceOutMk Выдать МК с различием старой найденной линии и новой
	{
		long int temp = LineNum - LineNumOld;
		MkExec(Load, { Cint,&temp });
		break; }
	case 459: // LineNumSet Установить номер найденной строки (нужно для организации операций сравнения)
		LineNum = Load.toInt();
		break;
	case 460: //Stack.back().LineCountOut Выдать количество найденных строк
		Load.Write(LineCount);
		break;
	case 461: //Stack.back().LineCountOutMk Выдать МК с количеством найденных строк
		MkExec(Load, { Cint, &LineCount });
		break;

	case 470: // LessProgSet
		LessProg = Load.Point;
		break;
	case 471: // BiggerProgSet
		BibberProg = Load.Point;
		break;
	case 472: // EQProgSet
		EQProg = Load.Point;
		break;
	case 473: // LessEQProgSet
		LessEQProg = Load.Point;
		break;
	case 474: // BiggerEQProgSet
		BibberEQProg = Load.Point;
		break;
	// Программы по анализу строк
	case 500: // LineEmptyExec Выполнить подпрограмму, если текущая строка пустая
		if (ListHead.back() != nullptr && LineUk.back() != nullptr && ((IC_type)LineUk.back()->Load.Point)->size()==0)
			ProgExec(Load);
		break;
	case 501: //LineFullExec  Выполнить подпрограмму, если текущая строка не пустая
		if (ListHead.back() != nullptr && LineUk.back() != nullptr && ((IC_type)LineUk.back()->Load.Point)->size() != 0)
			ProgExec(Load);
		break;
	case 505: // LastEmptyExec  Выполнить подпрограмму, если последняя строка пустая
		if (ListHead.back() != nullptr && ListHead.back()->size() != 0 && ((IC_type)ListHead.back()->back().Load.Point)->size() == 0)
			ProgExec(Load);
		break;
	case 506: //LastFullExec Выполнить подпрограмму, если последняя строка не пустая
		if (ListHead.back() != nullptr && ListHead.back()->size() != 0 && ((IC_type)ListHead.back()->back().Load.Point)->size() != 0)
			ProgExec(Load);
		break;
	case 600: // JsonSave Записать ОА-граф в формате JSON
		JsonConv.ToJson({TIC,ListHead.back()},Load.toStr());
		//JsonConv.FOutNameSet(Load.toStr());
		break;
	case 601: // JsonLoad Считать файл в формате JSON
		ListHead.back()=(IC_type)JsonConv.FromJSON(Load.toStr()).Point;
		//JsonConv.FInNameSet(Load.toStr());
		break;
	case 605: //MnemoCapsSet Установить ссылку на капсулу с мнемониками
		JsonConv.MnemoCapsSet(Load);
		break;
	case 606: // MnemoTableSet Установить ссылку на таблицу мнемоник
		JsonConv.MnemoTableSet(Load);
		break;
	case 607: //MillirangeSet Установить диапазон милликоманд для ФУ
		break;
	case 608: // CapsListRegister -- capture this CapsList accumulator
		sCapsList = this;
		break;
	case 609: // IndFileNameSet -- set the .ind output path
		JsonConv.IndFileName = Load.toStr();
		break;
	case 614: // IndVectWrite -- build the .ind index vector from the registered CapsList
		JsonConv.CapsListFu = (sCapsList != nullptr) ? sCapsList : this;
		JsonConv.buildIndexVector((void*)Bus);
		JsonConv.IndexVectWrite(Load.toStr());
		break;
	default:
		CommonMk(MK, Load, Sender);
		break;
	}
	if (ListHead.size() == 1 && PostfixProg!=nullptr)
		ProgExec(PostfixProg); // Выполнить финальную программу
}

FU* List::Copy() // Программа копирования ФУ
{
	return new List(Bus, this);
}

FU* List::TypeCopy() // Создать ФУ такого же типа (не копируя контекст
{
	return new List(Bus, nullptr);
}
