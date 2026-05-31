#include "stdafx.h"
#include "ALU.h"
#include "EnumMk.h"
#include <string>
#include <cmath>
#include <algorithm>
#include <vector>

bool		ALU::getSign() // 0 - положительное число, 1 - отрицательное
{
	switch (Stack.back().accumType >> 1)
	{
	case Dint:
	case Dchar:
	case Dfloat:
	case Ddouble:
		return Stack.back().accum < 0;
		break;
	default:
		return 0;
	}
}

void		ALU::EQ(LoadPoint Load)
{
	sub(Load);
	Stack.back().accum = Stack.back().accum == 0;
	Stack.back().accumType = (Tbool + +Stack.back().accumType % 2);
};
void		ALU::NotEQ(LoadPoint Load)
{
	sub(Load);
	Stack.back().accum = Stack.back().accum != 0;
	Stack.back().accumType = (Tbool + +Stack.back().accumType % 2);
};
void		ALU::Bigger(LoadPoint Load)
{
	sub(Load);
	Stack.back().accum = Stack.back().accum > 0;
	Stack.back().accumType = (Tbool + +Stack.back().accumType % 2);
};
void		ALU::BiggerEQ(LoadPoint Load)
{
	sub(Load);
	Stack.back().accum = Stack.back().accum >= 0;
	Stack.back().accumType = (Tbool + +Stack.back().accumType % 2);
};
void		ALU::Smaller(LoadPoint Load)
{
	sub(Load);
	Stack.back().accum = Stack.back().accum < 0;
	Stack.back().accumType = (Tbool + Stack.back().accumType % 2);
};


void		ALU::SmallerEQ(LoadPoint Load)
{
	sub(Load);
	Stack.back().accum = Stack.back().accum <= 0;
	Stack.back().accumType = (Tbool + +Stack.back().accumType % 2);
//	bool* t = new bool(getSign() || !getLogic());
//	set({ Cbool,t });
};

void		ALU::Remainder(LoadPoint Load)
{
	sub(Load);
	bool* t = new bool(getSign() || !getLogic());
	set({ Cbool,t });
};

void		ALU::Clear() // ������� ������������
{
	Stack.back().accum = 0;
	Stack.back().accumType = Cint;
}

void	ALU::error_msg(int error_code)
{
	switch (error_code)
	{
	case 1:
		cout << "Not registered operation!";
		break;
	case 2:
		cout << "Not allowed operation!";
		break;
	case 3:
		cout << "Divided by zero!";
		break;
	}
}

void ALU::ProgFU(long int MK, LoadPoint Load, FU* Sender)
{
	MK %= FUMkRange;
	if (MK == 1) MK = E_MK::SET; // Заменить МК установки аккумулятора
	ProgExec(PrefixProg); // Выполнить префиксную программу

	bool LoadDelFlag = false; // Флаг удаления нагрузки со временными данными
	if (MK == ProgExecMk || MK == CalcMk) // выполнение программы
	{
//		ExecCounter.push_back(ExecRepeat);
//		ExecRepeat = 1;
//		for (; ExecCounter.back() > 0; ExecCounter.back()--)
			ProgExec(Load);
//		ExecCounter.pop_back();

		accum = Stack.back().accum; // Записать в выходной аккумулятор
		accumType = Stack.back().accumType; // Записать в выходной аккумулятор
		accumStr = Stack.back().accumStr; // Записать в выходной аккумулятор
		Ind = Stack.back().Ind; // Запомнить индекс
		for(auto i: Stack.back().OutMkAdr)
			if(i.Adr.Point!=nullptr)
				i.Adr.Write(Stack.back().accum);
			else if (i.Mk >= 0)
			{
				double tt_double = 0; // Хранилище аккумулятора предыдущего уровня
				long int tt_int = 0;
				float tt_float = 0;
				bool tt_bool = 0;
				char tt_char = 0;
				string tt_str = "";
				if (LoadPoint::isVectInd(Stack.back().accumType)) {
					MkExec(i.Mk, { CLoadVectInd, Stack.back().accumVect, Stack.back().Ind }, Receiver); // Сделать преобразование типов попозже
				}
		        else if (LoadPoint::isVect(Stack.back().accumType)) {
					MkExec(i.Mk, { CLoadVect,Stack.back().accumVect }, Receiver); // Сделать преобразование типов попозже
				}
				else if (LoadPoint::isStr(Stack.back().accumType)) {
					string tt_str = Stack.back().accumStr;
					MkExec(i.Mk, { Stack.back().accumType | 1, &tt_str }, Receiver); // Сделать преобразование типов попозже
				}
				if (LoadPoint::isDouble(Stack.back().accumType)) {
					double tt_double = Stack.back().accum;
					MkExec(i.Mk, { Stack.back().accumType | 1, &tt_double }, Receiver); // Сделать преобразование типов попозже
				}
				else if (LoadPoint::isFloat(Stack.back().accumType)) {
					float tt_float = Stack.back().accum;
					MkExec(i.Mk, { Stack.back().accumType | 1, &tt_float }, Receiver); // Сделать преобразование типов попозже
				}
				else if (LoadPoint::isInt(Stack.back().accumType)) {
					long int tt_int = Stack.back().accum;
					MkExec(i.Mk, { Stack.back().accumType | 1, &tt_int }, Receiver); // Сделать преобразование типов попозже
				}
				else if (LoadPoint::isBool(Stack.back().accumType)) {
					bool tt_bool = Stack.back().accum;
					MkExec(i.Mk, { Stack.back().accumType | 1, &tt_bool }, Receiver); // Сделать преобразование типов попозже
				}
				else if (LoadPoint::isChar(Stack.back().accumType)) {
					char tt_float = Stack.back().accum;
					MkExec(i.Mk, { Stack.back().accumType | 1, &tt_char }, Receiver); // Сделать преобразование типов попозже
				}

			}
		Stack.back().OutMkAdr.clear(); // Очистить стек выходных МК и адресов
	}
	else
	{
		if (MK == E_MK::TERNAR_YES || MK == E_MK::TERNAR_NO) // Тернарная конструкция			if (LoadPoint::isDigitBool(Stack.back().accumType) &&
			if (!LoadPoint::isDigitBool(Stack.back().accumType) ||
				Stack.back().accum == 0 && MK == E_MK::TERNAR_YES || Stack.back().accum != 0 && MK == E_MK::TERNAR_NO)
				return; // Выход, если условие тернарной конструкции не совпадает
			else // Выполнение тернарной конструкции
			{
				ProgExec(Load);
				ProgStop += 1; // Остановить дальнейшее выполнение программы, чтобы за "зацепить" противоположное условие
				exit;
			}
		else
		if (MK >= 30 && MK < 200 && LoadPoint::isVect(Stack.back().accumType) && !LoadPoint::isVectInd(Stack.back().accumType)) // Векторные операции
		{
			VectOperation(MK, Load);
			return;
		}
		double tt_double=0; // Хранилище аккумулятора предыдущего уровня
		long tt_int = 0;
		float tt_float = 0;
		bool tt_bool = 0;
		char tt_char = 0;
		string tt_str = "";
		if (MK >= 26 && MK < 500 && Load.isProg()) // Арифметико-логическсое выражение со ссылкой в нагрузке
		{
			Stack.push_back({});
			ProgExec(Load);
			if (LoadPoint::isVect(Stack.back().accumType)){
				Load = { CLoadVect,Stack.back().accumVect };
			}
			else if(LoadPoint::isVectInd(Stack.back().accumType)){
				Load = { CLoadVect, Stack.back().accumVect, Stack.back().Ind};
			}
			else if (LoadPoint::isStr(Stack.back().accumType)){
				tt_str = Stack.back().accumStr;
				Load = { Stack.back().accumType | 1, &tt_str}; // Запомнить значение вычисленного аккумулятора
			}
			if (LoadPoint::isDouble(Stack.back().accumType)){
				tt_double = Stack.back().accum;
				Load = { Stack.back().accumType | 1, &tt_double }; // Запомнить значение вычисленного аккумулятора
			}
			else if (LoadPoint::isFloat(Stack.back().accumType)){
				tt_float = Stack.back().accum;
				Load = { Stack.back().accumType | 1, &tt_float }; // Запомнить значение вычисленного аккумулятора
			}
			else if (LoadPoint::isInt(Stack.back().accumType)){
				tt_int = Stack.back().accum;
					Load = { Stack.back().accumType | 1, &tt_int }; // Запомнить значение вычисленного аккумулятора
			}
			else if (Load.isBool(Stack.back().accumType))	{
				tt_bool = Stack.back().accum;
				Load = { Stack.back().accumType | 1, &tt_bool }; // Запомнить значение вычисленного аккумулятора
			}
			else if (LoadPoint::isChar(Stack.back().accumType))	{
				tt_float = Stack.back().accum;
				Load = { Stack.back().accumType | 1, &tt_char }; // Запомнить значение вычисленного аккумулятора
			}

			// Доделать очищение память для вектора
			Stack.pop_back(); // Удалить аккумулятор из стека
			if (Anses.back() >= Stack.size()) // Удаление ans если индекс меньше длины 
				Anses.pop_back();
		}

		switch (MK)
		{
		case E_MK::Remainder:

			break;
		case 500: // MkExtSet Установить внешнюю МК
			MKExt = Load.toInt();
			break;
		case 501: // MKExtExec Выполнить внешнюю МК
			MkExec(MKExt, Load, Receiver);
			break;
		case 502: // MkOutExtExec Выполнить внешнюю Мк для выдачи данных (на вход внешней команды автоматически дается Мк установки значения аккумулятора
			MkExec(Load, LoadNew(FUMkGlobalAdr + E_MK::SET), Receiver);
			break;
		case 505: // AnsFix Зафиксировать текущий уровень аккумулятора как ans
			if (Stack.size() - 1 == Anses.back()) break; // ans уже добавлен
			Anses.push_back(Stack.size() - 1);
			break;

		case 510: // VectStorePush Создать запасный вектор
			VectStore.push_back(new vector<LoadPoint>);
			if (Load.Point != nullptr && Load.isVect())
			{
				VectStore.back()->resize(((LoadVect_type)Load.Point)->size());
				copy(((LoadVect_type)Load.Point)->begin(), ((LoadVect_type)Load.Point)->end(), VectStore.back()->begin());
				for (auto& i : *VectStore.back()) // Дублирование констант в нагрузках
					i.Clone();
			}
			break;
		case 511: // VectStoreClear Очистить запасный вектор
			VectStore.back()->clear();
		case 512: // VectStoreAppend Добавить элемент к запасному вектору
			VectStore.back()->push_back(Load);
			break;

		case 513: // VectStoreExtend Добавить вектор из нагрузки в конец вектора из хранилища
		{
			int t = Load.toVect()->size();
			if (!Load.isVect() && VectStore.size()) break;
			VectStore.back()->resize(t + Load.toVect()->size());
			copy(Load.toVect()->begin(), Load.toVect()->end(), VectStore.back()->begin() + t);
			for (auto i = VectStore.back()->begin() + t; i != VectStore.back()->end(); i++)
				i->Clone(); // Клонирование нагрузок
			break;
		}
		case 515: // VectStoreOut Выдать вектор из хранилища
			if (!VectStore.size()) break;
			Load.Write(VectStore.back());
			break;
		case 516: // VectStoreOutMk Выдать МК с вектором из хранилища
			if (!VectStore.size()) break;
			MkExec(Load, { CLoadVect, VectStore.back() }, Receiver);
			break;
		case 517: // VectStorePop Вытолкнуть вектор из хранилища
			if (!VectStore.size()) break;
			Load.Write(VectStore.back());
			//			delete VectStore.back();
			VectStore.pop_back();
			break;
		case 518: // VectStorePopMk Выдать МК с вектором и вытолкнуть его из хранилища
			if (!VectStore.size()) break;
			MkExec(Load, { CLoadVect, VectStore.back() }, Receiver);
			//			delete VectStore.back();
			VectStore.pop_back();
			break;

		case 520: //ForNoBreak Вычислить предикат и завершить программу при True
		case 521: // ForYesBreak Вычислить предикат и завершить программу при False
		{
			if (!Load.isIC()) break;
			Stack.push_back(Stack.back());
			ProgExec(Load);
			bool t = Stack.back().accum;
			t = MK == 520 ? t : !t;
			Stack.pop_back();
			if (t) ProgStop += 2;
			break;
		}
		case 0: // Reset
			srand(time(NULL)); // Сброс генератора случайных чисел
			Stack.clear();
			Stack.push_back({});
			Stack.back().accumType = Cdouble;
			Stack.back().accum = 0;
			Anses.push_back(0); // Настроить Ans на персвый элемент стека АЛУ

			Accum = { Cdouble,&accum }; // Установить снандартный указатель на АЛУ и аккумулятор на самого себя
			Alu = this;
			ALUCreating = false;
			break;

//		case 530: // ExecCounterSet Установить счетчик итераций выполнения подпрограммы
//			ExecRepeat=Load.toInt(1);
//			break;
		case 531: // ExecCounterAdd Прибавить к счетчику итераций
			ExecRepeat+= Load.toInt();
			break;
		case 532: //ExecCounterSub Вычесть из счетчика итераций
			ExecRepeat -= Load.toInt();
			break;
		case 533: // ExecCounterMul Умножить счетчик итераций
			ExecRepeat *= Load.toInt();
			break;
		case 534: // ExecCounterDiv Целочисленно разделить счетчик итераций
			ExecRepeat /= Load.toInt();
			break;
		case 535: // ReceiverSet Установить примника результата
			Receiver = (FU*)Load.Point;
			break;

		case 2: // Out Выдать значение аккумулятора
		case 3: // OutMk Выдать МК со значением аккумулятора

		case 506: // AnsOut Выдать Ans
		case 507: // AnsOutMk Выдать МК с Ans
		{
			ALUContext* ta = (MK == 2 || MK == 3) ? &Stack.back() : &Stack[Anses.back()];
			if ((MK == 2 || MK == 3) && LoadPoint::isVectInd(Stack.back().accumType) || (MK == 506 || MK == 507) && LoadPoint::isVectInd(Stack[Anses.back()].accumType))
			{
				long int Ind = ta->Ind;
				if (Ind < 0) Ind = ta->accumVect->size() + Ind;
				if (!(Ind >= 0 && Ind < ta->accumVect->size())) {
					ProgExec(OutOfRangeErrProg); // Ошибка выхода индекса за пределы диапазона
					break;
				}
				else {
					switch (MK)
					{
					case 506:
					case 2: Load.WriteFromLoad(ta->accumVect->at(ta->Ind)); break;
					case 507:
					case 3: MkExec(Load, ta->accumVect->at(Ind), Receiver); break;
					}
					Stack.back().Ind += Stack.back().IndAutoInc;
					break;
				}
			}
			else
				switch (ta->accumType >> 1)
				{
				case DLoadVect:
					switch (MK) {
					case 506:
					case 2: Load.Write(ta->accum); break;
					case 507:
					case 3: MkExec(Load, { ta->accumType, ta->accumVect }, Receiver); break;
					}
					break;
				case Ddouble:
					switch (MK) {
					case 506:
					case 2:  Load.Write(ta->accum); break;
					case 507:
					case 3: MkExec(Load, { ta->accumType, &ta->accum }, Receiver); break;
					}
					break;
				case Dfloat:
				{
					float t = ta->accum;
					switch (MK) {
					case 506:
					case 2:  Load.Write(ta->accum); break;
					case 507:
					case 3: MkExec(Load, { ta->accumType, &t }, Receiver); break;
					}
					break;
				}
				case Dint:
				{
					long int t = ta->accum;
					switch (MK) {
					case 506:
					case 2:  Load.Write(t); break;
					case 507:
					case 3: MkExec(Load, { ta->accumType, &t }, Receiver); break;
					}
					break;
				}
				case Dchar:
				{
					char t = ta->accum;
					switch (MK) {
					case 506:
					case 2:  Load.Write(t); break;
					case 507:
					case 3: MkExec(Load, { ta->accumType, &t }, Receiver); break;
					}
					break;
				}
				case Dbool:
				{
					bool t = ta->accum;
					switch (MK) {
					case 506:
					case 2:  Load.Write(t); break;
					case 507:
					case 3: MkExec(Load, { Stack.back().accumType, &t }, Receiver); break;
					}
					break;
				}
				case Dstring:
					switch (MK) {
					case 506:
					case 2:  Load.Write(ta->accumStr); break;
					case 507:
					case 3: MkExec(Load, { ta->accumType, &ta->accumStr }, Receiver, true); break;
					}
					break;
				}
			break;
		}
		/*
				case 2: // Out Выдать значение аккумулятора
					if (accumType >> 1 == Dstring)
						Load.Write(Stack.back().accumStr);
					else if (Stack.back().accumType >> 1 == DLoadArray) // Вектор
					{
						if (Stack.back().IndF)
							if (!(Stack.back().Ind >= 0 && Stack.back().Ind < Stack.back().accumVect->size()))
							{
								ProgExec(OutOfRangeErrProg); // Ошибка выхода индекса за пределы диапазона
								break;
							}
							else
								Load.WriteFromLoad(Stack.back().accumVect->at(Stack.back().Ind));
						if (Load.Type == TLoadArray || Load.Type == Tvoid)
						{
							Load.Write(Stack.back().accumVect);
							Stack.back().Ind += Stack.back().IndAutoInc;
						}
						else
							;// Сообщение о несоответствии типов
					}
					else
						Load.Write(Stack.back().accum);
					break;
		*/
		case E_MK::PUSH: //  Push Сделать еще один уровень аккумулятора
			Stack.push_back({});
			ProgFU(E_MK::SET, Load);
			break;
		case 7: //PushExec Создать новый уровень аккумулятора и выполнить программу 
		case 8: // PushCycleExec Создать новый уровень аккумулятора и выполнить циклическую программу 
		case 9: // PushPostCycleExec Создать новый уровень аккумулятора и выполнить программу с постциклом
		{
			Stack.push_back(Stack.back());
			//			int t = 0;
			//			ProgFU(E_MK::SET, { Cint, &t });
			switch (MK) {
			case 7: ProgExec(Load); break;
			case 8: ProgExec(Load, 1); break;
			case 9: ProgExec(Load, 2); break;
			}
			// Добавить удаление вектора-аккумулятора
			Stack.pop_back();
			if (Anses.back() >= Stack.size()) // Удалить Ans-ы
				Anses.pop_back();
			break;
		}
		case 5: //AccumAdrOut Выдать адрес выходного аккумулятора
			Load.Write((void*)&Accum);
			break;
		case 6: //AccumAdrOutMk Выдать МК с адресом выходного аккумулятора
			MkExec(Load, { Cdouble,(void*)&Accum }, Receiver);
			break;
		case 10: // OutMkAdrClear Очистить буфер МК и адресов для выдачи результата
			Stack.back().OutMkAdr.clear();
			break;
		case 11: // OutMkAdd Добавить адрес для выдачи результата вычисления
			Stack.back().OutMkAdr.push_back({});
			Stack.back().OutMkAdr.back().Mk = Load.toInt();
			break;
		case 12: // OutAdrAdd Добавить адрес для выдачи результатов
			Stack.back().OutMkAdr.push_back({ -1, Load });
			break;
			// Операции с аккумулятором
		case 15: // Push Добавить новый аккумулятор (аналог Set, только сначала создается новый аккумулятор)
			Stack.push_back({});
			ProgFU(E_MK::SET, Load);
			break;
		case 16: // Pop Выдать значение аккумулятора и удалить аккмумулятор
		case 17: // PopMk Выдать МК со значением аккумулятора и удалить аккмумулятор
			if (MK == 16)
				if (Load.isDigit() && Load.isDigitBool(Stack.back().accumType))
					Load.Write(Stack.back().accum);
				else if (Load.isStrChar() && Load.isDigitBool(Stack.back().accumType))
					Load.Write(Stack.back().accumStr);
				else
					Load.Write(Stack.back().accumVect);
			else
			{
				if (Load.isDigit() && Load.isDigitBool(Stack.back().accumType))
					MkExec(Load, LoadPoint::TypeMinimizeOut(Stack.back().accum, Stack.back().accumType | 1), Receiver);
				else if (Load.isStrChar() && Load.isDigitBool(Stack.back().accumType))
					MkExec(Load, { Stack.back().accumType,&Stack.back().accumStr }, Receiver);
			}
			if (Stack.size() > 1)
				Stack.pop_back();
			else
				Stack.back() = {};
			if (Anses.back() >= Stack.size()) // Удаление ans если индекс меньше длины 
				Anses.pop_back();
			break;

		case 13:// CopyOut Выдать копию аккумулятора
		case 14:// CopyOutMK Выдать МК с копией аккумулятора
		case 18:// CopyConstOut Выдать копию аккумулятора и сделать значение константой
		case 19:// CopyConstOutMK Выдать МК с копией аккумулятора и сделать значение константой
		{
			LoadPoint t = { accumType,&Stack.back().accum };
			if (LoadPoint::isStrChar(accumType))
				t.Point = &Stack.back().accumStr;
			else
				if (LoadPoint::isVect(accumType))
					t.Point = &Stack.back().accumVect;
			t.Type = t.Type | 1; // Установить константу для того, чтобы клонировать
			t = t.Clone();
			if (MK == 13 || MK == 14)
				t.Type -= 1; // Установить тип переменной
			if (MK == 13 || MK == 18)
				Load.WriteFromLoad(t);
			else
				MkExec(Load, t, Receiver);
		}
		break;
		case 20: // VectToIndSet Установить ссылку на вектор, при этом прежнее значение аккумулятора становится индексом вектора
			if (!Load.isVect() || !LoadPoint::isDigit(Stack.back().accumType)) break;
			Stack.back().accumVect = (LoadVect_type)Load.Point;
			Stack.back().accumType = TLoadVectInd + (Load.Type & 1);
			Stack.back().Ind = Stack.back().accum;
			break;
			// Программы обработки ошибок
		case 600: // ErrProgSet Программа обработки ошибки любого типа
			ErrProg = Load.Point;
			break;
		case 601: // VectorErrProgSet Программа обработки ошибки векторной операции
			VectErrProg = Load.Point;
			break;
		case 602: // IndexOutOfRangeProgSet Программа обработки ошибки выхода за допустимые пределы индекса
			OutOfRangeErrProg = Load.Point;
			break;
		case 603: // TypeMismatchErrProgSet Установить программу отработки ошибки несоотвествия типов
			TypeMismatchErrProg = Load.Point;
			break;
		case 604: // DivByZeroErrProgSet Установить программу отработки ошибки деление на нуль
			DivByZeroErrProg = Load.Point;
			break;
		case 610: // VectIndOkExec Выполнить при удачной тестировании диапазона индекса вектора
		case 611: // VectIndErrExec Выполнить при неудачной тестировании диапазона индекса вектора
		{
			if (!Load.isProg()) break;
			if (!LoadPoint::isVectInd(Stack.back().accumType)) break;
			register int N = Stack.back().accumVect->size();
			if (N <= Stack.back().Ind || N < -Stack.back().Ind || Stack.back().accumVect->at(N * (Stack.back().Ind < 0) + Stack.back().Ind).Point == nullptr) // Ошибка индекса
			{
				if (MK == 611)	ProgExec(Load);
			}
			else if (MK == 610)
				ProgExec(Load);
			break;
		}

		case E_MK::RESET_A: // Reset Сброс аккумулятора
			Stack.back().accumType = Cdouble;
			Stack.back().accum = 0;
			break;
		case E_MK::SET: // Установить значение в аккумулятор
			//	case E_MK::SET: // Set
			set(Load);
			break;
		case E_MK::INC: // inc
			inc(Load);
			//set(Load);
			break;
		case E_MK::DEC: // dec
			dec(Load);
			//set(Load);
			break;
		case E_MK::SUM:
			add(Load);
			break;
		case E_MK::SUB:
			sub(Load);
			break;
		case E_MK::MULT:
			mult(Load);
			break;
		case E_MK::DIV:
			div(Load);
			break;
		case E_MK::MIN:
			fu_min(Load);
			break;
		case E_MK::MAX:
			fu_max(Load);
			break;
		case E_MK::COS:
			fu_cos(Load);
			break;
		case E_MK::SIN:
			fu_sin(Load);
			break;
		case E_MK::TAN:
			fu_tan(Load);
			break;
		case E_MK::ACOS:
			fu_acos(Load);
			break;
		case E_MK::ASIN:
			fu_asin(Load);
			break;
		case E_MK::ATAN:
			fu_atan(Load);
			break;
		case E_MK::MOD:
			fu_mod(Load);
			break;
		case E_MK::SQRT:
			fu_sqrt(Load);
			break;
		case E_MK::POW:
			fu_pow(Load);
			break;
		case E_MK::ABS:
			fu_abs(Load);
			break;
		case E_MK::CEIL:
			fu_ceil(Load);
			break;
		case E_MK::FLOOR:
			fu_floor(Load);
			break;
		case E_MK::ROUND:
			fu_round(Load);
			break;
		case E_MK::LOG:
			fu_log(Load);
			break;
		case E_MK::LOG10:
			fu_log10(Load);
			break;
		case E_MK::LOG2:
			fu_log2(Load);
			break;
		case E_MK::EXP:
			fu_exp(Load);
			break;
		case E_MK::RANDOM:
			fu_random(Load);
			break;
		case E_MK::AND:
			fu_and(Load);
			break;
		case E_MK::OR:
			fu_or(Load);
			break;
		case E_MK::INV:
			fu_inverse(Load);
			break;
		case E_MK::DIV_INT:
			div_int(Load);
			break;
		case E_MK::XOR:
			fu_xor(Load);
			break;
		case E_MK::INV_BIT:
			fu_inv_bit(Load);
			break;
		case E_MK::OR_BIT:
			fu_or_bit(Load);
			break;
		case E_MK::AND_BIT:
			fu_and_bit(Load);
			break;
		case E_MK::MR_BIT:
			fu_mr_bit(Load);
			break;
		case E_MK::ML_BIT:
			fu_ml_bit(Load);
			break;
		case E_MK::EQ:
			EQ(Load);
			break;
		case E_MK::NotEQ:
			NotEQ(Load);
			break;
		case E_MK::Bigger:
			Bigger(Load);
			break;
		case E_MK::BiggerEQ:
			BiggerEQ(Load);
			break;
		case E_MK::Smaller:
			Smaller(Load);
			break;
		case E_MK::SmallerEQ:
			SmallerEQ(Load);
			break;
		case E_MK::XOR_BIT:
			XOR_BIT(Load);
			break;
		case E_MK::Compar3Way:
			Compar3Way(Load);
			break;

		case E_MK::SUM_VECT:
			if (!Load.isVect())
			{
				ProgExec(TypeMismatchErrProg); ProgExec(ErrProg);
			}
			else
			{
				double s = 0;
				unsigned int Type = 0;
				for (auto& i : *((LoadVect_type)Load.Point)) {
					if (!i.isDigitBool()) {
						ProgExec(TypeMismatchErrProg);	ProgExec(ErrProg);
						break;
					}
					s += i.toDouble();
					Type = max(i.Type, Type);
				}
				Stack.back().accum = s;
				Stack.back().accumType = Type;
			}
			break;
		case E_MK::MAX_VECT:
			if (!Load.isVect())
			{
				ProgExec(TypeMismatchErrProg); ProgExec(ErrProg);
			}
			else
			{
				double Max = 0;
				unsigned int Type = 0;
				for (auto& i : *((LoadVect_type)Load.Point)) {
					if (!i.isDigitBool()) {
						ProgExec(TypeMismatchErrProg);	ProgExec(ErrProg);
						break;
					}
					Max = max(Max, i.toDouble());
					Type = max(i.Type, Type);
				}
				Stack.back().accum = Max;
				Stack.back().accumType = Type;
				Stack.back().Ind = -1;
			}
			break;
		case E_MK::MIN_VECT:
			if (!Load.isVect())
			{
				ProgExec(TypeMismatchErrProg); ProgExec(ErrProg);
			}
			else
			{
				double Min = 0;
				unsigned int Type = 0;
				for (auto& i : *((LoadVect_type)Load.Point)) {
					if (!i.isDigitBool()) {
						ProgExec(TypeMismatchErrProg);	ProgExec(ErrProg);
						break;
					}
					Min = min(Min, i.toDouble());
					Type = max(i.Type, Type);
				}
				Stack.back().accum = Min;
				Stack.back().accumType = Type;
			}
			break;
		case 198: // SumVect Поэлементное векторное +
		case 199: // MulVect Поэлементное векторное *
		{
			if (!Load.isVect(Stack.back().accumType)) break;
			vector<LoadPoint> Rez;
			if (Load.isDigitBool()) // Одна операция ко всем элементам вектора
			{
				Stack.push_back({}); // Создать дополнительные аккумулятор
				LoadVect_type v1 = (Stack.end() - 2)->accumVect; // Запомнить ссылку на исходный вектор
				for (auto& i : *v1)
				{
					if (!i.isDigitBool())
					{
						ProgExec(ErrProg); // Программа отработки ошибки
						ProgExec(VectErrProg); // Программа отработки ошибки векторных операций
						Stack.pop_back();
						if (Anses.back() >= Stack.size()) // Удаление ans если индекс меньше длины 
							Anses.pop_back();
						break;
					}
					ProgFU(E_MK::SET, i);
					ProgFU(MK - 300, Load);
					Rez.push_back({ Stack.back().accumType,MakeLoadFromDouble(Stack.back().accum,Stack.back().accumType | 1) }); // Запись константы

				}
				Stack.pop_back(); // Удалить дополнительный аккумулятор
				if (Anses.back() >= Stack.size()) // Удаление ans если индекс меньше длины 
					Anses.pop_back();
				*Stack.back().accumVect = Rez;
				break;
			}

			if (Load.isVect() && LoadPoint::isInt(Stack.back().accumType) && Load.toInt() >= 0) // Мультиплексия вектора
			{
				long int N = Load.toInt();
				for (int i = 0; i < N; i++)
					for (auto& j : *Stack.back().accumVect)
						Rez.push_back(j.Clone());
				*Stack.back().accumVect = Rez;
				break;
			}

			// if (Load.isVect() && Stack.back().accumType >> 1 == Dint && Load.toInt() >= 0)
			if (Load.isVect() && LoadPoint::isDoubleInt(Stack.back().accumType) && Load.toInt() >= 0)
				//if (!Load.isVect() || !(Load.isVect() && Stack.back().accumType >> 1 == DLoadVect))
				if (!Load.isVect() || !(Load.isVect() && LoadPoint::isVect(Stack.back().accumType)))
				{
					ProgExec(ErrProg); // Программа отработки ошибки
					ProgExec(VectErrProg); // Программа отработки ошибки векторных операций
					break;
				}
			Stack.push_back({}); // Создать дополнительные аккумулятор
			LoadVect_type v1 = (Stack.end() - 2)->accumVect;

			for (auto i = v1->begin(), j = ((LoadVect_type)Load.Point)->begin(); // выполнение векторных операций
				i != v1->end() && j != ((LoadVect_type)Load.Point)->end(); i++, j++)
			{
				ProgFU(E_MK::SET, *i);
				ProgFU(MK - 300, *j);
				Rez.push_back({ Stack.back().accumType,MakeLoadFromDouble(Stack.back().accum,Stack.back().accumType | 1) }); // Запись константы
			}

			Stack.back().accumVect->clear();
			delete Stack.back().accumVect;
			Stack.pop_back(); // Удалить дополнительный аккумулятор
			*Stack.back().accumVect = Rez;
			if (Anses.back() >= Stack.size()) // Удаление ans если индекс меньше длины 
				Anses.pop_back();
			break;
		}

		// Векторные операции		
		case 269: //IndAutoIncSet Установить величину автоинкрементации индекса вектора
			Stack.back().IndAutoInc = Load.toInt();
			break;
		case 270: //IndSet Установить индекс вектора (nil в нагрузке вызывает сброс индекса, т.е. по команде Out выдается вектор, а не элемент)
			if (Load.Point == nullptr)
			{
				Stack.back().IndF = false;
				Stack.back().Ind = 0;
				break;
			}
			if (!Load.isDigitBool())
			{
				ProgExec(ErrProg); // Программа отработки ошибки
				ProgExec(VectErrProg); // Программа отработки ошибки векторных операций
				break;
			}
			Stack.back().IndF = true;
			Stack.back().Ind = Load.toInt();
			Stack.back().accumType = CLoadVectInd;
			break;
		case 271: //IndInc Инремент индекса вектора
			if (Stack.back().IndF)
				Stack.back().Ind++;
			break;
		case 272: //IndDec Декремент индекса вектора
			if (Stack.back().IndF)
				Stack.back().Ind--;
			break;
		case 273: // IndAdd Прибавить смещение к индексу вектора
			if (Stack.back().IndF)
				Stack.back().Ind += Load.toInt();
			break;
		case 274: // VectValByIndSet Установить значение аккумулятора по индексу в нагрузке
		{
			long int i = Load.toInt();
			if (Stack.size() < 1 || 1 >= Stack.back().accumVect->size() || i < 0)
			{
				ProgExec(ErrProg);
				ProgExec(OutOfRangeErrProg);
				ProgStopAll = true;
				break;
			}
			ProgFU(E_MK::SET, Stack.back().accumVect->at(i));
		}
		case 275: // ConfineAppend Добавить элемент в предпредпредыдущий аккумулятор из векторa предыдущего аккумулятора по индексу из текущего аккумулятора (для конфайна)
		{
			long int i = Load.toInt();
			if (Stack.size() < 3 || !LoadPoint::isVect((Stack.end() - 3)->accumType))
			{
				ProgExec(ErrProg);
				ProgExec(VectErrProg);
				ProgStopAll = true;
				break;
			}
			if (i < 0 || i >= (Stack.end() - 3)->accumVect->size())
			{
				ProgExec(ErrProg);
				ProgExec(OutOfRangeErrProg);
				ProgStopAll = true;
				break;
			}
			(Stack.end() - 3)->accumVect->push_back((Stack.end() - 2)->accumVect->at((long int)Stack.back().accum).Clone());
		}
		case 276: // PrevIndSet Установить индекс у предыдущего аккумулятора
			if (!Load.isDigitBool() || Stack.size() < 2)
			{
				ProgExec(ErrProg);
				break;
			}
			if (LoadPoint::isVect(Stack.back().accumType))
			{
				ProgExec(ErrProg);
				ProgExec(VectErrProg);
				break;
			}
			(Stack.end() - 2)->Ind = Load.toInt();
			break;
		case 277: // VectValSet Установить в аккумулятор значение вектора из нагрузки по индексу в аккумуляторе
		case 278: // VectValPush Положить значение вектора из пред. аккумулятора по индексу текущего аккумулятора (если Load=nil, то вектор из предыдущего аккумулятора
			if (!Load.isVect() || !LoadPoint::isDigitBool(Stack.back().accumType))
			{
				ProgExec(ErrProg); // Программа отработки ошибки
				ProgExec(VectErrProg); // Программа отработки ошибки векторных операций
				break;
			}
			if (MK == 277)
				ProgFU(E_MK::SET, { Load.Type,Load.LoadVect() }); // Установить значение из элемента вектора
			else
				ProgFU(E_MK::PUSH, (Stack.end() - 2)->accumVect->at(Load.toInt())); // Добавить значение из элемента вектора
			break;
		case 280: // VectNew Создать новый вектор (в нагрузке может быть первый элемент
			emptyvect(Load.toInt());
			break;
		case 295: // VectSizeOut Выдать длину массива
			Load.Write((long)Stack.back().accumVect->size());
			break;
		case 296: // VectSizeOutMk Выдать МК с длиной массива
			MkExec(Load, (long)Stack.back().accumVect->size());
			break;
		case 290: // Append Добавить элемент в вектор
			if (Load.Point != nullptr)
				append(Load);
			else {

				append({ Cdouble, &Stack.back().accum });
				cout << Stack.back().accum << endl;
			}
			break;
		case 281: // VectDel Удалить вектор
			Stack.back().accumVect->resize(0); // Добавить удаление каждого элемента вектора!!!
 			delete Stack.back().accumVect;
			Stack.back().accumVect = nullptr;
			Stack.back().IndF = false;
			break;
		case 282: // VectPop Отделить вектор от аккумулятора и выдать адрес вектора
			if(!LoadPoint::isVect(accumType)) break;
			{LoadPoint t = { TLoadVect, Stack.back().accumVect,-1};
			Load.Write(t);
			}
			Stack.back().accumVect = nullptr;
			Stack.back().IndF = false;
			break;
		case 283: // VectPopMk Отделить вектор от аккумулятора и выдать МК с адресом вектора
			if (!LoadPoint::isVect(accumType)) break;
			{LoadPoint t = { TLoadVect, Stack.back().accumVect,-1 };
			MkExec(Load, t, Receiver); }
			Stack.back().accumVect = nullptr;
			Stack.back().IndF = false;
			break;
		case 285: // VectClear Очистить вектор
			//if (Stack.back().accumType >> 1 != DLoadVect)
			if (!LoadPoint::isVect(Stack.back().accumType))
			{
				ProgExec(ErrProg); // Программа отработки ошибки
				ProgExec(VectErrProg); // Программа отработки ошибки векторных операций
				break;
			}
			if (Stack.back().accumVect == nullptr)
				Stack.back().accumVect = new vector<LoadPoint>;
			Stack.back().accumVect->clear();
			break;

			// Реализация поэлементных векторных операций
			// Двуоперандные векторные операции

		case 400: // ZeroExec Выполнить программу, если в аккумуляторе 0
			if (Stack.back().accum == 0)
				ProgExec(Load);
			break;
		case 401: // NotZeroExec Выполнить программу, если в аккумуляторе не 0
			if (Stack.back().accum != 0)
				ProgExec(Load);
			break;
		case 402: // BiggerExec Выполнить программу, если аккумулятор > 0
			if (Stack.back().accum > 0)
				ProgExec(Load);
			break;
		case 403: // BiggerZeroExec Выполнить программу, если аккумулятор >= 0
			if (Stack.back().accum >= 0)
				ProgExec(Load);
			break;
		case 404: // SmallerExec Выполнить программу, если аккумулятор < 0
			if (Stack.back().accum < 0)
				ProgExec(Load);
			break;
		case 405: // SmallerZeroExec Выполнить программу, если аккумулятор <= 0
			if (Stack.back().accum <= 0)
				ProgExec(Load);
			break;
		default:
			CommonMk(MK, Load);
			break;
		}
		if (LoadDelFlag) // Удаление врЕменных данных
			Load.VarDel();
		// Переписывание результатов в буфер для считывания другими ФУ
		accum = Stack.back().accum;
		accumStr = Stack.back().accumStr;
		accumVect = &Stack.back().accumVect;
		accumType = Stack.back().accumType;
	}
	ProgExec(PostfixProg); // Выполнить постфиксную программу
}

void ALU::VectOperation(long int MK, LoadPoint Load) // Реализацая векторных операций
{
	if (MK == E_MK::SUM && Load.isVect()) // Конкатенация векторов
	{
		concat(Load);
		return;
	}

	if (MK == E_MK::RANDOM && !Load.isVect()) // Конкатенация векторов
	{
		fu_random(Load);
		return;
	}

	if (MK == E_MK::MULT) // Конкатенация векторов
	{
		vecmult(Load);
		return;
	}
	if (MK == 198 || MK == 199)
	{
		if (!Load.isVect())
		{
			ProgExec(ErrProg);
			ProgExec(VectErrProg);
			ProgExec(TypeMismatchErrProg);
			return;
		}
		if (MK == 198)// Векторное поэлементное сложение
			MK = E_MK::SUM;
		if (MK == 199)// Векторное мультиплицирование
			MK = E_MK::MULT;
	}

	if (Load.isDigitBool()) // Одна операция к каждому элементу вектора
	{
		Stack.push_back({});
		for (auto& i : *(Stack.end()-2)->accumVect)
		{
			ProgFU(E_MK::SET, i);
			ProgFU(MK, Load);
			i.Write(Stack.back().accum);
		}
		Stack.pop_back();
		if (Anses.back() >= Stack.size()) // Удаление ans если индекс меньше длины 
			Anses.pop_back();
	}
	else // Векторная операция
	{
		vector<LoadPoint>* t = Stack.back().accumVect;
		Stack.push_back({});
		auto j = ((vector<LoadPoint>*)Load.Point)->begin();
		auto i = (Stack.end() - 2)->accumVect->begin();
		for (; i != (Stack.end() - 2)->accumVect->end() && j != ((vector<LoadPoint>*)Load.Point)->end(); i++, j++)
		{
			ProgFU(E_MK::SET, *i);
			ProgFU(MK,*j);
			i->Write(Stack.back().accum);
		}
		Stack.pop_back();
		if (Anses.back() >= Stack.size()) // Удаление ans если индекс меньше длины 
			Anses.pop_back();
	}
}

void	ALU::calc(int MK, LoadPoint Load)
{
	switch (MK)
	{
	default:
		error_msg(1);
		break;
	}
}

void ALU::XOR_BIT(LoadPoint Load)
{



	if (Load.Point == nullptr) {
		//	Запуск подпрограммы ошибки "Нет операнда"
		return;
	}
	else if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{

		Stack.back().accum = static_cast<long int>(Stack.back().accum) ^ Load.toInt();
		if (LoadPoint::isDigitBool(Stack.back().accumType) && LoadPoint::isDigitBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}

	else {
		return;
	}
}

void ALU::inc(LoadPoint Load)
{
	Stack.back().accum += Load.toDouble(1);
}

void ALU::dec(LoadPoint Load) // Декреминт если нагрузки нет, то декремируется аккумулятор
{
	Stack.back().accum -= Load.toDouble(1);
}
void ALU::emptyvect(long size) { // Size - количество элементов
	Stack.back().accumType = TLoadVect;
	Stack.back().accumVect = new vector<LoadPoint>(size);
	for (auto ui = Stack.back().accumVect->begin(); ui != Stack.back().accumVect->end(); ui++)
	{
		ui->Point = new double(0);
		ui->Type = Tdouble;
	}

	Stack.back().IndF = false;
}
void ALU::append(LoadPoint Load) //добавление
{
	Stack.back().accumVect->push_back(Load.Clone());
}
void ALU::concat(LoadPoint Load) // Конкатенация векторов
{
	vector<LoadPoint> t; // Создадим временный вектор для результата
	for (auto& i : *Stack.back().accumVect)
		t.push_back(i.Clone());
	for (auto& i : *(vector<LoadPoint>*)Load.Point)
		t.push_back(i.Clone());
	*Stack.back().accumVect = t;
}

void ALU::vecmult(LoadPoint Load) { // Размножение вектора
	int N = Stack.back().accumVect->size();
	for(int k=Load.toInt()-1; k; k--)
		for (int i = 0; i < N; i++)
			Stack.back().accumVect->push_back(Stack.back().accumVect->at(i).Clone());
}

void	ALU::add(LoadPoint Load) // Сложение
{
	if (Load.Point == nullptr) {
		return;
	}
	if (Load.Type >> 1 == DLoadVect) {
		if (LoadPoint::isVect(Load.Type)) // Если второй операнд не вектор, то ошибка
		{
			ProgExec(ErrProg); // Запуск программы ошибки 
			ProgExec(VectErrProg); // Запуск программы ошибки векторной операции
			return;
		}
		Stack.back().accumType = TLoadVect; // Сделать результат вектором-переменной
		concat(Load); // Конкатенация
		return;
	}
	else if (LoadPoint::isStr(Load.Type))
	{
		if (LoadPoint::isStr(Stack.back().accumType))
			Stack.back().accumStr += Load.toStr();
		else if (LoadPoint::isVect(Stack.back().accumType)) {
			for (int i = 0; i < Stack.back().accumVect->size(); i++) {
				if (LoadPoint::isStr(Stack.back().accumVect->at(i).Type))
				{
					Stack.back().accumVect->at(i).Write(Stack.back().accumVect->at(i).toStr() + Load.toStr());
				}
			}
		}
	}

	else if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
			Stack.back().accum += Load.toDouble();
			if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
				Stack.back().accumType = Cint;
			else
				Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType)) // Мультисложение вектора и числа
	{
		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(Stack.back().accumVect->at(i).toDouble() + Load.toDouble());
			}
		}
	}
}

void	ALU::sub(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		return;
	}

	if (LoadPoint::isVect(Load.Type)) {
		int i = 0;
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(Stack.back().accumVect->at(i).toDouble() - (((vector<LoadPoint>*)Load.Point)->begin() + i)->toDouble());
				}

			}
		}
		else
			return;
	}

	else if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum -= Load.toDouble();
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType))
	{

		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(Stack.back().accumVect->at(i).toDouble() - Load.toDouble());
			}
		}
	}
	else {
		return;
	}
}

void	ALU::mult(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		return;
	}
	if (LoadPoint::isVect(Load.Type)) {
		int i = 0;
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if ((((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					if (Stack.back().accumVect->at(i).isDigit()) {
						Stack.back().accumVect->at(i).Write(Stack.back().accumVect->at(i).toDouble() * (((vector<LoadPoint>*)Load.Point)->begin() + i)->toDouble());
					}
					else if (LoadPoint::isStr(Stack.back().accumVect->at(i).Type)) {
						string l;
						int k = (((vector<LoadPoint>*)Load.Point)->begin() + i)->toInt();
						for (int j = 0; j < k; j++)
							l += (((vector<LoadPoint>*)Load.Point)->begin() + i)->toStr();
					}
				}
			}
		}
		else
			return;
	}
	else if (LoadPoint::isStr(Stack.back().accumType)) {
		string k;
		int l;
		l = Load.toInt();
		for (int i = 0; i < l; i++)
			k += Stack.back().accumStr;
		Stack.back().accumStr = k;
	}
	else if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum *= Load.toDouble();
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType))
	{

		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(Stack.back().accumVect->at(i).toDouble() * Load.toDouble());
			}
			else if (LoadPoint::isStr(Stack.back().accumVect->at(i).Type)) {
				std::string k = Stack.back().accumVect->at(i).toStr();
				for (int j = 0; j < Load.toDouble(); j++)
					Stack.back().accumVect->at(i).Write(Stack.back().accumVect->at(i).toStr() + k);
			}
		}
	}

	else {
		return;
	}
}

void	ALU::div(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		ProgExec(DivByZeroErrProg);
		return;
	}
	if (LoadPoint::isVect(Load.Type)) {
		int i = 0;
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(Stack.back().accumVect->at(i).toDouble() / (((vector<LoadPoint>*)Load.Point)->begin() + i)->toDouble());
				}

			}
		}
		else
			return;
	}

	else if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum /= Load.toDouble();
		if (Stack.back().accum != int(Stack.back().accum)) // Если результат деления дробный, то будет дробный тип результата
			Stack.back().accumType = Tdouble;
		else if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType))
	{
		int i;
		for (i = 0; i < Stack.back().accumVect->size(); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(Stack.back().accumVect->at(i).toDouble() / Load.toDouble());
			}
		}
	}
	else if(Load.isVect() && Load.isVect(Stack.back().accumType)) { // Два вектора
		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(Stack.back().accumVect->at(i).toDouble() / Load.toDouble());
			}
		}

		return;
	}
}

void	ALU::div_int(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		return;
	}
	if (Load.isVect() && LoadPoint::isVect(Stack.back().accumType)) {
		int N = min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size());
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (int i = 0; i < N; i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write((Stack.back().accumVect->at(i).toInt()) / (((vector<LoadPoint>*)Load.Point)->begin() + i)->toInt());
				}

			}
		}
		else
			return;
	}

	else if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = static_cast<int>(Stack.back().accum) / Load.toInt();
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))

			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType)) // Мультисложение вектора и числа
	{
		for (int i = 0; i < Stack.back().accumVect->size(); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(Stack.back().accumVect->at(i).toInt() / Load.toInt());
			}
		}

	}
	else
	{
		ProgExec(ErrProg);
		ProgExec(TypeMismatchErrProg);
	}
}

LoadPoint ALU::get()// выдать , проверку на данные нужно сделать
{
	switch (Stack.back().accumType >> 1)
	{
	case Dint:
	{
		long int t = round(Stack.back().accum);
		return{
			Stack.back().accumType, &t
		};

	}
	default:
		return{ 0,nullptr };
	}

}

void ALU::Out(LoadPoint Load) //
{
	if (Load.Type % 2 != 0) return; // Если константа, то запись в нее не производим
	if (Load.Point == nullptr) {
		//	((Threader*)Parent)->ProgExec(((Threader*)Parent)->NoOperandErrProg); // Запуск подпрограммы ошибки "Нет операнда"
		return;
	}
	if (LoadPoint::isStr(Load.Type))
	{
		*(string*)Load.Point = Stack.back().accumStr;
		return;
	}
	else if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		switch (Stack.back().accumType)
		{
		case Tdouble:;
		}
		{
			LoadPoint t = { (unsigned int)Stack.back().accumType, &Stack.back().accum };
			Load.Write(t);
		}
	}

	else {
		return;
	}
}

void* ALU::VarNew(LoadPoint Load)//создать новую переменную
{

	switch (Load.Type >> 1)
	{
	case Dint:
		return new int(*(int*)Load.Point);
	case Dfloat:
		return new float(*(float*)Load.Point);
	case Ddouble:
		return new double(*(double*)Load.Point);
	case Dchar:
		return new char(*(char*)Load.Point);
	case Dbool:
		return new bool(*(bool*)Load.Point);
	case Dstring:
		return new string(*(string*)Load.Point);
	}
}

void		ALU::set(LoadPoint Load) // Установить значение в аккумулятор
{
	if (Load.Point == nullptr) {
		Stack.back().accumType = Cint;
		Stack.back().accum = 0;
	}
	else if (Load.isVect() or Load.isVectInd())
	{
		Stack.back().accumVect = (vector<LoadPoint>*) Load.Point;
		Stack.back().accumType = Load.Type;
		Stack.back().Ind = Load.Ind;

	}
	else if (Load.isStr())
	{
		Stack.back().accumType = Load.Type;
		Stack.back().accumStr = Load.toStr();
	}
	else if (Load.isDigitBool())
	{
		Stack.back().accum = Load.toDouble();
		Stack.back().accumType = Load.Type;
	}
	else {
	}
	if (Stack.size() == 1) // Переписать в выходной аккумулятор
	{
		accumType = Stack.back().accumType;
		accum = Stack.back().accum;
	}
}

bool		ALU::getLogic()
{


	switch (Stack.back().accumType >> 1)
	{
	case Dint:
	case Dfloat:
	case Ddouble:
	case Dchar:
	case Dbool:
		return Stack.back().accum != 0;
	case Dstring:
		return Stack.back().accumStr != "";
	}

}

bool		ALU::getLogic(LoadPoint Load) // ����������� ����������� ����//все типы переходят в bool
{
	return false;
}

void	ALU::fu_max(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		return;
	}
	if (LoadPoint::isVect(Load.Type)) {
		int i = 0;
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(max(Stack.back().accumVect->at(i).toDouble(), (((vector<LoadPoint>*)Load.Point)->begin() + i)->toDouble()));
				}

			}
		}
		else
			return;
	}
	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = max(Stack.back().accum, Load.toDouble());
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}
	else if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type)) {

		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(max(Stack.back().accumVect->at(i).toDouble(), Load.toDouble()));
			}
		}
	}
	else {
		return;
	}
}

void	ALU::fu_min(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		return;
	}
	if (Load.isVect()) {
		int i = 0;
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(min(Stack.back().accumVect->at(i).toDouble(), (((vector<LoadPoint>*)Load.Point)->begin() + i)->toDouble()));
				}

			}
		}
		else
			return;
	}
	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = min(Stack.back().accum, Load.toDouble());
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType))
	{

		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(min(Stack.back().accumVect->at(i).toDouble(), Load.toDouble()));
			}
		}
	}
	else {
		return;
	}
}

void	ALU::fu_cos(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		int i = 0;
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(cos(Stack.back().accumVect->at(i).toDouble()));
				}

			}
		}
		else			if (Load.isDigitBool(Stack.back().accumType))
		{
			Stack.back().accum = cos(Stack.back().accum);
			Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
		}

		else

			return;
	}
	if (Load.isDigitBool())
	{
		Stack.back().accum = cos(Load.toDouble());
		Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
	}
	else
		if (Load.isVect()) {

			int i = 0;

			//		if (Stack.back().accumType >> 1 == DLoadVect) {
			if (LoadPoint::isVect(Stack.back().accumType)) {
				for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
					if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
						Stack.back().accumVect->at(i).Write(cos((((vector<LoadPoint>*)Load.Point)->begin() + i)->toDouble()));
					}

				}
			}
			else
				return;
		}
	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = cos(Load.toDouble());
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType))
	{

		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(cos(Load.toDouble()));
			}
		}
	}
	else {
		return;
	}
}

void	ALU::fu_sin(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		int i = 0;
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(sin(Stack.back().accumVect->at(i).toDouble()));
				}

			}
		}
		else			if (Load.isDigitBool(Stack.back().accumType))
		{
			Stack.back().accum = sin(Stack.back().accum);
			Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
		}
		return;
	}
	if (Load.isDigitBool())
	{
		Stack.back().accum = sin(Load.toDouble());
		Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
	}
	else
		if (LoadPoint::isVect(Load.Type)) {
			int i = 0;
			if (Stack.back().accumType >> 1 == DLoadVect) {
				for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
					if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
						Stack.back().accumVect->at(i).Write(sin((((vector<LoadPoint>*)Load.Point)->begin() + i)->toDouble()));
					}

				}
			}
			else
				return;
		}
	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = sin(Load.toDouble());
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType))
	{

		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(sin(Load.toDouble()));
			}
		}
	}
	else {
		return;
	}
}

void	ALU::fu_acos(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		int i = 0;
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(acos(Stack.back().accumVect->at(i).toDouble()));
				}

			}
		}
		else			if (Load.isDigitBool(Stack.back().accumType))
		{
			Stack.back().accum = acos(Stack.back().accum);
			Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
		}

		else
			return;
	}
	if (Load.isDigitBool()) //Enver//
	{
		Stack.back().accum = acos(Load.toDouble());
		Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
	}
	if (LoadPoint::isVect(Load.Type)) {
		int i = 0;
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(acos((((vector<LoadPoint>*)Load.Point)->begin() + i)->toDouble()));
				}

			}
		}
		else
			return;
	}
	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = acos(Load.toDouble());
		//		if (Stack.back().accumType >> 1 == Dbool && Load.Type >> 1 == Dbool)
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType))
	{
		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(acos(Load.toDouble()));
			}
		}
	}
	else {
		return;
	}
}

void	ALU::fu_asin(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		int i = 0;

		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(asin(Stack.back().accumVect->at(i).toDouble()));
				}

			}
		}
		else			if (Load.isDigitBool(Stack.back().accumType))
		{
			Stack.back().accum = asin(Stack.back().accum);
			Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
		}
		else
			return;
	}
	if (LoadPoint::isVect(Load.Type)) {
		int i = 0;
		if (Stack.back().accumType >> 1 == DLoadVect) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(asin((((vector<LoadPoint>*)Load.Point)->begin() + i)->toDouble()));
				}

			}
		}
		else
			return;
	}
	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = asin(Load.toDouble());
		Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType))
	{

		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(asin(Load.toDouble()));
			}
		}
	}
	else {
		return;
	}
}

void	ALU::fu_tan(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		int i = 0;

		//		if (Stack.back().accumType >> 1 == DLoadVect) {
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(tan(Stack.back().accumVect->at(i).toDouble()));
				}

			}
		}
		else			if (Load.isDigitBool(Stack.back().accumType))
		{
			//Enver//
			Stack.back().accum = tan(Stack.back().accum);
			Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
		}
		else
			return;
	}
	if (LoadPoint::isVect(Load.Type)) {
		int i = 0;
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(tan((((vector<LoadPoint>*)Load.Point)->begin() + i)->toDouble()));
				}

			}
		}
		else
			return;
	}
	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = tan(Load.toDouble());
		Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType))
	{

		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(tan(Load.toDouble()));
			}
		}
	}
	else {
		return;
	}
}

void	ALU::fu_atan(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		int i = 0;

		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(atan(Stack.back().accumVect->at(i).toDouble()));
				}

			}
		}
		else			if (Load.isDigitBool(Stack.back().accumType))
		{
			//Enver//
			Stack.back().accum = atan(Stack.back().accum);
			Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
		}

		else
			return;
	}
	if (LoadPoint::isVect(Load.Type)) {
		int i = 0;
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(atan((((vector<LoadPoint>*)Load.Point)->begin() + i)->toDouble()));
				}

			}
		}
		else
			return;
	}
	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = atan(Load.toDouble());
		Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType))
	{

		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(atan(Load.toDouble()));
			}
		}
	}
	else {
		return;
	}
}

void	ALU::fu_mod(LoadPoint Load)//остаток
{
	if (Load.Point == nullptr) {
		return;
	}
	if (LoadPoint::isVect(Load.Type)) {
		int i = 0;
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write((Stack.back().accumVect->at(i).toInt() % (((vector<LoadPoint>*)Load.Point)->begin() + i)->toInt()));
				}

			}
		}
		else
			return;
	}
	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{

		Stack.back().accum = static_cast<int>((int)Stack.back().accum) % Load.toInt();
		Stack.back().accumType = Cint;
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType))
	{

		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(Stack.back().accumVect->at(i).toInt() % Load.toInt());
			}
		}
	}
	else {
		return;
	}
}

void	ALU::fu_sqrt(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		int i = 0;

		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(sqrt(Stack.back().accumVect->at(i).toDouble()));
				}

			}
		}
		else			if (Load.isDigitBool(Stack.back().accumType))
		{
			//Enver//
			Stack.back().accum = sqrt(Stack.back().accum);
			Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
		}
		else
			return;
	}
	if (LoadPoint::isVect(Load.Type)) {
		int i = 0;
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(sqrt((((vector<LoadPoint>*)Load.Point)->begin() + i)->toDouble()));
				}

			}
		}
		else
			return;
	}
	if (Load.isDigitBool())
	{
		Stack.back().accum = sqrt(Load.toDouble());
		Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType))
	{

		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(sqrt(Load.toDouble()));
			}
		}
	}
	else {
		return;
	}
}

void	ALU::fu_pow(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		return;
	}
	if (LoadPoint::isVect(Load.Type)) {
		int i = 0;
		if (Stack.back().accumType >> 1 == DLoadVect) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(pow(Stack.back().accumVect->at(i).toDouble(), (((vector<LoadPoint>*)Load.Point)->begin() + i)->toDouble()));
				}

			}
		}
		else
			return;
	}
	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = pow(Stack.back().accum, Load.toDouble());
		Stack.back().accumType = max(Stack.back().accumType, Load.Type); // Коррекция типа результата
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType))
	{

		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(pow(Stack.back().accumVect->at(i).toDouble(), Load.toDouble()));
			}
		}
	}
	else {
		return;
	}
}

void	ALU::fu_abs(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		int i = 0;

		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(abs(Stack.back().accumVect->at(i).toDouble()));
				}

			}
		}
		else			if (Load.isDigitBool(Stack.back().accumType))
			Stack.back().accum = abs(Stack.back().accum);
		else
			// Запуск подпрограммы ошибки "Нет операнда"
			return;
	}
	if (Load.isVect()) {
		int i = 0;
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(abs((((vector<LoadPoint>*)Load.Point)->begin() + i)->toDouble()));
				}

			}
		}
		else

			return;
	}
	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = abs(Load.toDouble());
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType))
	{

		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(abs(Load.toDouble()));
			}
		}
	}
	else {

		return;
	}
}

void	ALU::fu_ceil(LoadPoint Load)//округление вверх
{
	if (Load.Point == nullptr) {
		int i = 0;

		//		if (Stack.back().accumType >> 1 == DLoadVect) {
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(ceil(Stack.back().accumVect->at(i).toDouble()));
				}

			}
		}
		else			if (Load.isDigitBool(Stack.back().accumType))
		{
			Stack.back().accum = ceil(Stack.back().accum);
			Stack.back().accumType = Cint;
		}

		else
			// Запуск подпрограммы ошибки "Нет операнда"
			return;
	}
	//	if (Load.Type >> 1 == DLoadVect) {
	if (LoadPoint::isVect(Load.Type)) {
		int i = 0;
		//		if (Stack.back().accumType >> 1 == DLoadVect) {
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(ceil((((vector<LoadPoint>*)Load.Point)->begin() + i)->toDouble()));
				}

			}
		}
		else
			return;
	}
	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = ceil(Load.toDouble());
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cbool;
		else
			Stack.back().accumType = Cint;
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType))
	{

		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(ceil(Load.toDouble()));
			}
		}
	}
	else {
		return;
	}
}

void	ALU::fu_floor(LoadPoint Load)//округление вниз
{
	if (Load.Point == nullptr) {
		int i = 0;

		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(floor(Stack.back().accumVect->at(i).toDouble()));
				}

			}
		}
		else			if (Load.isDigitBool(Stack.back().accumType))
		{
			Stack.back().accum = floor(Stack.back().accum);
			Stack.back().accumType = Cint;
		}
		return;
	}
	if (LoadPoint::isVect(Load.Type)) {
		int i = 0;
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(floor((((vector<LoadPoint>*)Load.Point)->begin() + i)->toDouble()));
				}
			}
		}
		else
			return;
	}
	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = floor(Load.toDouble());
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType))
	{

		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(floor(Load.toDouble()));
			}
		}
	}
	else {
		return;
	}
}

void	ALU::fu_round(LoadPoint Load)//просто округление
{
	if (Load.Point == nullptr) {
		int i = 0;

		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(round(Stack.back().accumVect->at(i).toDouble()));
				}

			}
		}
		else			if (Load.isDigitBool(Stack.back().accumType))
		{
			Stack.back().accum = round(Stack.back().accum);
			Stack.back().accumType = Cint;
		}
		return;
	}
	if (LoadPoint::isVect(Load.Type)) {
		int i = 0;
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(round((((vector<LoadPoint>*)Load.Point)->begin() + i)->toDouble()));
				}

			}
		}
		else
			return;
	}
	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = round(Load.toDouble());
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType))
	{

		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(round(Load.toDouble()));
			}
		}
	}
	else {
		return;
	}
}

void	ALU::fu_log(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		int i = 0;

		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(log(Stack.back().accumVect->at(i).toDouble()));
				}

			}
		}
		else			if (Load.isDigitBool(Stack.back().accumType))
		{
			//Enver//
			Stack.back().accum = log(Stack.back().accum);
			Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
		}

		else
			return;
	}
	if (Load.isDigitBool()) //Enver//
	{
		Stack.back().accum = log(Load.toDouble());
		Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
	}
	if (LoadPoint::isVect(Load.Type)) {
		int i = 0;
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(log((((vector<LoadPoint>*)Load.Point)->begin() + i)->toDouble()));
				}

			}
		}
		else
			return;
	}
	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = log(Load.toDouble());
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType))
	{

		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(log(Load.toDouble()));
			}
		}
	}
	else {
		return;
	}
}

void	ALU::fu_log10(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		int i = 0;
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(log10(Stack.back().accumVect->at(i).toDouble()));
				}

			}
		}
		else			if (Load.isDigitBool(Stack.back().accumType))
		{
			//Enver//
			Stack.back().accum = log10(Stack.back().accum);
			Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
		}

		else
			return;
	}
	if (Load.isDigitBool()) //Enver//
	{
		Stack.back().accum = log10(Load.toDouble());
		Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
	}
	if (LoadPoint::isVect(Load.Type)) {
		int i = 0;
		//		if (Stack.back().accumType >> 1 == DLoadVect) {
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(log10((((vector<LoadPoint>*)Load.Point)->begin() + i)->toDouble()));
				}

			}
		}
		else
			return;
	}
	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = log10(Load.toDouble());
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType))
	{

		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(log10(Load.toDouble()));
			}
		}
	}
	else {
		return;
	}
}

void	ALU::fu_log2(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		int i = 0;

		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(log2(Stack.back().accumVect->at(i).toDouble()));
				}

			}
		}
		else			if (Load.isDigitBool(Stack.back().accumType))
		{
			//Enver//
			Stack.back().accum = log2(Stack.back().accum);
			Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
		}

		else
			return;
	}
	if (Load.isDigitBool()) //Enver//
	{
		Stack.back().accum = log2(Load.toDouble());
		Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
	}
	if (LoadPoint::isVect(Load.Type)) {
		int i = 0;
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(log2((((vector<LoadPoint>*)Load.Point)->begin() + i)->toDouble()));
				}

			}
		}
		else
			return;
	}
	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = log2(Load.toDouble());
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType))
	{

		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(log2(Load.toDouble()));
			}
		}
	}
	else {
		return;
	}
}

void	ALU::fu_exp(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		int i = 0;

		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(exp(Stack.back().accumVect->at(i).toDouble()));
				}
			}
		}
		else			if (Load.isDigitBool(Stack.back().accumType))
		{
			//Enver//
			Stack.back().accum = exp(Stack.back().accum);
			Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
		}

		else
			return;
	}
	if (Load.isDigitBool()) //Enver//
	{
		Stack.back().accum = exp(Load.toDouble());
		Stack.back().accumType = Tdouble | (Stack.back().accumType & 1);
	}
	if (LoadPoint::isVect(Load.Type)) {
		int i = 0;
		if (LoadPoint::isVect(Stack.back().accumType)) {
			for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
				if (Stack.back().accumVect->at(i).isDigit() && (((vector<LoadPoint>*)Load.Point)->begin() + i)->isDigit()) {
					Stack.back().accumVect->at(i).Write(exp((((vector<LoadPoint>*)Load.Point)->begin() + i)->toDouble()));
				}

			}
		}
		else
			return;
	}
	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = exp(Load.toDouble());
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}
	else if (Load.isDigitBool() && LoadPoint::isVect(Stack.back().accumType))
	{

		int i;
		for (i = 0; i < min(Stack.back().accumVect->size(), ((vector<LoadPoint>*)Load.Point)->size()); i++) {
			if (Stack.back().accumVect->at(i).isDigit()) {
				Stack.back().accumVect->at(i).Write(exp(Load.toDouble()));
			}
		}
	}
	else {
		return;
	}
}

void	ALU::fu_random(LoadPoint Load)
{
	//Enver//
	if (Load.Point == nullptr)
	{
		if (Load.isDigitBool(Stack.back().accumType))
			if (!LoadPoint::isVect(Stack.back().accumType))
				Stack.back().accum = rand();
			else {
				Stack.back().accumVect->push_back({ Cdouble,new double });
				*(double*)Stack.back().accumVect->back().Point=rand();
			}
				
	}
	else
		if (!LoadPoint::isVect(Stack.back().accumType))
			Stack.back().accum = rand() % Load.toInt();
		else {
			Stack.back().accumVect->push_back({ Cdouble,new double });
			*(double*)Stack.back().accumVect->back().Point = rand() % Load.toInt();
		}
//	cout<<"Rand:"<< Stack.back().accum<<endl;
}

void	ALU::getCos(LoadPoint Load)
{
}

void	ALU::getSin(LoadPoint Load)
{
}

void		ALU::fu_inverse(LoadPoint Load)//обратное
{
	if (Load.Point == nullptr) {

		if (Load.isDigitBool(Stack.back().accumType))
		{
			if (Stack.back().accum == 0)
				Stack.back().accum = 1;
			else
				Stack.back().accum = 0;
		}

		return;
	}
	if (LoadPoint::isStr(Load.Type))
	{
		if (Load.toStr().length() != 0)
			Stack.back().accum = 0;
		else
			Stack.back().accum = 1;
		return;
	}
	else if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		if (Load.toDouble() == 0)
			Stack.back().accum = 1;
		else
			Stack.back().accum = 0;
	}

	else {
		return;
	}
}

void		ALU::fu_and(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		return;
	}
	if (LoadPoint::isStr(Load.Type))
	{
		if (LoadPoint::isStr(Stack.back().accumType))
			if (Load.toStr().length() != 0 && Stack.back().accumStr.length() != 0)
				Stack.back().accum = 1;
			else
				Stack.back().accum = 0;
		return;
	}
	else if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		if (Load.toDouble() == 0 || Stack.back().accum == 0)
			Stack.back().accum = 0;
		else
			Stack.back().accum = 1;
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}

	else {
		return;
	}
}

void		ALU::fu_or(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		return;
	}
	if (LoadPoint::isStr(Load.Type))
	{
		if (LoadPoint::isStr(Stack.back().accumType))
			if (Load.toStr().length() != 0 || Stack.back().accumStr.length() != 0)
				Stack.back().accum = 1;
			else
				Stack.back().accum = 0;
		else
			return;
	}
	else if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		if (Load.toDouble() == 0 && Stack.back().accum == 0)
			Stack.back().accum = 0;
		else
			Stack.back().accum = 1;
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}

	else {
		return;
	}
}

void		ALU::fu_xor(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		return;
	}
	if (LoadPoint::isStr(Load.Type))
	{
		if (LoadPoint::isStr(Stack.back().accumType))
			if (Load.toStr().length() != 0 || Stack.back().accumStr.length() != 0)
				Stack.back().accum = 1;
			else
				Stack.back().accum = 0;
		else
			return;
	}
	else if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		if ((Load.toDouble() != 0 && Stack.back().accum == 0) || (Load.toDouble() == 0 && Stack.back().accum != 0))
			Stack.back().accum = 1;
		else
			Stack.back().accum = 0;
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}

	else {
		return;
	}
}

void		ALU::fu_or_bit(LoadPoint Load)//bit побитовые логические операции
{
	{
		if (Load.Point == nullptr) {
			return;
		}
		else if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
		{

			Stack.back().accum = static_cast<int>(Stack.back().accum) | Load.toInt();
			if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
				Stack.back().accumType = Cint;
			else
				Stack.back().accumType = max(Stack.back().accumType, Load.Type);
		}

		else {
			return;
		}
	}
}

void		ALU::fu_and_bit(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		//((Threader*)Parent)->ProgExec(((Threader*)Parent)->NoOperandErrProg); // Запуск подпрограммы ошибки "Нет операнда"
		return;
	}

	else if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = static_cast<int>(Stack.back().accum) & Load.toInt();
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}

	else {
		return;
	}
}

void		ALU::fu_mr_bit(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		return;
	}

	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = static_cast<int>(Stack.back().accum) >> Load.toInt();
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}

	else {
		return;
	}
}

void		ALU::fu_ml_bit(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		return;
	}

	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = static_cast<int>(Stack.back().accum) << Load.toInt();
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}

	else {
		return;
	}
}

void		ALU::fu_inv_bit(LoadPoint Load)
{
	if (Load.Point == nullptr) {
		return;
	}
	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = ~Load.toInt();
		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cbool;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}

	else {
		return;
	}
}
void		ALU::Compar3Way(LoadPoint Load) {// Трехстороннее сравнение
	if (Load.Point == nullptr) {
		return;
	}
	if (Load.isDigitBool() && Load.isDigitBool(Stack.back().accumType))
	{
		Stack.back().accum = ((Stack.back().accum > 0) ? 1 : ((Stack.back().accum == Load.toDouble()) ? 0 : -1));

		if (LoadPoint::isBool(Stack.back().accumType) && LoadPoint::isBool(Load.Type))
			Stack.back().accumType = Cint;
		else
			Stack.back().accumType = max(Stack.back().accumType, Load.Type);
	}

}
void ALU::length(LoadPoint Load) {
	if (LoadPoint::isVect(accumType) || LoadPoint::isStr(accumType))
		Load.Write(Stack.back().accumVect->size());
	else
		Load.Write(0);
}
void ALU::lenMk(LoadPoint Load) {
	long int l;
	if (LoadPoint::isVect(accumType) || LoadPoint::isStr(accumType)) {
		l = Stack.back().accumVect->size();
		MkExec(Load, { Cint,&l }, Receiver);
	}
	else {
		l = 0;
		MkExec(Load, { Cint,&l }, Receiver);
	}
}
void ALU::clear(LoadPoint Load) {
	if (Load.Point == nullptr) {

		return;
	}
	else if (Load.isStr()) {

		string s;
		Stack.back().accumStr = s;
	}
	else if (Load.isVect()) {
		Stack.back().accumType >> 1 == DLoadVect;
		vector<LoadPoint> v;
		*Stack.back().accumVect = v;
	}
}
void ALU::push_back(LoadPoint Load) {
	if (Load.Point == nullptr) {
		return;
	}
	else if (Load.isVect(accumType)) {

		if (Load.isDigitBool() || Load.isStr()) {
			Stack.back().accumVect->push_back(Load);
		}

	}
	else if (Load.isStr(accumType) && (Load.isStrChar(accumType)))
		Stack.back().accumStr += Load.toStr();
}

void ALU::emplace_back(LoadPoint Load) {
	if (Load.Point == nullptr) {

		return;
	}
	else if (LoadPoint::isVect(accumType)) {
		if (Load.isDigitBool() || Load.isStr()) {
			Stack.back().accumVect->emplace_back(Load);
		}
	}
}
void ALU::pop_back(LoadPoint Load) {
	if (Stack.back().accumVect->empty() && Stack.back().accumStr.empty()) {
		return;
	}
	else if (LoadPoint::isVect(accumType)) {
		Load.Write(Stack.back().accumVect->back());
		Stack.back().accumVect->pop_back();
	}
	Load.Write(Stack.back().accumStr.back());
	Stack.back().accumStr.pop_back();
}
void ALU::insert(LoadPoint Load) {
	if (Load.Point == nullptr || Stack.back().IndF == false) {
		return;
	}
	else {

		Stack.back().accumVect->insert(Stack.back().accumVect->begin() + Stack.back().Ind, Load.Clone());
	}
}
void ALU::emplace(LoadPoint Load) {
	if (Load.Point == nullptr) {
		return;
	}
	else if (LoadPoint::isVect(accumType)) {
		Stack.back().accumVect->emplace(Stack.back().accumVect->begin() + Stack.back().Ind, Load.Clone());
	}
	else if (Load.isChar()) {
		Stack.back().accumStr.insert(Stack.back().Ind, Load.toStr());
	}
}

void ALU::pop_backMk(LoadPoint Load) {

	if (!Stack.back().accumVect->empty()) {
		return;
	}
	MkExec(Load, { Cdouble, &Stack.back().accumVect }, Receiver);
	Stack.back().accumVect->pop_back();

}

void ALU::pop(LoadPoint Load) { //???

	if (Stack.back().accumVect->empty() && Stack.back().accumStr.empty()) {
		return;
	}
	if (Load.isInt()) {
		if (LoadPoint::isVect(accumType))
			Stack.back().accumVect->erase(Stack.back().accumVect->begin() + Load.toDouble(), Stack.back().accumVect->end());
		else
			Stack.back().accumStr.erase(Stack.back().accumStr.begin() + Load.toDouble(), Stack.back().accumStr.end());
	}
}

void ALU::del(LoadPoint Load) {

	if (Stack.back().accumVect->empty() && Stack.back().accumStr.empty()) {
		return;
	}
	if (Load.isInt()) {
		if (LoadPoint::isVect(accumType))
			Stack.back().accumVect->erase(Stack.back().accumVect->begin() + Load.toDouble());
		else
			Stack.back().accumStr.erase(Stack.back().accumStr.begin() + Load.toDouble());
	}
}

void ALU::Reverse(LoadPoint Load) {

	if (Stack.back().accumVect->empty() && Stack.back().accumStr.empty()) {
		return;
	}
	if (LoadPoint::isVect(accumType)) {
		reverse(Stack.back().accumVect->begin(), Stack.back().accumVect->end());
	}
	else
		reverse(Stack.back().accumStr.begin(), Stack.back().accumStr.end());

}

void ALU::Sort(LoadPoint Load, bool revers) {
	if (Load.Point != nullptr)
		set(Load); // Если нагрузка непустая, то установить аккумулятор

	if (Stack.back().accumVect->empty() || Stack.back().accumStr.empty()) {
		return;
	}
	if (LoadPoint::isVect(accumType)) {
		sort(Stack.back().accumVect->begin(), Stack.back().accumVect->end(), [&revers](LoadPoint& x, LoadPoint& y)->bool {
			if (x.isDigit() && y.isDigit()) {
				return revers ? x.toDouble() > y.toDouble() : x.toDouble() < y.toDouble();
			}
			if (x.isStr() && y.isStr()) {
				return revers ? x.toStr() > y.toStr() : x.toStr() < y.toStr();
			}
			return false;
			});
	}
	if (LoadPoint::isStr(accumType))
	{
		sort(Stack.back().accumStr.begin(), Stack.back().accumStr.end());
	}
	else // Ошибка типов
	{
		ProgExec(ErrProg);
		ProgExec(TypeMismatchErrProg);
	}
}

FU* ALU::Copy() // Программа копирования ФУ
{
	return new ALU(Bus, this);
}

FU* ALU::TypeCopy() // Создать ФУ такого же типа (не копируя контекст)
{
	return new ALU(Bus, nullptr);
}
/*
void ALU::ProgExec(void* Uk, unsigned int CycleMode, FU* Bus, vector<ip>::iterator* Start) // Исполнение программы из ИК
{
	if(!ExecCounter.size())
		FU::ProgExec(Uk, CycleMode, Bus, Start);
	else
	{
		for (int i = 0; i < ExecCounter.back(); i++)
			FU::ProgExec(Uk, CycleMode, Bus, Start);
		ExecCounter.pop_back();
	}
}
void ALU::ProgExec(LoadPoint Uk, unsigned int CycleMode, FU* Bus, vector<ip>::iterator* Start) // Исполнение программы из ИК
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
*/