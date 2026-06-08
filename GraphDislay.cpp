#include "stdafx.h"
#include "GraphDisplay.h"

void GraphDisplay::ProgFU(long int MK, LoadPoint Load, FU* Sender)
{
	MK %= FUMkRange; // Оставить только свои МК
	switch (MK)
	{
	case 0: // Reset
		GraphUk = nullptr;
		break;
	case 1: // Set Установить указатель на ОА граф
		GraphUk = Load.Point;
		break;
	case 5: // ExtendSet Установить флаг разворачивания всех ветвей иерархческого списка
		Extend = Load.toBool();
		break;
	case 10: // MnemoListSet Установить ссылку на ФУ списка лексем для получения из него мнемоник
		if(Load.isNil()) // Обнуление указателя на ФУ список
		{
			if (LoadMnemoStr.MnemoList != nullptr && !LoadMnemoStr.MnemoListExt) delete (List*)LoadMnemoStr.MnemoList;
			LoadMnemoStr.MnemoList = nullptr;
		}
		else if (Load.isFU()) {
			if (LoadMnemoStr.MnemoList != nullptr && !LoadMnemoStr.MnemoListExt) delete (List*)LoadMnemoStr.MnemoList;
			LoadMnemoStr.MnemoList = (FU*)Load.Point;
		}
		else if (Load.isIC())
		{
			if (LoadMnemoStr.MnemoList == nullptr)
				LoadMnemoStr.MnemoList = new List(Bus, nullptr);
				LoadMnemoStr.MnemoList->ProgFU(1, Load, Sender); // Установка списка лексем в ФУ списка
		}
		break;
	default:
		CommonMk(MK, Load);
		break;
	}
}

FU* GraphDisplay::Copy() // Программа копирования ФУ
{
	return new GraphDisplay(Bus, this);
}

FU* GraphDisplay::TypeCopy() // Создать ФУ такого же типа (не копируя контекст
{
	return new GraphDisplay(Bus, nullptr);
}
