#include "StreamManager.h"
#include "FUIni.h"

using namespace std;

void StreamManager::ProgFU(long int MK, LoadPoint Load, FU* Sender)
{
	int MKinitial = MK;
	MK %= FUMkRange;
	switch (MK % FUMkRange)
	{
	case 0: //Reset
		for (auto& i : Field)
		{
			for (auto& j : i)
				free(j);
			i.clear();
		}
		Field.clear();
		DeviseCounter = 0;
		break;
	case 1: // FieldCreate Создавать поле АЛУ (на входе номер типа или указатель на шаблон ФУ)
	{
		Field.push_back({});
		for (long int i = 0; i < Counter; i++)
		{
			if (Load.isEmpty()) continue;
			if (Load.isInt())
				Field.back().push_back(MakeFU.MakeFu(Load.toInt() + FUTypeCorrect, Bus));
			else if (Load.isFU()) // Если указатель на ФУ
				Field.back().push_back(((FU*)Load.Point)->Copy());

			Field.back().back()->FUInd = i; // Индекс ФУ
			Field.back().back()->FUInd2 = Field.size() - 1; // Индекс группы
			Field.back().back()->FUMkRange = DevMkRange; // Индекс группы
			Field.back().back()->FUMkGlobalAdr= i * DevMkRange; // Индекс группы
		}
		break;
	}
	case 2: // GroupCreateTempl Создавать группу на основе эталона на входе идекс группы, по умолчанию по индексу GroupInd)
	{
		if (Load.isEmpty() && (IndGroup >= Field.size() || IndGroup < 0))
		{
			FU::ProgExec(DevNotExistErrProg);
			break;
		}
		long int t;
		if (Load.isEmpty())

			for (int i = 1; i <= Counter; i++)
			{
				Field.push_back({});
				if (Load.isInt())
				{
					Field.back().push_back(MakeFU.MakeFu(Load.toInt()+ FUTypeCorrect, Bus));
					break;
				}
				else if (Load.isFU()) // Если указатель на ФУ
				{
					Field.back().push_back(MakeFU.MakeFu(Load.Point));
					break;
				}
			}
	}
	break;
	case 3: // FieldClear Очистить поле АЛУ
		for (int i = 0; i < Field.size(); i++) // Убираются все ФУ, кроме первого
		{
			for (int j = 0; j < Field[i].size(); j++)
				free(Field[i][j]);
			Field[i].clear();
		}
		Field.clear();
		break;
	case 5: // IndGroupSet Установить индекс группы АЛУ
		IndGroup = Load.toInt();
		break;
	case 6:	//IndSet Установить индекс АЛУ в группе
		Ind = Load.toInt();
		break;
	case 4: //Ind2Set Установить второй индекс АЛУ в группе
		Ind2 = Load.toInt();
		break;
	case 7: // IndSwap Поменять индексы устройств местами
		swap(Ind, Ind2);
		//swap(Mk1, Mk2);
		break;
	case 8: // GroupIndSwap Поменять индексы групп местами
		swap(IndGroup, IndGroup2);
		break;
	case 9: //Ind2GroupSet Установить второй индекс группы АЛУ
		IndGroup2 = Load.toInt();
		break;
	case 10: // GoupCreate Создать группу АЛУ
		Field.push_back({});
		break;
	case 45: // DevCopy Скопировать ФУ в группу
	case 11: // DevCreate Создать АЛУ в группе (На входе шаблон ФУ)
		if (IndGroup >= Field.size() && IndGroup >= 0)
		{
			FU::ProgExec(DevNotExistErrProg);
			break;
		}
		if (Load.isEmpty()) break;
		for (int i = Counter; i < 0; i++)
			if (Load.isInt())
			{
				Field[IndGroup].push_back(MakeFU.MakeFu(Load.toInt() + FUTypeCorrect, Bus));
				break;
			}
			else if (Load.isFU()) // Если указатель на ФУ
			{
				if(MK==11)
					Field[IndGroup].push_back(MakeFU.MakeFu(Load.Point));
				break;
			}
		break;
	case 12: // DevCreateLast Создать АЛУ в последней группе
		if (Field.size() == 0) // Если список пустой, то добавить одну строку
		{
			Field.push_back({});
			IndGroup = 0;
		}
		if (Load.isEmpty()) break;
		for (int i = Counter; i < 0; i++)
			if (Load.isInt())
			{
				Field.back().push_back(MakeFU.MakeFu(Load.toInt() + FUTypeCorrect, Bus));
				break;
			}
			else if (Load.isFU()) // Если указатель на ФУ
			{
				Field.back().push_back(MakeFU.MakeFu(Load.Point));
				break;
			}
		break;
	case 13: // DevAdd Добавить устройство в текущую группу
		//if (!Load.isFU()) break;
		if (Field.size() == 0) // Если список пустой, то добавить одну строку
		{
			Field.push_back({});
			IndGroup = 0;
		}
		if (Load.isInt())
			Field[IndGroup].push_back(MakeFU.MakeFu(Load.toInt() + FUTypeCorrect, Bus));
		else if (!Load.isFU())
			Field[IndGroup].push_back((FU*)Load.Point);
		break;
	case 14: // DevLastAdd Добавить устойство в последнюю группу
		//if (!Load.isFU()) break;
		if (Field.size() == 0) // Если список пустой, то добавить одну строку
		{
			Field.push_back({});
			IndGroup = 0;
		}
		if (Load.isInt())
			Field.back().push_back(MakeFU.MakeFu(Load.toInt() + FUTypeCorrect, Bus));
		else if (!Load.isFU())
			Field.back().push_back((FU*)Load.Point);
		Field.back().back()->FUInd = Field.back().size() - 1;
		Field.back().back()->FUInd2 = Field.size() - 1;
		break;
	case 15: // GroupRefCreate Cоздать ссылки на ФУ для группы ФУ
	{
		for (auto& i : Field[IndGroup])
			switch (i->GetFuType())
			{
			case FUStreamFloatALU:
				for (int j = 0; j < ((StreamFloatALU*)i)->ReceiverMk.size(); j++)
				{
					if (((StreamFloatALU*)i)->ReceiverContexts[j] == Bus || ((StreamFloatALU*)i)->ReceiverContexts[j] == nullptr)
					{
						FU* t = nullptr;
						for (auto& k : Field[IndGroup])
							if (k->FUMkGlobalAdr <= 10);
					}
				}
				break;
			case FUStreamIntALU:
				break;
			}
	}
	break;
	case 16: // IndOut Выдать первый индекс
		Load.Write(Ind);
		break;
	case 17: // IndOutMk Выдать МК с первым индексом
		MkExec(Load, { Cint,&Ind });
		break;
	case 18: // Ind2Out Выдать второй индекс
		Load.Write(Ind2);
		break;
	case 19: // Ind2OutMk Выдать МК со вторым индексом
		MkExec(Load, { Cint,&Ind2 });
		break;
	case 151: // IndLastOut Выдать индекс последнего ФУ (при нулевой нагрзуке записывается в текущий индекс)
		if (!Field.size()) break;
		if (Load.isEmpty())
		{
			Ind = Field[IndGroup].size() - 1;
			break;
		}
		Load.Write(Field[IndGroup].size() - 1);
		break;
	case 152: // IndLastOutMk Выдать МК с индексом последнего ФУ (при нулевой нагрзуке записывается в текущий индекс)
	{
		if (!Field.size()) break;
		if (Load.isEmpty())
		{
			Ind = Field[IndGroup].size() - 1;
			break;
		}
		long t = Field[IndGroup].size() - 1;
		MkExec(Load, {Cint,&t});
		}
		break;
	case 20: // IndAdd Прибавить к индексу группы (в нагрузке значение, которое прибавляется к индексу
		Ind += Load.toInt();
		break;
	case 21: // IndGroupAdd   Прибавить к индексу группы (в нагрузке значение, которое прибавляется к индексу, 1 по умолчанию) 
		IndGroup += Load.toInt();
		break;
	case 22: // Ind2Add Прибавить ко второму индексу устройства (в нагрузке значение, которое прибавляется к индексу)
		Ind2 += Load.toInt();
		break;
	case 23: // Ind2GroupAdd   Прибавить ко второму индексу группы (в нагрузке значение, которое прибавляется к индексу, 1 по умолчанию) 
		IndGroup2 += Load.toInt();
		break;
	case 24: // IndSub Вычесть из индекса устойства
		Ind -= Load.toInt();
		break;
	case 25: // IndMul Умножить индекс устройства
		Ind *= Load.toInt();
		break;
	case 26: // IndMod Остаток от деления индекса устройства
		Ind %= Load.toInt();
		break;

	case 28: // MkSet Установить МК для первого индекса
		Mk1 = Load.toInt();
		break;
	case 29: // Mk2Set Установить МК для второго индекса
		Mk2 = Load.toInt();
		break;
	case 27: // MkLastSet Установить МК для последнего ФУ
		MkLast = Load.toInt();
		break;
	case 30: // ExecAll Выполнить программу для всех ФУ поля
		for (auto& i : Field)
			for (auto& j : i)
				j->ProgExec(Load);
		break;
	case 31: // ExecGroup Выполнить программу для всех ФУ группы
		if (IndGroup >= Field.size())
		{
			FU::ProgExec(DevNotExistErrProg);
			break;
		}
		for (auto& i : Field[IndGroup])
			i->ProgExec(Load);
		break;
	case 32: // ExecDev Выполнить программу для конкретного ФУ с индексом Ind
		if (IndGroup >= Field.size() || Ind >= Field[IndGroup].size())
		{
			FU::ProgExec(DevNotExistErrProg);
			break;
		}
		Field[IndGroup][Ind]->ProgExec(Load);
		break;
	case 43: // ExecLastDev Выполнить программу для последнего ФУ
		if (!Field.size() || !Field.back().size()) break;
		Field.back().back()->ProgExec(Load);
		break;
	case 33: // Exec2Group Выполнить программу для всех ФУ группы
		if (IndGroup >= Field.size())
		{
			FU::ProgExec(DevNotExistErrProg);
			break;
		}
		for (auto& i : Field[IndGroup2])
			i->ProgExec(Load);
		break;
	case 34: // Exec2Dev Выполнить программу для конкретного ФУ
		if (IndGroup >= Field.size() || Field[IndGroup].size() >= Ind)
		{
			FU::ProgExec(DevNotExistErrProg);
			break;
		}
		Field[IndGroup2][Ind]->ProgExec(Load);
		break;
	case 35: // MkExec Выполнить МК для ФУ поля
	{
		FU* t = nullptr;
		if (Load.toInt() == 36) // Если происходит выполнение МК для двух ФУ поля
		{
			t = Field[IndGroup][Ind]->Bus; // Запомнить указатель на шину для второго ФУ
			Field[IndGroup][Ind]->Bus = Field[IndGroup][Ind2];
			Field[IndGroup][Ind]->MkExec(Mk1, { CMk, &Mk2 });
			Field[IndGroup][Ind]->Bus = t; // Восстановить указатель на шину
			break;
		}
		Field[IndGroup][Ind]->MkExec(Mk1, Load);
	}
	break;
	case 42: // MkLastExec // Выполнить МК для последнего ФУ
		if (!Field.size() || !Field.back().size()) break;
		Field.back().back()->MkExec(MkLast, Load);
		break;
	case 36: // Mk2Exec Выполнить МК для ФУ поля по второму индексу
	{
		FU* t = nullptr;
		if (Load.toInt() == 35) // Если происходит выполнение МК для двух ФУ поля
		{
			t = Field[IndGroup][Ind2]->Bus; // Запомнить указатель на шину для второго ФУ
			Field[IndGroup][Ind2]->Bus = Field[IndGroup][Ind];
			Field[IndGroup2][Ind2]->MkExec(Mk2, { CMk, &Mk1 });
			Field[IndGroup][Ind2]->Bus = t; // Восстановить указатель на шину
			break;
		}
		Field[IndGroup2][Ind2]->MkExec(Mk2, Load);
		break;
	}
	case 37: // MkBackExec Выполнить МК для последнего ФУ последней группы поля
		Field.back().back()->MkExec(Mk1, Load);
		break;
	case 38: // Mk2BackExec Выполнить МК для ФУ поля по второму индексу
		Field.back().back()->MkExec(Mk2, Load);
		break;
	case 39: // MkAllExec Выполнить МК для всех ФУ поля
		for (auto& i : Field)
			for (auto& j : i)
				j->MkExec(Mk1, Load);
		break;
	case 40: // MkAllGroupExec Выполнить МК для всех ФУ текущей группы
		for (auto& j : Field[IndGroup])
			j->MkExec(Mk1, Load);
		break;
	case 41: // MkAllLastGroupExec Выполнить МК для всех ФУ последней группы
		for (auto& j : Field.back())
			j->MkExec(Mk1, Load);
		break;
	case 44: // DevMkRangeSet Установить диапазон МК для устройств поля
		DevMkRange = Load.toInt();
		break;

	case 50: //  DevCountOut Выдать количество АЛУ в поле
		Load.Write(DeviseCounter);
		break;
	case 51: //  DevCountOutMk Выдать МК с количеством АЛУ в поле
		MkExec(Load, { Cint,&DeviseCounter });
		break;
	case 52: //  DevGroupCountOut Выдать количество АЛУ в текущей группе
		if (IndGroup >= Field.size())
		{
			FU::ProgExec(DevNotExistErrProg);
			break;
		}
		Load.Write(Field[IndGroup].size());
		break;
	case 53: //  DevGroupCountOutMk Выдать количество АЛУ в текущей группе
	{
		if (IndGroup >= Field.size())
		{
			FU::ProgExec(DevNotExistErrProg);
			break;
		}
		long int t = Field[IndGroup].size();
		MkExec(Load, { Cint,&t });
		break;
	}
	case 55: //  GroupCountOut Выдать количество групп
		Load.Write(Field.size());
		break;
	case 56: //  GroupCountOutMk Выдать МК с количеством групп
	{
		long int t = Field.size();
		MkExec(Load, { Cint,&t });
		break;
	}
	case 57: //  GroupLastIndOutMk Выдать МК с последним индексом группы
	{
		long int t = Field.size()-1;
		MkExec(Load, { Cint,&t });
		break;
	}
	case 60: // CounterSet Установить сколько раз необходимо создавать устройства
		Counter = Load.toInt(1);
		break;
	case 61: //CounterAdd Прибавить к счетчику (по умолчанию 1)
		Counter += Load.toInt(1);
		break;
	case 62: // CounterMul Умножить считчик (по умолчанию 2)
		Counter *= Load.toInt(2);
		break;
	case 63: // CounterSub Вычесть из считчика (по умолчанию 1)
		Counter -= Load.toInt(1);
		break;
	case 64: // ExecCounterSet Установить счетчик итераций выполнения подпрограммы
		ExecCounter.push_back(Load.toInt(1));
		break;
	case 65: // ExecCounterAdd Прибавить к счетчику итераций
		ExecCounter.back() += Load.toInt();
		break;
	case 66: //ExecCounterSub Вычесть из счетчика итераций
		ExecCounter.back() -= Load.toInt();
		break;
	case 67: // ExecCounterMul Умножить счетчик итераций
		ExecCounter.back() *= Load.toInt();
		break;
	case 68: // ExecCounterDiv Целочисленно разделить счетчик итераций
		ExecCounter.back() /= Load.toInt();
		break;

	case 70: //  DevOut Выдать контекст текущего АЛУ
	case 71: //  DevOutMk Выдать контекст текущего АЛУ
	case 72: //  Dev2Out Выдать контекст текущего АЛУ
	case 73: //  Dev2OutMk Выдать контекст текущего АЛУ
		if (IndGroup >= Field.size() || Field[IndGroup].size() >= Ind)
		{
			FU::ProgExec(DevNotExistErrProg);
			break;
		}
		switch (MK % FUMkRange) {
		case 70:
			Load.Write((FU*)Field[IndGroup][Ind]);
			break;
		case 71:
			MkExec(Load, { CFU,Field[IndGroup][Ind] });
			break;
		case 72:
			Load.Write((FU*)Field[IndGroup2][Ind2]);
			break;
		case 73:
			MkExec(Load, { CFU,Field[IndGroup2][Ind2] });
			break;
		}
		break;
	case 80: //  DevLastOut Выдать контекст последнего созданного АЛУ
		if (!Field.size())
		{
			FU::ProgExec(DevNotExistErrProg);
			break;
		}
		Load.Write((FU*)&Field.back().back());
		break;
	case 81: //  DevLastOutMk Выдать МК с контекстом последнего созданного АЛУ
		if (!Field.size())
		{
			FU::ProgExec(DevNotExistErrProg);
			break;
		}
		MkExec(Load, { CFU,(FU*)&Field.back().back() });
		break;
	case 85: // DevCurrentSet Установить указатель на текущее ФУ
		DevCurrent = (FU*)Load.Point;
		break;
	case 86: // DevCurrentExec Выполнить программу для текущего ФУ
		DevCurrent->ProgExec(Load);
		break;
	case 87: // DevCurrentMkSet Установить МК для текущего ФУ
		DevCurrentMk = Load.toInt();
		break;
	case 88: // DevCurrentMkExec Выполнить МК для текущего ФУ
		DevCurrent->MkExec(DevCurrentMk,Load);
		break;
	case 89: // DevCurrentOutMk Выдать МК с текущим ФУ
		MkExec(Load, { CFU,DevCurrent });
		break;
	case 84: // DevCurrentOut Выдать текущее ФУ
		Load.Write(DevCurrent);
		break;
	case 150: // DevLastExec Выполнить программу для последнего ФУ
		if (!Field.size() || !Field.back().size()) break;
		Field.back().back()->ProgExec(Load);
		break;
	case 90: //  GroupLastIndOut Выдать индекс последней созданной группы АЛУ (-1, если поле пустое)
		Load.Write(Field.size() - 1);
		break;
	case 91: //  GroupLastIndOutMk Выдать МК с индексом последней созданной группы АЛУ (-1, если поле пустое)
	{
		long int t = Field.size() - 1;
		MkExec(Load, { CFU,&t });
	}
	break;
	case 100: //  DevLastIndOut Выдать индекс последнего созданного АЛУ (-1, если поле пустое)
		if (!Field.size() || !Field.back().size())
			Load.Write((long int)-1);
		else
			Load.Write(Field.back().size() - 1);
		break;
	case 101: //  DevLastIndOutMk Выдать МК с индексом последнего созданного АЛУ (-1, если поле пустое)
	{
		long int t = -1;
		if (Field.size() && Field.back().size())
			t = Field.back().size() - 1;
		MkExec(Load, { CFU,&t });
	}
	break;

	case 108: //RezBufVectOut Выдать вектор значений буфера результата всех ФУ поля
	case 109: //RezBufVectOutMk Выдать Мк с вектором значений буфера результата всех ФУ поля
	case 110: //RezVectOut Выдать вектор результатов всех ФУ поля
	case 111: //RezVectOutMk Выдать Мк с вектором результатов всех ФУ поля
	case 112: //ReadyVectOut Выдать вектор готовности результатов всех ФУ поля
	case 113: //ReadyVectOutMk  Выдать Мк с вектором готовности результатов всех ФУ поля
	{
		if (IndGroup >= Field.size() || IndGroup < 0)
		{
			FU::ProgExec(DevNotExistErrProg);
			break;
		}
		LoadVect_type t = new vector<LoadPoint>;
		for (auto& i : Field)
			for (auto& j : i)
				switch (j->FUtype)
				{
				case FUStreamFloatALU:
					switch (MK)
					{
					case 108:
					case 109: t->push_back({ Cdouble, &((StreamFloatALU*)j)->RezBuf }); break;
					case 111:
					case 110: t->push_back({ Cdouble, &((StreamFloatALU*)j)->Rez }); break;
					default:
						t->push_back({ Cint, &((StreamFloatALU*)j)->Ready });
						break;
					}
					break;
				case FUStreamIntALU:
					switch (MK) {
					case 108:
					case 109: t->push_back({ Cint, &((StreamIntALU*)j)->RezBuf }); break;
					case 111:
					case 110: t->push_back({ Cint, &((StreamIntALU*)j)->Rez }); break;
					default:
						t->push_back({ Cint, &((StreamIntALU*)j)->Ready });
						break;
					}
				}
		if (MK % 2 == 0)
			Load.Write(t);
		else
			MkExec(Load, { CLoadVect, t });
		break;
	}

	case 115: //RezGroupVectOut Выдать вектор результатов всех ФУ группы
	case 116: //RezGroupVectPutMk Выдать Мк с вектором результатов всех ФУ группы
	{
		if (IndGroup >= Field.size() || IndGroup < 0)
		{
			FU::ProgExec(DevNotExistErrProg);
			break;
		}
		LoadVect_type t = new vector<LoadPoint>;
		for (auto& j : Field[IndGroup])
			switch (j->FUtype)
			{
			case FUStreamFloatALU:
				t->push_back({ Cdouble, &((StreamFloatALU*)j)->Ready });
				break;
			case FUStreamIntALU:
				t->push_back({ Cdouble, &((StreamIntALU*)j)->Ready });
				break;
			}
		if (MK == 110)
			Load.Write(t);
		else
			MkExec(Load, { CLoadArray, &t });
		break;
	}
	case 117: //ReadyGroupVectOut Выдать вектор готовности результатов всех ФУ группы
	case 118: //ReadyGroupVectOutMk Выдать Мк с вектором готовности результатов всех ФУ группы
	{
		if (IndGroup >= Field.size() || IndGroup < 0)
		{
			FU::ProgExec(DevNotExistErrProg);
			break;
		}
		LoadVect_type t = new vector<LoadPoint>;
		for (auto& j : Field[IndGroup])
			switch (j->FUtype)
			{
			case FUStreamFloatALU:
				t->push_back({ Cdouble, &((StreamFloatALU*)j)->Ready });
				break;
			case FUStreamIntALU:
				t->push_back({ Cdouble, &((StreamIntALU*)j)->Ready });
				break;
			}
		if (MK == 110)
			Load.Write(t);
		else
			MkExec(Load, { CLoadArray, &t });
		break;
	}
	// Работа с переменным, засылаемыми в поле ФУ
	case 129: // VarClear Очитить список переменных для пересылки ФУ поля
		VarFuMap.clear();
		break;
	case 130: // VarNameAdd Добавить имя переменной
		VarFuMap.insert({ Load.toStr(), {} });
		AddedVarName = Load.toStr();
		break;
	case 131: // VarMkAdd Добавить МК имя переменной
		VarFuMap[AddedVarName].push_back({ Load.toInt(), nullptr });
		break;
	case 132: // VarFuAdd Добавить ФУ для переменной
		VarFuMap[AddedVarName].back().second=(FU*)Load.Point;
		break;
	case 140: //VarNameSet Установить имя переменной, которая будет затем переслана на ФУ поля
		VarName = Load.toStr();
		break;
	case 145: // ValByNameSend Переслать значение переменной по ее имени
		if (!VarFuMap.count(VarName))
			ProgExec(ValNotFaund);
		else
			for (auto& i : VarFuMap[VarName])
				MkExec(i.first, Load, i.second);
		break;
	default:
		CommonMk(MK, Load, Sender);
		break;
	}
}

void StreamManager::ProgExec(void* Uk, unsigned int CycleMode, FU* Bus, vector<ip>::iterator* Start) // Исполнение программы из ИК
{
	if (!ExecCounter.size())
		FU::ProgExec(Uk, CycleMode, Bus, Start);
	else
	{
		for (int i = 0; i < ExecCounter.back(); i++)
			FU::ProgExec(Uk, CycleMode, Bus, Start);
		ExecCounter.pop_back();
	}
}
void StreamManager::ProgExec(LoadPoint Uk, unsigned int CycleMode, FU* Bus, vector<ip>::iterator* Start) // Исполнение программы из ИК
{
	if (!ExecCounter.size())
		FU::ProgExec(Uk, CycleMode, Bus, Start);
	else
	{
		for (int i = 0; i < ExecCounter.back(); i++)
			FU::ProgExec(Uk, CycleMode, Bus, Start);
		ExecCounter.pop_back();
	}
}

StreamManager::~StreamManager() // Деструктор
{
	ProgFU(0, {0,nullptr},nullptr);
	Field.clear();
}

FU* StreamManager::Copy() // Программа копирования ФУ
{
	return new StreamManager(Bus, this);
}

FU* StreamManager::TypeCopy() // Создать ФУ такого же типа (не копируя контекст
{
	return new StreamManager(Bus, nullptr);
}