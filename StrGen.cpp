#include "stdafx.h"
#include "StrGen.h"
#include <ctime>
#include <string>
#include <iostream>
using namespace std;

void StrGen::ProgFU(long int MK, LoadPoint Load, FU* Sender)
{
	MK %= FUMkRange;
	switch (MK)
	{
	case 0: //Reset
//		str_buf.clear();
//		str_buf.push_back("");
		Receiver = nullptr;
		ReceiverMK = 0;
		str_buf_size = 1;
		str_buf.resize(str_buf_size);
		LineCount = 0;
		//Source.close();
		Receiver = Bus;
		str_bufCount = 0;
		break;
	case 20: //ReceiverSet Установить ссылку на приемник строк
		Receiver = Load.Point;
		break;
	case 21: //ReceiverMkSet Установить МК для приемника строк
		if (Load.Type >> 1 == Dint) ReceiverMK = Load.toInt(); break;
	case 1: // SourceSet
		break;
	case 2: // SourceSetStart
		break;
	case 3: // Start Начать генерацию
	{
		ifstream Source;
		work = true;
		ProgExec(StartProg);// Выполнить стартовую программу
		TimeStart = clock();
		TimeLong = 0;
		if (Load.Point != nullptr)
		{
			Filename = Load.toStr();
			Source.open(Filename);
			if (Source.fail())
			{
				ProgExec(FileNotExistErrProg);
				break;
			}
			LineCount = 0;
		}
		while (Source && work)
		{
			string str;
			getline(Source, str, '\n');
			LineCount++;
			if (str.find(IncludeStr) == 0) // Включение нового модуля
			{
				str = str.substr(IncludeStr.length(), 10000000);
				while (str[0] == ' ')
					str = str.substr(1, 10000000);
				ProgFU(3, { Cstring, &str }, this);
			}
			if (str == "") continue;
			str_bufCount = (str_bufCount + 1) % str_buf_size;
			str_buf[str_bufCount] = str;

			LoadPoint Point;
			Point.Type = Cstring;
			Point.Point = &str;

			((FU*)Receiver)->ProgFU(ReceiverMK, Point, this);
		}
/*		if (work) // выдача завершающей лексемы
		{
			LoadPoint Point;
			Point.Type = 2;
			Point.Point = &finStr;
			((FU*)Receiver)->ProgFU(ReceiverMK, Point, this);
		}
*/
		Source.close();
		TimeLong = clock() - TimeStart;
		ProgExec(FinProg);// Выполнить завершающую программу
		break;
	}
	case 4: // Stop	
		work = false;
		ProgExec(StopProg);// Выполнить программу по прерыванию генерации строк
		break;
	case 5: // Pause
		work = false;
		break;
	case 6: // Continue ???
	{work = true;
	ProgFU(3, { 0, nullptr }); }
	break;
	case 7: // FileNotExistErrProgSet Установить программу обработки ошибки 'Файл не существует'
		FileNotExistErrProg = Load.Point;
		break;
	case 9: // ModeSet
		break;
	case 10: // SourceReset
		break;
	case 11: // LogMkOut
		break;

	case 15: // TimeOut	Выдать время компиляции в секундах
		Load.Write(TimeLong);
		break;
	case 30: // TimeMkOut Выдать МК со времем компиляции в секундах
	{
		double t = TimeLong / CLOCKS_PER_SEC;
		MkExec(Load, { Cdouble , &t });
		break;
	}
	case 35: // LineBufSizeSet Установить размер буфера строк
		str_buf_size =Load.toInt();
		str_bufCount = 0;
		str_buf.resize(str_buf_size);
		break;
	case 40: // LogOut
		break;
	case 50: // LineCountOutMk
		MkExec(Load, { Tint, &LineCount });
		break;
	case 51: // LastLineBufOutMk Выдать МК с последней строкой из буфера
		MkExec(Load, { Tstring, &str_buf[str_bufCount] });
		break;
	case 52: // LineBufOutMk // Выдать МК со строками из буфера
		for (int i = 0; i < str_buf_size; i++)
		{
			MkExec(Load, { Tstring, &str_buf[str_bufCount] });
			str_bufCount = (str_bufCount + 1) % str_buf_size;
		}
		break;
	case 55: //IncludeStrSet Установить строку для включения модуля в текст программы
		IncludeStr = Load.toStr();
		break;
	case 60: // StartProgSet // Установить указатель на программу, выполняемую при запуске нерации строк из файла
		StartProg = Load.Point;
		break;
	case 61: // FinProgSet // Установить указатель на программу, выполняемую по завершению генерации строк из файла
		FinProg = Load.Point;
		break;
	case 62: // StopProgSet // Установить указатель на программу, выполняемую при прерывании генерации строк из файла
		StopProg = Load.Point;
		break;
	default:
		CommonMk(MK, Load, Sender);
		break;
	}
}

FU* StrGen::Copy() // Программа копирования ФУ
{
	return new StrGen(Bus, this);
}

FU* StrGen::TypeCopy() // Создать ФУ такого же типа (не копируя контекст)
{
	return new StrGen(Bus, nullptr);
}
