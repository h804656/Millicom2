	#include "stdafx.h"
	#include "Lex.h"
	#include <string.h>

	using namespace std;

	void Lex::LexOut(bool Copy,int MK) // Выдача лексемы потребителю
	{
		auto uk = UnicAtr.find(LexBuf[ib].atr);
		if (MK < 0)MK = ReceiverMK.back();
		if (uk!= UnicAtr.end())
		{
			if (uk->second.Fu != nullptr && uk->second.Mk!= 0)
				if(!Copy)
					uk->second.Fu->ProgFU(uk->second.Mk, { TIP, &LexBuf[ib] }, this);
				else
					uk->second.Fu->ProgFU(uk->second.Mk, { TIP, LexBuf[ib].Clone() }, this);
		}
		else
			if (Receiver.back() != nullptr)
				if (Copy)
					Receiver.back()->ProgFU(MK, { TIP, LexBuf[ib].Clone() }, this);
				else
					Receiver.back()->ProgFU(MK, { TIP, &LexBuf[ib] }, this);
	}

	void Lex::ProgFU(long int MK, LoadPoint Load, FU* Sender)
	{
		// Доделать буфер ИП с лексемами
		MK %= FUMkRange;
		switch (MK)
 		{
		case 0: // Reset
			S = 0; // Номер состояния распознающего автомата
				   //		LexAccum = "";
			UnicAtr.clear();
			ReceiverMK.back() = 0;
			Receiver.back() = Bus;
			ErrProg = nullptr;
			StopProg = nullptr;
			TabErrProg = nullptr;
			LexAccum="";
			break;
		case 2: // ReceiverPush Положить старые адрес и Мк приёмника лексемы в стек и установить МК для приемника
			Receiver.push_back(Receiver.back());
			if(Load.Point==nullptr)
				ReceiverMK.push_back(ReceiverMK.back());
			else
				ReceiverMK.push_back(Load.toInt());
			break;
		case 3: // ReceiverPop Вытолкнуть Мк премника из стека и записать ее
			{
			long int t = ReceiverMK.back();
			if (!ReceiverMK.size()) break;
			ReceiverMK.pop_back();
			Receiver.pop_back();
			MkExec(Load, {Cint,&t});
			break;
			}
		case 4: // ReceiverPopMk Вытолкнуть адрес премника из стека и выдать МК с ней
		{
			Load.Write(ReceiverMK.back());
			if (!ReceiverMK.size()) break;
			ReceiverMK.pop_back();
			Receiver.pop_back();
		}
			break;
		case 1: // ReceiverPopCend Вытолкнуть Мк премника из стека и выдать Лексему (при нулевой нагрузке выдается текущая лексема)
			if (!ReceiverMK.size()) break;
			ReceiverMK.pop_back();
			Receiver.pop_back();
			if (Load.Point == nullptr)
				LexOut();
				//Receiver.back()->ProgFU(MK, { TIP, &LexBuf[ib] });
			else
    			Receiver.back()->ProgFU(ReceiverMK.back(), Load, this);
			break;
		case 5: //ReceiverMKSet Установить МК для приемника лексем 
			if (Load.Type >> 1 == Dint) ReceiverMK.back() = Load.toInt(); break;
		case 6: // FinMkAdd Добавить финальную МК
			FinMk.insert(Load.toInt());
			break;
		case 7: // FinMkClear Очистить список фитальных МК
			FinMk.clear();
			break;
		case 8: // FinMkProgExec Выполнить программу, если лексер в финальном состоянии
			if (FinMk.count(ReceiverMK.back()))
				ProgExec(Load);
			break; 
		case 9: // NoFinMkProgExec Выполнить программу, если лексер не в финальном состоянии
			if (!FinMk.count(ReceiverMK.back()))
				ProgExec(Load);
			break;
		case 10: //ErrProgSet
			if (Load.isProg())
				ErrProg = Load.Point;
			break;
		case 11: // TabErrorProgSet Ошибка табуляции
			if(Load.isProg())
				TabErrProg = Load.Point;
			break;
		case 12: // StartProgSet Установить программу, запускаемую перед началом компиляции 
			StartProg = (IC_type)Load.Point;
			break;
		case 13: // StopProgSet Установить программу, запускаемую при досрочном завершении лексического анализа по МК Stop 
			StopProg = (IC_type)Load.Point;
			break;
		case 14: // FinProgSet  Установить программу, запускаемую после окончания анализа строки
			FinProg = (IC_type)Load.Point;
			break;
		case 15:// UnicAtrSet Установить уникальный атрибут
			LastUnicAtr = Load.toInt();
			UnicAtr[LastUnicAtr] = {0,Bus};
			break;
		case 16:// UnicMkSet Установить МК для уникального атрибута
			if (UnicAtr.size())
				UnicAtr[LastUnicAtr].Mk = Load.toInt();
			break;
		case 17:// UnicFuSet Установить контекст для уникального атрибута
			if (UnicAtr.size())
				UnicAtr[LastUnicAtr].Fu = (FU*)Load.Point;
			break;
		case 18:// UnicReset Сбросить список уникальных атрибутов
			UnicAtr.clear();
			break;
		case 20: // LexBufSizeSet Установить размер буфера лексем
			if (Load.Type >> 1 != Dint) break;
			for (int i = 0; i < SizeBuf; LexBuf[i++].Load.Clear());
			delete[] LexBuf;
			SizeBuf = Load.toInt(2);
			LexBuf = new ip[SizeBuf];
			for (int i = 0; i < SizeBuf; LexBuf[i++].Load = { 0,nullptr });
			LexBuf[0].atr=SeperatAtr;// Установить атрибут сеирататора для для начального псевдосимвола
			ib = 0;
			ProgLevel = 0; // Уровень счетчик программный скобок
			break;
		case 21: // SepSet Установить разделить
			Seps.insert(Load.toStr());
			break;
		case 22: // SepErase Удалить разделитель из списка разделителей
			Seps.erase(Load.toStr());
			break;
		case 23: // SepClear Очистить список разделителей
			Seps.clear();
			break;
		case 24: // FalseAdd Добавить мнемонику лжи
			FalseConst.insert(Load.toStr());
			break;
		case 25: // TrueAdd Добавить мнемонику правды
			TrueConst.insert(Load.toStr());
			break;
		case 19: // TrueFalseClear Очистить списки мнемоник правды и лжи
			FalseConst.clear();
			TrueConst.clear();
			break;
		case 26: // NoUnucToReseiver Выдать лексему, не учитывая уникальных атрибутов (при нулевой нагрузке выдается текущая лексема)
			if (Load.Point == nullptr)
				MkExec(ReceiverMK.back(), LexBuf[ib].Load, Receiver.back());
			else
			{
				ib = (ib + 1) % SizeBuf;
				LexBuf[ib].Load.Clear(); // Удаляем нагрузку ИП
				LexBuf[ib].copy(Load);
				MkExec(Load, LexBuf[ib].Load, Receiver.back());
			}
			break;
		case 27: //Replace Заменить тукущую лексему в буфере
		case 28: //ReplaceToReceiver Выдать лексему из нагрузка МК получателю, заменив тукущую лексему в буфере
		case 29: //ReplaceCopyToReceiver Выдать копию лексемы из нагрузки МК получателю, заменив тукущую лексему в буфере
		case 30: //ToReceiver Выдать лексему из нагрузки МК получателю (Если нагрузка nil, то выдается текущая лексема)
		case 31: //CopyToReceiver Выдать копию лексемы из нагрузки МК получателю
			if(MK==30 && Load.Point==nullptr)	{
				LexOut(false);
				break;
			}
			if(MK==30 || MK==31) ib = (ib + 1) % SizeBuf;
			LexBuf[ib].Load.Clear(); // Удаляем нагрузку ИП
			LexBuf[ib].copy(Load);
			if(MK!=27)LexOut(MK==31);
			break;
		case 35: //OutMk Выдать MK c последней лексемой (если nil в нагрузке, то выдается на Receiver)
			if (Load.Point == nullptr)
			{
				//LexOut(false);
				Receiver.back()->ProgFU(ReceiverMK.back(), { TIP, &LexBuf[ib] }, this);
			}
			else
				if(Load.Type>>1==Dint)
					Receiver.back()->ProgFU(Load.toInt(), { TIP, &LexBuf[ib] }, this);
			break;
		case 36: // CopyOutMk Выдать МК с копией последней лексемы (если nil в нагрузке, то выдается на Receiver)
			if (Load.Point!=nullptr) 
				Receiver.back()->ProgFU(*(int*)Load.Point, { TIP, LexBuf[ib].Clone() }, this);
			else
				Receiver.back()->ProgFU(ReceiverMK.back(), { TIP, LexBuf[ib].Clone() }, this);
			break;
		case 37: // LoadOut Выдать нагрузку текущей лексемы
			Load.Write(LexBuf[ib].Load);
			break;
		case 38: // LoadOutMk Выдать МК с нагрузкой текущей лексемы
			MkExec(Load, LexBuf[ib].Load);
			break;
		case 39: // LoadCopyOut Выдать нагрузку текущей лексемы
			Load.Write(LexBuf[ib].Load.Clone());
			break;
		case 40: // LoadCopyOutMk Выдать МК с копией нагрузки текущей лексемы
			MkExec(Load, LexBuf[ib].Load.Clone());
			break;
		case 41: // AtrOut Выдать атрибут текущей лексемы
			Load.Write(LexBuf[ib].atr);
			break;
		case 42: // AtrOutMk Выдать МК с атрибутом текущей лексемы
			MkExec(Load, { Cint, &LexBuf[ib].atr });
			break;
		case 45: //PrevOutMk Выдать МК с предыдущей лексемой (если нагрузка nil, то выдается на Receiver)
		case 46: //PrevPrevOutMk Выдать МК с предпредыдущей лексемой (если нагрузка nil, то выдается на Receiver)
			if (Load.Point==nullptr)
				Receiver.back()->ProgFU(ReceiverMK.back(), { TIP, &LexBuf[(ib - MK + 44) % SizeBuf] }, this);
			else
				Receiver.back()->ProgFU(Load.toInt(), {TIP, &LexBuf[(ib - MK + 44) % SizeBuf]}, this);
			break;
		case 47: //PrevCopyOutMk Выдать МК с копией предыдущей лексемы (если нагрузка nil, то выдается на Receiver)
		case 48: //PrevPrevCopyOutMk Выдать МК с копией предпредыдущей лексемы (если нагрузка nil, то выдается на Receiver)
			if (Load.Point == nullptr)
				Receiver.back()->ProgFU(ReceiverMK.back(), { TIP, LexBuf[(ib - MK + 46) % SizeBuf].Clone() }, this);
			else	
				Receiver.back()->ProgFU(Load.toInt(), {TIP, LexBuf[(ib - MK + 46) % SizeBuf].Clone()}, this);
			break;
		case 50: // AtrSet Установить атрибут последней лексемы
			LexBuf[ib].atr=Load.toInt();
			break;
		case 51: // PrevAtrSet Установить атрибут предыдущей лексемы
			LexBuf[(ib - 1 + SizeBuf) % SizeBuf].atr = Load.toInt();
			break;
		case 55: // LoadSet Установить нагрзузку у последней лексемы
			LexBuf[ib].Load = Load;
			break;
		case 56: // PrevLoadSet Установить нагрзузку у предыдущей лексемы
			LexBuf[(ib - 1 + SizeBuf) % SizeBuf].Load = Load;
			break;
		case 57: // LoadCopySet Установить копию нагрзузки у последней лексемы
			LexBuf[ib].Load.Clone(Load);
			break;
		case 58: // PrevLoadCopySet Установить копию нагрзузки у предыдущей лексемы
			LexBuf[(ib - 1 + SizeBuf) % SizeBuf].Load.Clone(Load);
			break;
		case 65: // VarSet Установить тип переменной для нагрузки последней лексемы
			LexBuf[ib].Load.Type |= 1;
			LexBuf[ib].Load.Type --;
			break;
		case 66: // PrevVarSet Установить тип переменной для нагрузки предыдущей лексемы
			LexBuf[(ib - 1 + SizeBuf) % SizeBuf].Load.Type |= 1;
			LexBuf[(ib - 1 + SizeBuf) % SizeBuf].Load.Type--;
			break;

		case 70: // SendToReceiver Переслать лексему из нагрузки получателю (если в нагрузке nil, то посылается текущая лексема)
			// SendToReceiver always forwards to the current receiver. The
			// nil-load branch used to call LexOut(), but LexOut re-fires UnicMk
			// dispatch on the current token -- which infinitely recurses if a
			// MnemoTable entry attached `Lex.SendToReceiver` (with no payload)
			// as its program. Dispatch the current LexBuf entry to the receiver
			// directly to break that loop.
			if (Load.Point == nullptr)
				Receiver.back()->ProgFU(ReceiverMK.back(), { TIP, &LexBuf[ib] }, this);
			else
				Receiver.back()->ProgFU(ReceiverMK.back(), Load, this);

/*
			if (Load.Point == nullptr)
				LexOut();
			else if(Load.isIC() || Load.isIP())
			{
				ib = (ib + 1) % SizeBuf; //увеличение текущего адреса буфера выходных лексем на 1
				LexBuf[ib].Load.Clear(); //удаление нагрузки ИП
				if(Load.isIP())
					LexBuf[ib] = ((ip*)Load.Point)->Copy(); //добавление лексемы в буфер выходных лексем в виде ИП {атрибут, тип, указатель}
				else
					LexBuf[ib] = ((IC_type)Load.Point)->begin()->Copy(); //добавление лексемы в буфер выходных лексем в виде ИП {атрибут, тип, указатель}

				LexOut();
			}
*/

			break;
		case 71: // CendCopyToReceiver Переслать копию лексемы из нагрузки получателю (если в нагрузке nil, то посылается текущая лексема)
			if (Load.Point == nullptr)
				LexOut(true);
			else
				Receiver.back()->ProgFU(ReceiverMK.back(), Load.Clone(), this);
//			MkExec(ReceiverMK.back(), Load.Copy());
			break;

		case 85: // RegSet Установить регулярное выражение
			RegVect.push_back({ Load.toStr(), nullptr});
			break;
		case 86: // RegProgSet Установить программу обработки лексемы, выделенной регулярным выражением
			if(RegVect.size())
				RegVect.back().Prog=Load.Point;
			break;
		case 90: // IfStopExec Выполнить, если лексер остановлен
			if (!Work)
				ProgExec(Load);
			break;
		case 91: // IfRunExec Выполнить, если лексер в рабочем состоянии
			if (Work)
				ProgExec(Load);
			break;

		case 94: // LexStrOut Выдать строку с последней распознанной лексемой
			Load.Write(LexAccum);
			break;
		case 95: // LexStrOutMk Выдать МК со строкой с последней распознанной лексемой
			MkExec(Load, { Cstring, &LexAccum });
			break;
		case 96: //TabModeSet Установить решим генерации скобок по символам табуляции (по умолчанию ture)
			TabMode = Load.toBool(true);
			break;
		case 98: // LexReset Сбросить настройки лексического анализа
			S = 0;
			LexBuf[0].atr = SeperatAtr;
			ProgLevel = 0; // Счетчик табуляций			LexAccum="";
			break;
		case 99: // Stop Остановить лексический анализ (Эту МК необходимо выполнить при перезапуске лексичекого анализа)
			Work = false;
			ProgExec(StopProg); // выполнить программу по останову лексического анализа
			S = 0;
			LexBuf[0].atr = SeperatAtr;
			ProgLevel = 0; // Счетчик табуляций
			break;
		case 100: // Lexing
		{	string str = Load.toStr()+" ";
			str += EOL; // Дабавить символы конца строки
			S = 0; // --- установка начального состояния автомата
			if (Load.Type >> 1 == Dstring && Load.toStr() == "")
			{
				ib = (ib + 1) % SizeBuf;
				LexBuf[ib].Load.Clear(); // Удаляем нагрузку ИП
				LexBuf[ib] = { StrAtr, Cstring, new string("") };
				LexOut();
			}
			Work = true;
			if (TabMode) {
				long int tabCounter = 0;
				while (str[tabCounter] == '\t')
					tabCounter++;
				if (tabCounter > ProgLevel + 1) // Табуляция больше чем на одну позицию
				{
					ProgExec(ErrProg); // Общая ошибка лексического анализа
					ProgExec(TabErrProg); // Ошибка табуляции
					break;
				}
				if (tabCounter > ProgLevel && ProgBracket!="")
				{
					ib = (ib + 1) % SizeBuf;
					LexBuf[ib].Load.Clear(); // Удаляем нагрузку ИП
					//string t = ProgBracket;
					LexBuf[ib] = { SeperatAtr,Cstring , new string(ProgBracket) };
					LexOut();
				}
				if(ProgFinBracket!="")
					for (int i = ProgLevel - tabCounter; i > 0; i--) // Выдать закрывающеся програмные скобки
					{
						ib = (ib + 1) % SizeBuf;
						LexBuf[ib].Load.Clear(); // Удаляем нагрузку ИП
						//string *t = new string(ProgFinBracket);
						LexBuf[ib] = { SeperatAtr,Cstring , new string(ProgFinBracket) };
						LexOut();
					}
				ProgLevel = tabCounter; // Запомнить текущий программный уровень
				if (tabCounter) // Убрать символы табуляции
					str = str.substr(tabCounter, string::npos);
			}
			for (auto i = str.begin(); i != str.end() && Work; i++)
				switch (S) //LEXER
				{
				// Стартовое состояние
				case 0:
				{
					// Поиск по регулярке
					// Если найдено, то помещаемы выделенную лексему в LexAccum и запускаем ОА программу для обработки лексемы
				/*
					if (найдена регулярка)
					{
						LexAccum=выделенная лексема
						ProgExec(RegVect[k].Prog); k - это индекс найденной регулярка в RegVect
						Пропустить выделенную лексему (i+=длина лексемы)
					}
					*/
					if (*i == ' ') //разделитель; 0 -> 0
					{
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (*i == ProgBracket[0]) // открыающаяся программная скобка
					{
						auto j = i+1;
						for (; *j == ' ' || *j == '\t'; j++);
						if (j == str.end() || *j == '\n') // Конечная программная скобка в строке
							ProgLevel++;
					}
					if (*i == '"') //кавычка; 0 -> 4
					{
						LexAccum = ""; //буферная лексема
						S = 4; //переход в 4 состояние
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if ((*i == '+' || *i == '-') && (i==str.begin() ||
						(LexBuf[(ib + SizeBuf) % SizeBuf].atr == SeperatAtr) && *(i - 1) != ')') && *(i - 1) != '}') // символ (+,-); 0 -> 10
					{
						LexAccum = *i; //запись в буферную переменную
						S = 10; //переход в состояние 10
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (*i == '/') // символ (/); 0 -> 5
					{
						LexAccum = *i; //запись в буферную переменную
						S = 5; //переход в состояние 5
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (*i == '\n') //переход на новую строку; 0 -> 12
					{
						//LexAccum = *i; //запись в буферную переменную
						// TO DO tabCounter = 0 (реализация соответствующих действий)
						
						S = 12; //переход в состояние 12
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					auto SepUk = Seps.find(str.substr(distance(str.begin(), i), 1));
					auto SepUk2 = Seps.find(str.substr(distance(str.begin(), i), 2));
					auto SepUk3 = Seps.find(str.substr(distance(str.begin(), i), 3));
					if (SepUk != Seps.end() || SepUk2 != Seps.end() || SepUk3 != Seps.end())
					{
						ib = (ib + 1) % SizeBuf;
						LexBuf[ib].Load.Clear();
						string* tstr=new string;
						if (SepUk3 != Seps.end() && SepUk3->size()==3)
						{
							*tstr = *SepUk3;
							i += 2;
						}
						else if (SepUk2 != Seps.end() && SepUk2->size() == 2)
						{
							*tstr = *SepUk2;
							i += 1;
						}
						else if(SepUk != Seps.end())
							*tstr = *SepUk;

						LexBuf[ib] = { SeperatAtr,Cstring , tstr };
						LexOut();
						break;
					}
					else

					if ((Digit.count(*i)) && (*i != '0')) // --- цифра (1..9); 0 -> 1
					{
						LexAccum = *i; //запись в буферную переменную
						S = 1; //переход в состояние 1
						//Debug(*i, S, LexAccum); // --- отла/дка
						//TO DO numberFormat = DEC (реализация соответствующих действий)
						break;
					}
					if (ABC.count(*i)) //буква; 0 -> 3
					{
						LexAccum = *i; //запись в буферную переменную
						S = 3; //переход в состояние 3
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (*i == '0') // цифра (0); 0 -> 9
					{
						LexAccum = *i; //запись в буферную переменную
						S = 9; //переход в состояние 9
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (DigitSeps.count(*i))  // --- символ разделения дробной и целой части (. ,); 0 -> 2
					{
						LexAccum = '0'; // --- добавление символа 0 в буферную переменную, так как .5 = 0.5; ,5 = 0,5
						LexAccum += *i; //добавление символа в буферную переменную
						//TO DO numberFormat = DEC (реализация соответствующих действий)
						S = 2; //переход в состояние 2
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					Work = false; //установка флага рабочего режима лексера на false
					if (ErrProg != nullptr) ProgExec(ErrProg, 0, Bus, nullptr); //обработка ошибки
					break;
				}
				// Обработка целой части
				case 1: 
					if (Digit.count(*i)) // --- число (0..9); 1 -> 1
					{
						LexAccum += *i; //добавление символа в буферную переменную
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (*i == '_') //символ разделения чисел (_); 1 -> 1
					{
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (DigitSeps.count(*i))  // --- символ разделения дробной и целой части (. ,); 1 -> 2
					{
						LexAccum += *i; //добавление символа в буферную переменную
						S = 2; //переход в состояние 2
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (Seps.count(str.substr(distance(str.begin(), i), 1)) || *i == ' ') //разделитель; 1 -> 0
					{
						i--; // Для обработки сепаратора
						int  *tint = new int;
						*tint = atoi(LexAccum.c_str()); //запись лексемы в переменную
						ib = (ib + 1) % SizeBuf; //увеличение текущего адреса буфера выходных лексем на 1
						LexBuf[ib].Load.Clear(); //удаление нагрузки ИП
						LexBuf[ib] = { IntAtr,Cint , tint }; //добавление лексемы в буфер выходных лексем в виде ИП {атрибут, тип, указатель}
						LexOut();
						S = 0; //переход в состояние 0
						break;
					}
					Work = false; //установка флага рабочего режима лексера на false
					ProgExec(ErrProg, 0, Bus, nullptr); //обработчик ошибок
					break;
				// Обработка дробной части
				case 2:
					if (Digit.count(*i))  // --- число (0..9); 2 -> 2
					{
						LexAccum += *i; //добавление символа в буферную переменную
					//	Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (*i == '_') //символ разделения чисел (_); 2 -> 2
					{
					//	Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (Seps.count(str.substr(distance(str.begin(), i), 1)) || *i == ' ') //разделитель; 2 -> 0
					{
						i--; // Для обработки сепаратора
						double *ft = new double;
						*ft = atof(LexAccum.c_str()); //запись лексемы в переменную
						ib = (ib + 1) % SizeBuf; //увеличение текущего адреса буфера выходных лексем на 1
						LexBuf[ib].Load.Clear(); //удаление нагрузки ИП
						LexBuf[ib] = { DoubleAtr, Tdouble, ft }; //добавление лексемы в буфер выходных лексем в виде ИП {атрибут, тип, указатель}
						LexOut();
						S = 0; //переход в состояние 0
						break;
					}
					Work = false;  //установка флага рабочего режима лексера на false
					ProgExec(ErrProg, 0, Bus, nullptr); //обработчик ошибок
					break;
				//Обработка мнемоники
				case 3:
					if (Seps.count(str.substr(distance(str.begin(), i), 1)) || *i == ' ') //разделитель; 3 -> 0
					{
						i--; // Для обработки сепаратора
						if (find(TrueConst.begin(), TrueConst.end(), LexAccum) != TrueConst.end())
						{
							bool* t = new bool(true);
							ib = (ib + 1) % SizeBuf; //увеличение текущего адреса буфера выходных лексем на 1
							LexBuf[ib].Load.Clear(); //удаление нагрузки ИП
							LexBuf[ib] = { BoolAtr, Cbool, t };  //добавление лексемы в буфер выходных лексем в виде ИП {атрибут, тип, указатель}
							LexOut();
							S = 0;
							break;
						}
						if (find(FalseConst.begin(), FalseConst.end(), LexAccum) != FalseConst.end())
						{
							bool* t = new bool(false);
							ib = (ib + 1) % SizeBuf; //увеличение текущего адреса буфера выходных лексем на 1
							LexBuf[ib].Load.Clear(); //удаление нагрузки ИП
							LexBuf[ib] = { BoolAtr, Cbool, t };  //добавление лексемы в буфер выходных лексем в виде ИП {атрибут, тип, указатель}
							LexOut();
							S = 0;
							break;
						}
						string *st = new string;
						*st = LexAccum; //запись лексемы в переменную
						ib = (ib + 1) % SizeBuf; //увеличение текущего адреса буфера выходных лексем на 1
						LexBuf[ib].Load.Clear(); //удаление нагрузки ИП
						LexBuf[ib] = { MnemoAtr, Cstring, st };  //добавление лексемы в буфер выходных лексем в виде ИП {атрибут, тип, указатель}
						LexOut();
						S = 0; //переход в состояние 0
						break;
					}
					if (ABC.count(*i) || Digit.count(*i)) // --- буква либо число; 3 -> 3
					{
						LexAccum += *i; //добавление символа в буферную переменную
					//	Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					Work = false;  //установка флага рабочего режима лексера на false
					ProgExec(ErrProg, 0, Bus, nullptr); //обработчик ошибок
					break;
				//Обработка кавычек
				case 4:
					if (*i != '"' && *i != '\\') //любой символ кроме кавычки и \; 4 -> 4
					{
						LexAccum += *i;  //добавление символа в буферную переменную
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}		
					else 
						if (*i == '\\')//символ (\); 4 -> 8
						{
							S = 8; //переход в состояние 8
						//Debug(*i, S, LexAccum); // --- отладка
							break;
						}
						else 
							if (*i == '"')//кавычка; 4 -> 0
							{
								string *st2 = new string;
								*st2 = LexAccum; //запись лексемы в переменную
								ib = (ib + 1) % SizeBuf; //увеличение текущего адреса буфера выходных лексем на 1
								LexBuf[ib].Load.Clear(); //удаление нагрузки ИП
								LexBuf[ib] = { StrAtr, Cstring, st2 }; //добавление лексемы в буфер выходных лексем в виде ИП {атрибут, тип, указатель}
								LexOut();
								S = 0; //переход в состояние 0
							//	Debug(*i, S, LexAccum); // --- отладка
								break;
							}
					Work = false;  //установка флага рабочего режима лексера на false
					ProgExec(ErrProg, 0, Bus, nullptr); //обработчик ошибок
					break;
				//Обработка строчного комментария
				case 5:
					if (*i == '/') //символ (/); 5 -> 11
					{
						LexAccum += *i; //добавление символа в буферную переменную
						S = 11; //переход в состояние 11
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (*i == '*') //символ (*); 5 -> 6
					{
						LexAccum += *i; //добавление символа в буферную переменную
						S = 6; //переход в состояние 6
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					{//
						string* t=new string(LexAccum);
						ib = (ib + 1) % SizeBuf;
						LexBuf[ib].Load.Clear(); // Удаляем нагрузку ИП
						LexBuf[ib] = { SeperatAtr,Cstring , new string(*t) };
						LexOut();
						i--;
						S = 0;

						//					Work = false;  //установка флага рабочего режима лексера на false
						//					ProgExec(ErrProg, 0, Bus, nullptr); //обработчик ошибок
						break;
					}
				//Обработка многострочного комментария
				case 6:
					if (*i != '/') //любой символ кроме (/); 6 -> 6
					{
						LexAccum += *i;  //добавление символа в буферную переменную
					//	Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (*i == '/') //символ (/); 6 -> 7
					{
						LexAccum += *i; //добавление символа в буферную переменную
						S = 7; //переход в состояние 7
					//	Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					Work = false;  //установка флага рабочего режима лексера на false
					ProgExec(ErrProg, 0, Bus, nullptr); //обработчик ошибок
					break;
				//Обработка многострочного комментария
				case 7:
					if (*i != '*') //любой символ кроме (*); 7 -> 6
					{
						LexAccum += *i;  //добавление символа в буферную переменную
						S = 6; //переход в состояние 6
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					else //символ (*); 7 -> 0
					{
						string* multiLineComment = new string;
						*multiLineComment = LexAccum; //запись лексемы в переменную
						ib = (ib + 1) % SizeBuf; //увеличение текущего адреса буфера выходных лексем на 1
						LexBuf[ib].Load.Clear(); //удаление нагрузки ИП
						LexBuf[ib] = { StrAtr, Cstring, multiLineComment }; //добавление лексемы в буфер выходных лексем в виде ИП {атрибут, тип, указатель}
						LexOut();
						S = 0; //переход в состояние 0
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					Work = false;  //установка флага рабочего режима лексера на false
					ProgExec(ErrProg, 0, Bus, nullptr); //обработчик ошибок
					break;
				//Обработка экранирования
				case 8:
					if (*i == '"' || *i == 'n' || *i == '\'') //символы (", n, '); 8 -> 4
					{
						LexAccum += *i; //добавление символа в буферную переменную
						//TO DO Обработка экранирования (реализация соответствующих действий)
						S = 4; //переход в состояние 4
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					else //любой символ кроме (n, ", '); 8 -> 4
						{
							LexAccum += *i; //добавление символа в буферную переменную
							S = 4; //переход в состояние 4
							//Debug(*i, S, LexAccum); // --- отладка
							break;
						}
					Work = false;  //установка флага рабочего режима лексера на false
					ProgExec(ErrProg, 0, Bus, nullptr); //обработчик ошибок
					break;
				//Определитель системы счисления
									//Определитель системы счисления
				/*
				case 9:
					if (*i == '0') //число (0); 9 -> 1
					{
						S = 1; //переход в состояние 1
						LexAccum += *i; //добавление символа в буферную переменную
						//TO DO numberFormat = DEC (реализация соответствующих действий)
						break;
					}
					if (*i == 'x') //символ (x); 9 -> 1
					{
						S = 1; //переход в состояние 1
						LexAccum += *i; //добавление символа в буферную переменную
						//TO DO numberFormat = HEX (реализация соответствующих действий)
						break;
					}
					if (*i == 'b') //символ (b); 9 -> 1
					{
						S = 1; //переход в состояние 1
						LexAccum += *i; //добавление символа в буферную переменную
						//TO DO numberFormat = BIN (реализация соответствующих действий)
						break;
					}
					Work = false;  //установка флага рабочего режима лексера на false
					ProgExec(ErrProg); //обработчик ошибок
					break;

					*/
				case 9:
					if (*i == '_') //символ разделения чисел (_); 9 -> 9
					{
			//			Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (Digit.count(*i)) // --- число (0..9); 9 -> 1
					{
						LexAccum += *i; //добавление символа в буферную переменную
						//TO DO numberFormat = DEC (реализация соответствующих действий)
						S = 1; //переход в состояние 1
				//		Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (*i == 'b') //символ (b); 9 -> 13
					{
						//LexAccum += *i; //добавление символа в буферную переменную не нужно, так как b является обозначением BIN числа
						//TO DO numberFormat = BIN (реализация соответствующих действий)
						S = 13; //переход в состояние 13
				//		Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (*i == 'o') //символ (o); 9 -> 15
					{
						//LexAccum += *i; //добавление символа в буферную переменную не нужно, так как o является обозначением OCT числа
						//TO DO numberFormat = OCT (реализация соответствующих действий)
						S = 15; //переход в состояние 15
				//		Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (*i == 'x') //символ (x); 9 -> 17
					{
						//LexAccum += *i; //добавление символа в буферную переменную не нужно, так как x является обозначением HEX числа
						//TO DO numberFormat = HEX (реализация соответствующих действий)
						S = 17; //переход в состояние 17
				//		Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (DigitSeps.count(*i))  // --- символ разделения дробной и целой части (. ,); 9 -> 2
					{
						LexAccum += "."; //добавление символа в буферную переменную
						//TO DO numberFormat = DEC (реализация соответствующих действий)
						S = 2; //переход в состояние 2
				//		Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					i--;
					S = 1;
					break;
					Work = false;  //установка флага рабочего режима лексера на false
					ProgExec(ErrProg); //обработчик ошибок
					break;
					
					//Обработка АЛВ
				case 10:
					if ((Digit.count(*i)) && (*i != '0')) // --- число (1..9); 10 -> 1
					{
						LexAccum += *i; //добавление символа в буферную переменную
						//TO DO numberFormat = DEC (реализация соответствующих действий)
						S = 1; //переход в состояние 1
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (*i == '0') //число 0; 10 -> 9
					{
						LexAccum += *i; //добавление символа в буферную переменную
						S = 9; //переход в состояние 9
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (Seps.count(str.substr(distance(str.begin(), i), 1)) || *i == ' ' || //разделитель; 10 -> 0
					  ABC.count(*i));
					{
						i--; // Для обработки сепаратора
						string* sep = new string;
						*sep = LexAccum; //запись лексемы в переменную
						ib = (ib + 1) % SizeBuf; //увеличение текущего адреса буфера выходных лексем на 1
						LexBuf[ib].Load.Clear(); //удаление нагрузки ИП
						LexBuf[ib] = { SeperatAtr, Cstring, sep };  //добавление лексемы в буфер выходных лексем в виде ИП {атрибут, тип, указатель}
						LexOut();
						S = 0; //переход в состояние 0
						break;
					}
					Work = false;  //установка флага рабочего режима лексера на false
					ProgExec(ErrProg, 0, Bus, nullptr); //обработчик ошибок
					break;
				//Обработка строчного комментария
				case 11:
					if (*i != '\n') //любой символ кроме (\n); 11 -> 11
					{
						LexAccum += *i;  //добавление символа в буферную переменную
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (*i == '\n') //символ (\n); 11 -> 12
					{
						LexAccum += *i; //добавление символа в буферную переменную
						// TO DO tabCounter = 0 (реализация соответствующих действий)
						S = 12; //переход в состояние 12
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					Work = false; //установка флага рабочего режима лексера на false
					ProgExec(ErrProg, 0, Bus, nullptr); //обработчик ошибок
					break;
				//Обработка табуляции
				case 12:
					if (*i != '\n')//любой символ кроме (\n); 12 -> 0
					{
						/*
						TO DO
						If (bracketAmount > 0)
						Замена табуляции на кол-во { равное bracketAmount

						If (bracketAmount < 0)
						Замена табуляции на кол-во } равное bracketAmount
						*/
						i--; // Для обработки сепаратора
						string* st3 = new string;
						*st3 = LexAccum; //запись лексемы в переменную
						ib = (ib + 1) % SizeBuf; //увеличение текущего адреса буфера выходных лексем на 1
						LexBuf[ib].Load.Clear(); //удаление нагрузки ИП
						LexBuf[ib] = { StrAtr, Cstring, st3 }; //добавление лексемы в буфер выходных лексем в виде ИП {атрибут, тип, указатель}
						LexOut();
						S = 0; //переход в состояние 0
					//	Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (*i == '\n') //символ (\n); 12 -> 12
					{
					//	LexAccum += *i; //добавление символа в буферную переменную
						// TO DO tabCounter = 0 (реализация соответствующих действий)
					//	Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (*i == '\t') //символ (\n); 12 -> 12
					{
						LexAccum += *i; //добавление символа в буферную переменную
						// TO DO tabCounter++ (реализация соответствующих действий)
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					Work = false; //установка флага рабочего режима лексера на false
					ProgExec(ErrProg, 0, Bus, nullptr); //обработчик ошибок
					break;
				case 13:
					if (DigitBIN.count(*i)) // --- число (0..1); 13 -> 13
					{
						LexAccum += *i; //добавление символа в буферную переменную
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (*i == '_') //символ разделения чисел (_); 13 -> 13
					{
					//	Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (DigitSeps.count(*i))  // --- символ разделения дробной и целой части (. ,); 13 -> 14
					{
						LexAccum += "."; //добавление символа в буферную переменную
						S = 2; //переход в состояние 2
					//	Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (Seps.count(str.substr(distance(str.begin(), i), 1)) || *i == ' ') //разделитель; 13 -> 0
					{
						i--; // Для обработки сепаратора
						int* tint = new int;
						*tint = atoi(LexAccum.c_str()); //запись лексемы в переменную
						ib = (ib + 1) % SizeBuf; //увеличение текущего адреса буфера выходных лексем на 1
						LexBuf[ib].Load.Clear(); //удаление нагрузки ИП
						LexBuf[ib] = { IntAtr, Cint , tint }; //добавление лексемы в буфер выходных лексем в виде ИП {атрибут, тип, указатель}
						S = 0; //переход в состояние 0

					//	Debug(*i, S, LexAccum); // --- отладка
						std::cout << "LexBuf[" << ib << "] = {IntAtr, TBinInt, " << *tint << "}\n"; // --- отладка

						break;
					}
					Work = false; //установка флага рабочего режима лексера на false
					ProgExec(ErrProg); //обработчик ошибок
					break;
					// Обработка дробной части двоичных чисел
				case 14:
					if (DigitBIN.count(*i))  // --- число (0..1); 14 -> 14
					{
						LexAccum += *i; //добавление символа в буферную переменную
					//	Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (*i == '_') //символ разделения чисел (_); 14 -> 14
					{
					//	Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (Seps.count(str.substr(distance(str.begin(), i), 1)) || *i == ' ') //разделитель; 14 -> 0
					{
						i--; // Для обработки сепаратора
						double* ft = new double;
						*ft = atof(LexAccum.c_str()); //запись лексемы в переменную
						ib = (ib + 1) % SizeBuf; //увеличение текущего адреса буфера выходных лексем на 1
						LexBuf[ib].Load.Clear(); //удаление нагрузки ИП
						LexBuf[ib] = { DoubleAtr, Cdouble, ft }; //добавление лексемы в буфер выходных лексем в виде ИП {атрибут, тип, указатель}
						LexOut();
						S = 0; //переход в состояние 0

					//	Debug(*i, S, LexAccum); // --- отладка
						std::cout << "LexBuf[" << ib << "] = {DoubleAtr, TBinDouble, " << *ft << "}\n"; // --- отладка

						break;
					}
					Work = false;  //установка флага рабочего режима лексера на false
					ProgExec(ErrProg); //обработчик ошибок
					break;
					// Обработка целой части восьмеричных чисел
				case 15:
					if (DigitOCT.count(*i)) // --- число (0..7); 15 -> 15
					{
						LexAccum += *i; //добавление символа в буферную переменную
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (*i == '_') //символ разделения чисел (_); 15 -> 15
					{
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (DigitSeps.count(*i))  // --- символ разделения дробной и целой части (. ,); 15 -> 16
					{
						LexAccum += "."; //добавление символа в буферную переменную
						S = 2; //переход в состояние 2
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (Seps.count(str.substr(distance(str.begin(), i), 1)) || *i == ' ') //разделитель; 15 -> 0
					{
						i--; // Для обработки сепаратора
						int* tint = new int;
						*tint = atoi(LexAccum.c_str()); //запись лексемы в переменную
						ib = (ib + 1) % SizeBuf; //увеличение текущего адреса буфера выходных лексем на 1
						LexBuf[ib].Load.Clear(); //удаление нагрузки ИП
						LexBuf[ib] = { IntAtr, Cint, tint }; //добавление лексемы в буфер выходных лексем в виде ИП {атрибут, тип, указатель}
						S = 0; //переход в состояние 0

						//Debug(*i, S, LexAccum); // --- отладка
						std::cout << "LexBuf[" << ib << "] = {IntAtr, TOctInt, " << *tint << "}\n"; // --- отладка

						break;
					}
					Work = false; //установка флага рабочего режима лексера на false
					ProgExec(ErrProg); //обработчик ошибок
					break;
					// Обработка дробной части восьмеричных чисел
				case 16:
					if (DigitOCT.count(*i))  // --- число (0..7); 16 -> 16
					{
						LexAccum += *i; //добавление символа в буферную переменную
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (*i == '_') //символ разделения чисел (_); 16 -> 16
					{
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (Seps.count(str.substr(distance(str.begin(), i), 1)) || *i == ' ') //разделитель; 16 -> 0
					{
						i--; // Для обработки сепаратора
						double* ft = new double;
						*ft = atof(LexAccum.c_str()); //запись лексемы в переменную
						ib = (ib + 1) % SizeBuf; //увеличение текущего адреса буфера выходных лексем на 1
						LexBuf[ib].Load.Clear(); //удаление нагрузки ИП
						LexBuf[ib] = { DoubleAtr, Cdouble, ft }; //добавление лексемы в буфер выходных лексем в виде ИП {атрибут, тип, указатель}
						LexOut();
						S = 0; //переход в состояние 0

						//Debug(*i, S, LexAccum); // --- отладка
						std::cout << "LexBuf[" << ib << "] = {DoubleAtr, TOctDouble, " << *ft << "}\n"; // --- отладка

						break;
					}
					Work = false;  //установка флага рабочего режима лексера на false
					ProgExec(ErrProg); //обработчик ошибок
					break;
					// Обработка целой части шестнадцатеричных чисел
				case 17:
					if (DigitHEX.count(*i)) // --- число (0..9, A..F, a..f); 17 -> 17
					{
						LexAccum += *i; //добавление символа в буферную переменную
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (*i == '_') //символ разделения чисел (_); 17 -> 17
					{
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (DigitSeps.count(*i))  // --- символ разделения дробной и целой части (. ,); 17 -> 2
					{
						LexAccum += "."; //добавление символа в буферную переменную
						S = 2; //переход в состояние 2
					//	Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (Seps.count(str.substr(distance(str.begin(), i), 1)) || *i == ' ') //разделитель; 17 -> 0
					{
						i--; // Для обработки сепаратора
						string* st2 = new string;
						*st2 = LexAccum; //запись лексемы в переменную
						ib = (ib + 1) % SizeBuf; //увеличение текущего адреса буфера выходных лексем на 1
						LexBuf[ib].Load.Clear(); //удаление нагрузки ИП
						LexBuf[ib] = { IntAtr, Cint, st2 }; //добавление лексемы в буфер выходных лексем в виде ИП {атрибут, тип, указатель}
						S = 0; //переход в состояние 0

						//Debug(*i, S, LexAccum); // --- отладка
						std::cout << "LexBuf[" << ib << "] = {IntAtr, THexInt, " << *st2 << "}\n"; // --- отладка

						break;
					}
					Work = false; //установка флага рабочего режима лексера на false
					ProgExec(ErrProg); //обработчик ошибок
					break;
					// Обработка дробной части шестнадцатеричных чисел
				case 18:
					if (DigitHEX.count(*i))  // --- число (0..9, A..F, a..f); 18 -> 18
					{
						LexAccum += *i; //добавление символа в буферную переменную
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (*i == '_') //символ разделения чисел (_); 18 -> 18
					{
						//Debug(*i, S, LexAccum); // --- отладка
						break;
					}
					if (Seps.count(str.substr(distance(str.begin(), i), 1)) || *i == ' ') //разделитель; 18 -> 0
					{
						i--; // Для обработки сепаратора
						string* st2 = new string;
						*st2 = LexAccum; //запись лексемы в переменную
						ib = (ib + 1) % SizeBuf; //увеличение текущего адреса буфера выходных лексем на 1
						LexBuf[ib].Load.Clear(); //удаление нагрузки ИП
						LexBuf[ib] = { DoubleAtr, Cdouble, st2 }; //добавление лексемы в буфер выходных лексем в виде ИП {атрибут, тип, указатель}
						S = 0; //переход в состояние 0

						//Debug(*i, S, LexAccum); // --- отладка
						std::cout << "LexBuf[" << ib << "] = {DoubleAtr, THexDouble, " << *st2 << "}\n"; // --- отладка

						break;
					}
					Work = false;  //установка флага рабочего режима лексера на false
					ProgExec(ErrProg); //обработчик ошибок
					break;
				default:
					break;
				}
		}
		ProgExec(FinProg); // Выполнить 
		break;
		default:
			CommonMk(MK, Load, Sender);
		}
	}
/*
	void Lex::Debug(char i, int S, string LexAccum) // --- для отладки, позже удалить
	{
		cout << " Current symbol: " << i << "; "; // --- отладка
		cout << "New condition: " << S << "; "; // --- отладка
		cout << " LexAccum: " << LexAccum << endl; // --- отладка
	}
*/
	Lex::Lex(FU *BusContext, FU *Templ) 
	{
		FUtype = 3;
		Bus = BusContext;
		Receiver.back() = BusContext;
		copy(ABC_templ.begin(), ABC_templ.end(), inserter(ABC, ABC.end()));
		copy(Digit_templ.begin(), Digit_templ.end(), inserter(Digit, Digit.end())); // --- Добавление в множество чисел
		copy(DigitHEX_templ.begin(), DigitHEX_templ.end(), inserter(DigitHEX, DigitHEX.end())); // --- Добавление в множество чисел
		copy(DigitOCT_templ.begin(), DigitOCT_templ.end(), inserter(DigitOCT, DigitOCT.end())); // --- Добавление в множество чисел
		copy(Digit_seps_templ.begin(), Digit_seps_templ.end(), inserter(DigitSeps, DigitSeps.end())); // --- Добавление в множество разделителей дробных и целых частей чисел
		copy(Seps_templ.begin(), Seps_templ.end(), inserter(Seps, Seps.end()));
		DigitBIN.insert('0'); DigitBIN.insert('1');
		LexBuf = new ip[SizeBuf];
		for (int i = 0; i < SizeBuf; LexBuf[i++].Load = { 0,nullptr });
		LexBuf[0].atr = SeperatAtr;
		ProgFU(0, {0,nullptr});
	}

	FU* Lex::Copy() // Программа копирования ФУ
	{
		return new Lex(Bus, this);
	}

	FU* Lex::TypeCopy() // Создать ФУ такого же типа (не копируя контекст
	{
		return new Lex(Bus, nullptr);
	}
