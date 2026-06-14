#include "stdafx.h"
#include "Consts.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include "LocationTable.h"
#include "ALU.h"
#include "IntAlu.h"
#include "FUini.h"

using namespace std;

unsigned int LoadPoint::getType() // Выдать тип нагрузки
{
	if (Type >> 1 == DLoadVectInd)
		return ((LoadVect_type)Point)->at(Ind).Type;
	if (Type >> 1 != DICInd)
		return Type;
	else
		switch (Ind % 3) {
		case 0: return TIP;
		case 1: return Tint;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return -1; return ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
}

long int LoadPoint::DataSize() // Выдать размер данных в нагрузке
{
	LoadPoint LP = *this;
	while (LP.Type >> 1 == DLoadVectInd || LP.Type >> 1 == DICInd)
	{
		if (LP.Type >> 1 == DLoadVectInd)
			LP = ((LoadVect_type)Point)->at(LP.Ind);
		else
			if (LP.Ind < 0 || LP.Ind / 3 >= ((IC_type)LP.Point)->size()) return 2; // Ошибка индекса
			else switch (LP.Ind % 3) {
			case 0: LP = { TIP, &((IC_type)LP.Point)->at(Ind / 3) }; break;
			case 1: LP = { Tint, &((IC_type)LP.Point)->at(Ind / 3).atr }; break;
			case 2: if (((IC_type)LP.Point)->at(Ind / 3).Load.Point == nullptr) return 1; LP = ((IC_type)LP.Point)->at(Ind / 3).Load;
			}
	}
	switch (Type >> 1)
	{
	case Dbool: return 1;
	case Dchar: return 2;
	case TMk:
	case Dint:
	case Dfloat: return 4;
	case Ddouble: return 8;
	case DFU: return SizeOfFUType(((FU*)LP.Point)->FUtype);
	case DIP: return 4 + ((ip*)LP.Point)->Load.DataSize();
	case DLoad: return sizeof(LoadPoint);
	case Dstring: return sizeof(string) + ((string*)LP.Point)->length();
	case DLoadVect: {
		register int s = 0, t = sizeof(LoadPoint);
		for (auto& i : *((LoadVect_type)LP.Point))
			s += t + i.DataSize();
	}
	case TCalc:
	case TProg:
	case TGraph:
	case DIC: {
		register int s = 0, t = sizeof(ip);
		for (auto& i : *((IC_type)LP.Point))
			s += t + i.Load.DataSize();
	}
	}
	return 0;
}

bool LoadPoint::isDigit() {
	if (Point == nullptr) return false;
	register unsigned int t = Type;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return false;
		t = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Type;
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return false;
		else switch (Ind % 3) {
		case 0: return false;
		case 1: return true;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return false; t = ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
	return t >> 1 == Dint || t >> 1 == Dchar || t >> 1 == Dfloat || t >> 1 == Ddouble;
}; // Число?}

bool LoadPoint::isDigitBool() {
	if (Point == nullptr) return false;
	register unsigned int t = Type;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return false;
		t = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Type;
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return false;
		else switch (Ind % 3) {
		case 0: return false;
		case 1: return true;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return false; t = ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
	return t >> 1 == Dint || t >> 1 == Dchar || t >> 1 == Dfloat || t >> 1 == Ddouble || t >> 1 == Dbool;
}; // Число или булеан?

bool LoadPoint::isBool() {
	if (Point == nullptr) return false;
	register unsigned int t = Type;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return false;
		t = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Type;
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return false;
		else switch (Ind % 3) {
		case 0: return false;
		case 1: return false;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return false; t = ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
	return t >> 1 == Dbool;
}; // булеан?

bool LoadPoint::isInt() {
	if (Point == nullptr) return false;
	register unsigned int t = Type;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return false;
		t = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Type;
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return false;
		else switch (Ind % 3) {
		case 0: return false;
		case 1: return true;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return false; t = ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
	return t >> 1 == Dint || t>>1 == DMk;
}; // Целое число?

bool LoadPoint::isIntBool() {
	if (Point == nullptr) return false;
	register unsigned int t = Type;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return false;
		t = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Type;
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return false;
		else switch (Ind % 3) {
		case 0: return false;
		case 1: return true;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return false; t = ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
	return t >> 1 == Dint || t >> 1 == Dbool;
}; // булеан или целое число?

bool LoadPoint::isFloatDouble() {
	if (Point == nullptr) return false;
	register unsigned int t = Type;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return false;
		t = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Type;
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return false;
		else switch (Ind % 3) {
		case 0: return false;
		case 1: return false;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return false; t = ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
	return t >> 1 == Ddouble || t >> 1 == Dfloat;
};

bool LoadPoint::isFloat() {
	if (Point == nullptr) return false;
	register unsigned int t = Type;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return false;
		t = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Type;
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return false;
		else switch (Ind % 3) {
		case 0: return false;
		case 1: return false;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return false; t = ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
	return t >> 1 == Dfloat;
};

bool LoadPoint::isDouble() {
	if (Point == nullptr) return false;
	register unsigned int t = Type;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return false;
		t = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Type;
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return false;
		else switch (Ind % 3) {
		case 0: return false;
		case 1: return false;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return false; t = ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
	return t >> 1 == Ddouble;
};

bool LoadPoint::isProg() {
	if (Point == nullptr) return false;
	register unsigned int t = Type;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return false;
		t = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Type;
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return false;
		else switch (Ind % 3) {
		case 0: return false;
		case 1: return false;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return false; t = ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
	return t >> 1 == DIC || t >> 1 == DCalc || t >> 1 == DProg;
}; // Определение может ли быть нагрузка программой

bool LoadPoint::isStrChar() {
	if (Point == nullptr) return false;
	register unsigned int t = Type;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return false;
		t = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Type;
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return false;
		else switch (Ind % 3) {
		case 0: return false;
		case 1: return false;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return false; t = ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
	return Point != nullptr && (t >> 1 == Dstring || t >> 1 == Dchar);
}; // Строка или символ?

bool LoadPoint::isStr() {
	if (Point == nullptr) return false;
	register unsigned int t = Type;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return false;
		t = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Type;
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return false;
		else switch (Ind % 3) {
		case 0: return false;
		case 1: return false;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return false; t = ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
	return Point != nullptr && t >> 1 == Dstring;
}; // Строка?

bool LoadPoint::isChar() {
	if (Point == nullptr) return false;
	register unsigned int t = Type;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return false;
		t = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Type;
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return false;
		else switch (Ind % 3) {
		case 0: return false;
		case 1: return false;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return false; t = ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
	return Point != nullptr && t >> 1 == Dchar;
}; // символ?

bool LoadPoint::isMk() {
	if (Point == nullptr) return false;
	register unsigned int t = Type;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return false;
		t = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Type;
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return false;
		else switch (Ind % 3) {
		case 0: return false;
		case 1: return (Ind / 3 >= ((IC_type)Point)->at(Ind / 3).atr >= 0) ? true : false; // Если атрибут неотрицательный, то он считается милликомандой
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return false; t = ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
	return Point != nullptr && t >> 1 == DMk;
}; // Милликоманда?

bool LoadPoint::isAtr() { // Атрибут или милликоманда?
	if (Point == nullptr) return false;
	register unsigned int t = Type;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return false;
		t = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Type;
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return false;
		else switch (Ind % 3) {
		case 0: return false;
		case 1: return (Ind / 3 >= ((IC_type)Point)->at(Ind / 3).atr >= 0) ? true : false; // Если атрибут неотрицательный, то он считается милликомандой
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return false; t = ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
	return Point != nullptr && (t >> 1 == DAtr);
}; // Милликоманда?

bool LoadPoint::isAtrMk() { // Атрибут или милликоманда?
	if (Point == nullptr) return false;
	register unsigned int t = Type;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return false;
		t = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Type;
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return false;
		else switch (Ind % 3) {
		case 0: return false;
		case 1: return (Ind / 3 >= ((IC_type)Point)->at(Ind / 3).atr >= 0) ? true : false; // Если атрибут неотрицательный, то он считается милликомандой
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return false; t = ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
	return Point != nullptr && (t >> 1 == DMk || t >> 1 == DAtr);
}; // Милликоманда?


bool LoadPoint::isVect() {
	if (Point == nullptr) return false;
	register unsigned int t = Type;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return false;
		t = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Type;
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return false;
		else switch (Ind % 3) {
		case 0: return false;
		case 1: return false;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return false; t = ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
	return t >> 1 == DLoadVect;
}; // Вектор ?

bool LoadPoint::isVectIndVectInd()  // Индексированный элемент вектора нагрузок от индексированного вектора нагрузок
{
	if (Point == nullptr) return false;
	register unsigned int t = Type;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return false;
		t = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Type;
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return false;
		else switch (Ind % 3) {
		case 0: return false;
		case 1: return false;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return false; t = ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
	return (t >> 1) == DLoadVectInd;
}

bool LoadPoint::isEmptyIC() // Определить указывает ли ссылка на пустую ИК
{
	if (!isIC()) return false;
	return ((IC_type)Point)->size() == 0;
}

bool LoadPoint::isIC() // Определить указывает ли ссылка на ИК
{
	if (Point == nullptr) return false;
	register unsigned int t = Type;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return false;
		t = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Type;
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return false;
		else switch (Ind % 3) {
		case 0: return false;
		case 1: return false;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return false; t = ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
	set<unsigned int> IC_Types = { DIC, DCalc, DProg }; // Множевство типов ИК и ОА-графов
	return IC_Types.count(t >> 1);
}

bool LoadPoint::isIP() // Определить указывает ли ссылка на ИП
{
	if (Point == nullptr) return false;
	register unsigned int t = Type;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return false;
		t = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Type;
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return false;
		else switch (Ind % 3) {
		case 0: return true;
		case 1: return false;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return false; t = ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
	set<unsigned int> IP_Types = { DIP }; // Множевство типов ИК и ОА-графов
	return IP_Types.count(t >> 1);
}

bool LoadPoint::isCalc() // Определить указывает ли ссылка на ИК с АЛВ
{
	if (Point == nullptr) return false;
	register unsigned int t = Type;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return false;
		t = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Type;
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return false;
		else switch (Ind % 3) {
		case 0: return false;
		case 1: return false;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return false; t = ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
	set<unsigned int> IP_Types = { DCalc }; // Множевство типов ИК и ОА-графов
	return IP_Types.count(t >> 1);
}

bool LoadPoint::isICSet() // Определить указывает ли ссылка на ИК с неупорядоченным множеством
{
	if (Point == nullptr) return false;
	register unsigned int t = Type;
	if (Type >> 1 == DLoadVect) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return false;
		t = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Type;
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return false;
		else switch (Ind % 3) {
		case 0: return false;
		case 1: return false;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return false; t = ((IC_type)Point)->at(Ind / 3).Load.Type;
		}
	set<unsigned int> IP_Types = { DIC }; // Множевство типов ИК и ОА-графов
	return IP_Types.count(t >> 1);
}

LoadPoint LoadPoint::IndLoadReturn() // Возвратить указатель на нагрузку индексированного элемента
{
	if (Point == nullptr)
		return *this;
	LoadPoint LP = *this;
	while (LP.isInd())
	{
		if (LP.Type >> 1 == DLoadVectInd) {
			register int N = ((LoadVect_type)LP.Point)->size();
			if (N <= LP.Ind || N < -LP.Ind || ((LoadVect_type)LP.Point)->at(N * (LP.Ind < 0) + LP.Ind).Point == nullptr) return {};
			LP = ((LoadVect_type)LP.Point)->at(N * (LP.Ind < 0) + LP.Ind);
		}
		else if (LP.Type >> 1 == DICInd)
			if (LP.Ind < 0 || LP.Ind / 3 >= ((IC_type)LP.Point)->size()) return {};
			else switch (LP.Ind % 3) {
			case 0: LP = { TIP, &((IC_type)LP.Point)->at(LP.Ind / 3) }; break;
//			case 1: LP = LP.IC()->at(LP.Ind / 3).Load;  break;
			case 1:LP = { Tint, &((IC_type)LP.Point)->at(LP.Ind / 3).atr }; break;
			case 2: LP = ((IC_type)LP.Point)->at(LP.Ind / 3).Load;  break;
			}
	}
	return LP;
}

LoadPoint LoadPoint::IndLoadReturn(LoadPoint LP) // Возвратить указатель на нагрузку индексированного элемента
{
	return LP.IndLoadReturn();
}

string LoadPoint::toStr(string define) // Перевод в bool
{
	LoadPoint LP = *this;
	while (LP.Type >> 1 == DLoad && LP.Point != nullptr) // follow DLoad references
		LP = *((LoadPoint*)LP.Point);
	if (LP.Point == nullptr)
		return define;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return "";
		LP = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind);
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return define;
		else switch (Ind % 3) {
		case 0: return define;
		case 1: return to_string(*(int*)((IC_type)Point)->at(Ind / 3).Load.Point);
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return ""; LP = ((IC_type)Point)->at(Ind / 3).Load;
		}
	switch (LP.Type >> 1) {
	case Dstring: return *((string*)LP.Point);
	case Ddouble: return to_string(*(double*)LP.Point); break;
	case Dfloat: return to_string(*(float*)LP.Point); break;
	case DMk:
	case Dint: return to_string(*(long int*)LP.Point); break;
	case Dbool: return to_string(*(bool*)LP.Point); break;
	default:
		return define;
		break;
	}
}

bool LoadPoint::toBool(bool define) // Перевод в bool (по умолчания false)
{
	LoadPoint LP = *this;
	while (LP.Type >> 1 == DLoad && LP.Point != nullptr) // follow DLoad references
		LP = *((LoadPoint*)LP.Point);
	if (LP.Point == nullptr)
		return define;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return define;
		LP = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind);
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return define;
		else switch (Ind % 3) {
		case 0: return define;
		case 1: return define;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return define; LP = ((IC_type)Point)->at(Ind / 3).Load;
		}
	while (LP.Type >> 1 == DLoad && LP.Point != nullptr) // follow DLoad references
		LP = *((LoadPoint*)LP.Point);
	switch (LP.Type >> 1)
	{
	case Ddouble: return *(double*)LP.Point;
	case Dfloat: return *(float*)LP.Point;
	case DMk:
	case Dint: return *(long int*)LP.Point;
	case Dbool: return *(bool*)LP.Point;
	case Dstring: return !((*(string*)LP.Point) == "");
	default:
		return define;
	}
}

long int LoadPoint::toInt(long int define) { // Перевод в integer
	LoadPoint LP = *this;
	while (LP.Type >> 1 == DLoad && LP.Point != nullptr) // follow DLoad references
		LP = *((LoadPoint*)LP.Point);
	if (LP.Point == nullptr)
		return define;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return define;
		LP = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind);
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return define;
		else switch (Ind % 3) {
		case 0: return define;
		case 1: return ((IC_type)Point)->at(Ind / 3).atr;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return define; LP = ((IC_type)Point)->at(Ind / 3).Load;
		}
	while (LP.Type >> 1 == DLoad && LP.Point != nullptr) // follow DLoad references
		LP = *((LoadPoint*)LP.Point);
	switch (LP.Type >> 1)
	{
	case Ddouble: return (long int)*(double*)LP.Point; break;
	case Dfloat: return (long int)*(float*)LP.Point; break;
	case DMk:
	case DAtr:
	case Dint: return *(long int*)LP.Point; break;
	case Dbool: return *(bool*)LP.Point; break;
	case Dstring: return atoi((*(string*)LP.Point).c_str());
	default:
		return define;
		break;
	}
}
double LoadPoint::toDouble(double define) {// Перевод в integer
	LoadPoint LP = *this;
	while (LP.Type >> 1 == DLoad && LP.Point != nullptr) // follow DLoad references
		LP = *((LoadPoint*)LP.Point);
	if (LP.Point == nullptr)
		return define;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return define;
		LP = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind);
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return define;
		else switch (Ind % 3) {
		case 0: return define;
		case 1: return ((IC_type)Point)->at(Ind / 3).atr;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return define; LP = ((IC_type)Point)->at(Ind / 3).Load;
		}
	while (LP.Type >> 1 == DLoad && LP.Point != nullptr) // follow DLoad references
		LP = *((LoadPoint*)LP.Point);
	switch (LP.Type >> 1)
	{
	case Ddouble: return *(double*)LP.Point; break;
	case Dfloat: return *(float*)LP.Point; break;
	case DMk:
	case Dint: return *(long int*)LP.Point; break;
	case Dbool: return *(bool*)LP.Point; break;
	case Dstring: return atof((*(string*)LP.Point).c_str());
	default:
		return define;
		break;
	}
}

float LoadPoint::toFloat(float define) {// Перевод в integer
	LoadPoint LP = *this;
	while (LP.Type >> 1 == DLoad && LP.Point != nullptr) // follow DLoad references
		LP = *((LoadPoint*)LP.Point);
	if (LP.Point == nullptr)
		return define;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return define;
		LP = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind);
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return define;
		else switch (Ind % 3) {
		case 0: return define;
		case 1: return ((IC_type)Point)->at(Ind / 3).atr;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return define; LP = ((IC_type)Point)->at(Ind / 3).Load;
		}
	switch (LP.Type >> 1)
	{
	case Ddouble: return *(double*)LP.Point; break;
	case Dfloat: return *(float*)LP.Point; break;
	case DMk:
	case Dint: return *(long int*)LP.Point; break;
	case Dbool: return *(bool*)LP.Point; break;
	case Dstring: return atof((*(string*)LP.Point).c_str());
	default:
		return define;
		break;
	}
};
// Запись в нагрузку ИП
int LoadPoint::Write(vector<double> x) //
{
	if (Point == nullptr)
		return 1;
	if (Type == CdoubleArray)
	{
		*(vector<double>*)Point = x;
		return 0;
	}
	else
		return 1;
}
int LoadPoint::Write(vector<float> x) //
{
	if (Point == nullptr)
		return 1;
	if (Type == CdoubleArray)
	{
		*(vector<float>*)Point = x;
		return 0;
	}
	else
		return 1;
}
int LoadPoint::Write(vector<bool> x) // Копирование вектора
{
	if (Point == nullptr)
		return 1;
	if (Type == CdoubleArray)
	{
		*(vector<bool>*)Point = x;
		return 0;
	}
	else
		return 1;
}
int LoadPoint::Write(vector<char> x) // Копирование вектора
{
	if (Point == nullptr)
		return 1;
	if (Type == CdoubleArray)
	{
		*(vector<char>*)Point = x;
		return 0;
	}
	else
		return 1;
}
int  LoadPoint::Write(vector<int> x) // Копирование вектора
{
	if (Point == nullptr)
		return 1;
	if (Type == CdoubleArray)
	{
		*(vector<int>*)Point = x;
		return 0;
	}
	else
		return 1;
}
// -----

int LoadPoint::WriteFromLoad(LoadPoint Load) // Записать величину из нагрузки
{
	if (Point == nullptr) return 1;
	LoadPoint LP = *this;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return 2; // Ошибка индекса
		LP = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind);
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return 2; // Ошибка индекса
		else switch (Ind % 3) {
		case 0: LP = { TIP, &((IC_type)Point)->at(Ind / 3) }; break;
		case 1: LP = { Tint, &((IC_type)Point)->at(Ind / 3).atr }; break;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return 1; LP = ((IC_type)Point)->at(Ind / 3).Load;
		}
	switch (LP.Type)
	{
	case Tdouble:
		*((double*)LP.Point) = Load.toDouble();
		break;
	case Tfloat:
		*((float*)LP.Point) = Load.toFloat();
		break;
	case Tint:
		*((int*)LP.Point) = Load.toInt();
		break;
	case Tbool:
		*((bool*)LP.Point) = Load.toBool();
		break;
	case Tchar:
		*((char*)LP.Point) = Load.toChar();
		break;
	case Tstring:
		*((string*)LP.Point) = Load.toStr();
		break;
	case TIP:
		//...
		break;
	case TIC:
		//...
		break;
	default: // Перезапись указателя
		if (Type % 2 != 0) // Если тип переменной
			return 2; // Ошибка: попытка запись в константу
		else
			if (Type >> 1 != Load.Type >> 1)
				return 1; // Ошибка: несоотвествие типов
			else
				Point = Load.Point;
	}
	return 0;
	/*
		if (Point == nullptr)
			return 1;
		register unsigned int t = Type;
		register void* P = Point;
		if (Ind >= 0)
			if (Type == TLoadVect) {
				if (Ind >= ((LoadVect_type)Point)->size()) return 2; // Ошибка индекса
				P = ((LoadVect_type)Point)->at(Ind).Point;
				t = ((LoadVect_type)Point)->at(Ind).Type;
			}
			else if (Type == TIC){
				if (Ind/3 >= ((IC_type)Point)->size()) return 2; // Ошибка индекса
				switch (Ind % 3) {
					case 0: P = &((IC_type)P)->at(Ind / 3); t = TIP; break;
					case 1: P = &((IC_type)P)->at(Ind / 3).atr; t = Tint; break;
					case 2: P = ((IC_type)P)->at(Ind / 3).Load.Point; t = ((IC_type)P)->at(Ind / 3).Load.Type;
				}
			}
			else return 2; // Ошибка индекса
		if (P == nullptr || t % 2 != 0) return 1;
		switch (t)
		{
		case Tdouble:
			*((double*)P) = Load.toDouble();
			break;
		case Tfloat:
			*((float*)P) = Load.toFloat();
			break;
		case Tint:
			*((int*)P) = Load.toInt();
			break;
		case Tbool:
			*((bool*)P) = Load.toBool();
			break;
		case Tchar:
			*((char*)P) = Load.toChar();
			break;
		case Tstring:
			*((string*)P) = Load.toStr();
			break;
		default: // Перезапись указателя
			if (Type % 2 != 0) // Если тип переменной
				return 2; // Ошибка: попытка запись в константу
			else
				if (Type >> 1 != Load.Type >> 1)
					return 1; // Ошибка: несоотвествие типов
				else
					Point = Load.Point;
		}
		*/
	return 0;
}

int LoadPoint::Write(size_t x)
{
	if (Point == nullptr) return 1;
	LoadPoint LP = *this;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return 2; // Ошибка индекса
		LP = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind);
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return 2; // Ошибка индекса
		else switch (Ind % 3) {
		case 0: LP = { TIP, &((IC_type)Point)->at(Ind / 3) }; break;
		case 1: LP = { Tint, &((IC_type)Point)->at(Ind / 3).atr }; break;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return 1; LP = ((IC_type)Point)->at(Ind / 3).Load;
		}
	switch (LP.Type)
	{
	case Tdouble:
		*((double*)LP.Point) = x;
		break;
	case Tfloat:
		*((float*)LP.Point) = x;
		break;
	case Tint:
		*((int*)LP.Point) = x;
		break;
	case Tbool:
		*((bool*)LP.Point) = x;
		break;
	case Tchar:
		*((char*)LP.Point) = x;
		break;
	case Tstring:
		*((string*)LP.Point) = to_string(x);
		break;

	default:
		return 2; // Несоответствие типов
	}
	return 0;
}

int LoadPoint::Write(long int x)
{
	if (Point == nullptr) return 1;
	LoadPoint LP = *this;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return 2; // Ошибка индекса
		LP = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind);
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return 2; // Ошибка индекса
		else switch (Ind % 3) {
		case 0: LP = { TIP, &((IC_type)Point)->at(Ind / 3) }; break;
		case 1: LP = { Tint, &((IC_type)Point)->at(Ind / 3).atr }; break;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return 1; LP = ((IC_type)Point)->at(Ind / 3).Load;
		}
	switch (LP.Type)
	{
	case Tdouble:
		*((double*)LP.Point) = x;
		break;
	case Tfloat:
		*((float*)LP.Point) = x;
		break;
	case Tint:
		*((long int*)LP.Point) = x;
		break;
	case Tbool:
		*((bool*)LP.Point) = x;
		break;
	case Tchar:
		if (x >= 0 && x < 256)
			*((char*)LP.Point) = x;
		else
			return 1; // Несоответствие типов
		break;
	case Tstring:
		*((string*)LP.Point) = to_string(x);
		break;
	default:
		return 1; // Несоответствие типов
	}
	return 0;
}

int LoadPoint::Write(int x)
{
	if (Point == nullptr) return 1;
	LoadPoint LP = *this;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return 2; // Ошибка индекса
		LP = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind);
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return 2; // Ошибка индекса
		else switch (Ind % 3) {
		case 0: LP = { TIP, &((IC_type)Point)->at(Ind / 3) }; break;
		case 1: LP = { Tint, &((IC_type)Point)->at(Ind / 3).atr }; break;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return 1; LP = ((IC_type)Point)->at(Ind / 3).Load;
		}
	switch (LP.Type)
	{
	case Tdouble:
		*((double*)LP.Point) = x;
		break;
	case Tfloat:
		*((float*)LP.Point) = x;
		break;
	case Tint:
		*((long int*)LP.Point) = x;
		break;
	case Tbool:
		*((bool*)LP.Point) = x;
		break;
	case Tchar:
		if (x >= 0 && x < 256)
			*((char*)LP.Point) = x;
		else
			return 1; // Несоответствие типов
		break;
	case Tstring:
		*((string*)LP.Point) = to_string(x);
		break;
	default:
		return 1; // Несоответствие типов
	}
	return 0;
}

int LoadPoint::Write(double x)
{
	if (Point == nullptr) return 1;
	LoadPoint LP = *this;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return 2; // Ошибка индекса
		LP = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind);
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return 2; // Ошибка индекса
		else switch (Ind % 3) {
		case 0: LP = { TIP, &((IC_type)Point)->at(Ind / 3) }; break;
		case 1: LP = { Tint, &((IC_type)Point)->at(Ind / 3).atr }; break;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return 1; LP = ((IC_type)Point)->at(Ind / 3).Load;
		}
	switch (LP.Type)
	{
	case Tdouble:
		*((double*)LP.Point) = x;
		break;
	case Tfloat:
		*((float*)LP.Point) = x;
		break;
	case Tint:
		*((int*)LP.Point) = x;
		break;
	case Tbool:
		*((bool*)LP.Point) = x;
		break;
	case Tchar:
		*((char*)LP.Point) = x;
		break;
	case Tstring:
		*((string*)LP.Point) = to_string(x);
		break;
	defoult:
		return 1; // Несоответствие типов
	}
	return 0;
}

int LoadPoint::Write(float x)
{
	if (Point == nullptr) return 1;
	LoadPoint LP = *this;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return 2; // Ошибка индекса
		LP = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind);
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return 2; // Ошибка индекса
		else switch (Ind % 3) {
		case 0: LP = { TIP, &((IC_type)Point)->at(Ind / 3) }; break;
		case 1: LP = { Tint, &((IC_type)Point)->at(Ind / 3).atr }; break;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return 1; LP = ((IC_type)Point)->at(Ind / 3).Load;
		}
	switch (LP.Type >> 1)
	{
	case Tdouble:
		*((double*)LP.Point) = x;
		break;
	case Tfloat:
		*((float*)LP.Point) = x;
		break;
	case Tint:
		*((int*)LP.Point) = x;
		break;
	case Tbool:
		*((bool*)LP.Point) = x;
		break;
	case Tchar:
		*((char*)LP.Point) = x;
		break;
	case Tstring:
		*((string*)LP.Point) = to_string(x);
		break;
	defoult:
		return 1; // Несоответствие типов
	}
	return 0;
}

int LoadPoint::Write(bool x)
{
	if (Point == nullptr) return 1;
	LoadPoint LP = *this;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return 2; // Ошибка индекса
		LP = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind);
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return 2; // Ошибка индекса
		else switch (Ind % 3) {
		case 0: LP = { TIP, &((IC_type)Point)->at(Ind / 3) }; break;
		case 1: LP = { Tint, &((IC_type)Point)->at(Ind / 3).atr }; break;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return 1; LP = ((IC_type)Point)->at(Ind / 3).Load;
		}
	switch (LP.Type)
	{
	case Tdouble:
		*((double*)LP.Point) = x;
		break;
	case Tfloat:
		*((float*)LP.Point) = x;
		break;
	case Tint:
		*((int*)LP.Point) = x;
		break;
	case Tbool:
		*((bool*)LP.Point) = x;
		break;
	case Tchar:
		*((char*)LP.Point) = x;
		break;
	case Tstring:
		*((string*)LP.Point) = to_string(x);
		break;
	defoult:
		return 1; // Несоответствие типов
	}
	return 0;
}

int LoadPoint::Write(char x)
{
	if (Point == nullptr) return 1;
	LoadPoint LP = *this;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return 2; // Ошибка индекса
		LP = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind);
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return 2; // Ошибка индекса
		else switch (Ind % 3) {
		case 0: LP = { TIP, &((IC_type)Point)->at(Ind / 3) }; break;
		case 1: LP = { Tint, &((IC_type)Point)->at(Ind / 3).atr }; break;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return 1; LP = ((IC_type)Point)->at(Ind / 3).Load;
		}
	switch (LP.Type)
	{
	case Tdouble:
		*((double*)LP.Point) = x;
		break;
	case Tfloat:
		*((float*)LP.Point) = x;
		break;
	case Tint:
		*((int*)LP.Point) = x;
		break;
	case Tbool:
		*((bool*)LP.Point) = x;
		break;
	case Tchar:
		*((char*)LP.Point) = x;
		break;
	case Tstring:
		*((string*)LP.Point) = x;
		break;
	defoult:
		return 1; // Несоответствие типов
	}
	return 0;
}

int LoadPoint::Write(string x)
{
	if (Point == nullptr) return 1;
	LoadPoint LP = *this;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return 2; // Ошибка индекса
		LP = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind);
	}
	else if (Type >> 1 == DICInd)
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return 2; // Ошибка индекса
		else switch (Ind % 3) {
		case 0: LP = { TIP, &((IC_type)Point)->at(Ind / 3) }; break;
		case 1: LP = { Tint, &((IC_type)Point)->at(Ind / 3).atr }; break;
		case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return 1; LP = ((IC_type)Point)->at(Ind / 3).Load;
		}
	switch (LP.Type)
	{
	case Tdouble:
		*((double*)LP.Point) = atof(x.c_str());
		break;
	case Tfloat:
		*((float*)LP.Point) = atof(x.c_str());
		break;
	case Tint:
		*((int*)LP.Point) = atoi(x.c_str());
		break;
	case Tbool:
		*((bool*)LP.Point) = atoi(x.c_str());
		break;
	case Tchar:
		*((char*)LP.Point) = atoi(x.c_str());
		break;
	case Tstring:
		*((string*)LP.Point) = x;
		break;
	defoult:
		return 1; // Несоответствие типов
	}
	return 0;
}

int LoadPoint::Write(LoadPoint x) // Записать величину из нагрузки
{
	if (Point == nullptr) return 1;
	LoadPoint LP = *this;
	if (Type >> 1 == DLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return 2; // Ошибка индекса
		((LoadVect_type)Point)->at(N * (Ind < 0) + Ind) = x; return 0;
	}
	else if (Type >> 1 == DICInd) {
		if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return 2; // Ошибка индекса
		if (Ind % 3 != 2)
			return 1;
		else
		{
			((IC_type)Point)->at(Ind / 3).Load = x.Clone();
			return 0;
		}
	}
	//LP = x.Clone();
	if(LP.Type==TLoad)
		*(LoadPoint*)LP.Point = x;
	return 0;
}
template <typename T>
int LoadPoint::WriteByVector(T x) // Запись по нагрузкам в векторе
{
	if (Point == nullptr) return 1;
	if (!isVect()) return 1;
	for (auto& i : *((vector<LoadPoint>*) Point))
		Write(i);
	return 0;
};

int LoadPoint::Write(vector<LoadPoint>* x)
{
	register unsigned int t = Type;
	register void* P = Point;
	if (Type == TLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return 2; // Ошибка индекса
		P = ((LoadVect_type)Point)->at(Ind).Point;
		t = ((LoadVect_type)Point)->at(Ind).Type;
	}
	else if (Type == TICInd) {
		if (Ind / 3 >= ((LoadVect_type)Point)->size()) return 2; // Ошибка индекса
		switch (Ind % 3) {
		case 0: P = &((IC_type)P)->at(Ind / 3); t = TIP; break;
		case 1: P = &((IC_type)P)->at(Ind / 3).atr; t = Tint; break;
		case 2: P = ((IC_type)P)->at(Ind / 3).Load.Point; t = ((IC_type)P)->at(Ind / 3).Load.Type;
		}
	}
	else return 2; // Ошибка индекса
	if (P == nullptr || t % 2 != 0) return 1;
	P = x; t = TLoadVect;
	return 0;
}

int LoadPoint::Write(vector<LoadPoint> x) // Копирование вектора нагрузок нагрузку
{
	if (Point == nullptr || !isVect() || Type % 2 != 0)
		return 1;
	register unsigned int t = Type;
	register void* P = Point;

	if (Type == TLoadVectInd) {
		register int N = ((LoadVect_type)Point)->size();
		if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return 2; // Ошибка индекса
		P = ((LoadVect_type)Point)->at(Ind).Point;
		t = ((LoadVect_type)Point)->at(Ind).Type;
	}
	else if (Type == TICInd) {
		if (Ind / 3 >= ((LoadVect_type)Point)->size()) return 2; // Ошибка индекса
		switch (Ind % 3) {
		case 0: P = &((IC_type)P)->at(Ind / 3); t = TIP; break;
		case 1: P = &((IC_type)P)->at(Ind / 3).atr; t = Tint; break;
		case 2: P = ((IC_type)P)->at(Ind / 3).Load.Point; t = ((IC_type)P)->at(Ind / 3).Load.Type;
		}
	}
	else return 2; // Ошибка индекса
	if (P == nullptr || t % 2 != 0) return 1;

	((LoadVect_type)P)->resize(x.size());
	copy(x.begin(), x.end(), ((LoadVect_type)P)->begin());
	return 0;
}

unsigned int  LoadPoint::TypeMinimize(double x) // Минимизировать тип, т.е. было целое число - возвращается int и т.д.
{
	if (x == int(x) and abs(x) < 2147483647)
		return Tint;
	else if (x == 1 || x == 0)
		return Tbool;
	else
		return Tdouble;

}

LoadPoint LoadPoint::TypeMinimizeOut(double x, bool var) // Минимизировать тип (возвращается LoadPoint), т.е. было целое число - возвращается int и т.д.
{
	if (x == int(x) and abs(x) < 2147483647)
	{
		int* t = new int;
		return { (var ? Tint : Cint), t };
	}
	else if (x == 1 || x == 0)
	{
		bool* t = new bool;
		return { (var ? Tbool : Cbool), t };
	}
	else
	{
		double* t = new double;
		return { (var ? Tdouble : Cdouble), t };
	}
}

LoadPoint LoadPoint::Clone(LoadPoint LP) // Дублировать нагрузку (вариант с передаваемой в качестве параметра нагрузки)
{
	return LP.Clone();
}

LoadPoint LoadPoint::Clone(bool All) // Вернуть клонированную нагрузку
{
	if (Point == nullptr)
		return { 0,nullptr };
	if (Type % 2 == 0 && !All) // Если переменная, то возвращаем указатель
		return *this;

	LoadPoint LP = *this;
	while (LP.Point != nullptr && (Type >> 1 == DLoadVectInd || Type >> 1 == DICInd))
		if (Type >> 1 == DLoadVectInd)
			if (Type == TLoadVectInd && !All)
				return LP;
			else
				LP = ((LoadVect_type)Point)->at(Ind);
		else if (LP.Type >> 1 == DICInd)
			if (LP.Ind < 0 || LP.Ind / 3 >= ((IC_type)Point)->size()) return {}; // Ошибка индекса
			else if (LP.Type == CICInd && !All)
				return LP;
			else switch (Ind % 3) {
			case 0: LP = { TIP, &((IC_type)Point)->at(Ind / 3) }; break;
			case 1: LP = { Tint, &((IC_type)Point)->at(Ind / 3).atr }; break;
			case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return {}; LP = ((IC_type)Point)->at(Ind / 3).Load;
			}
	switch (LP.Type >> 1)
	{
	case Dstring: return { LP.Type, new string(*(string*)LP.Point) };
	case Dint: return { LP.Type, new int(*(int*)LP.Point) };
	case Dfloat: return { LP.Type, new float(*(float*)LP.Point) };
	case Ddouble: return { LP.Type,new double(*(double*)LP.Point) };
	case Dchar: return { LP.Type,new char(*(char*)LP.Point) };
	case Dbool: return { LP.Type,new bool(*(bool*)LP.Point) };
	case DPPoint: return { LP.Type,new (void*)(*(void**)LP.Point) };
	case DLoadVect:
	{
		vector<LoadPoint>* t = new vector<LoadPoint>;
		t->resize(((vector<LoadPoint>*)Point)->size());
		for (auto i = t->begin(), j = ((vector<LoadPoint>*)Point)->begin(); i != t->end(); i++, j++)
			*i = j->Clone(); // Клонирование каждой нагрузки
		return { CLoadVect, t };
	}
	case DIC:

		break;
	case DIP: //return { Type, (*(ip*)Point).Сlone() };
	{
		vector<ip>* t = new vector<ip>;
		t->push_back(*((ip*)Point));
		//		((ip*)Point)->Load.Clone();
		return { Type, t };
	}
	default: return *this;
	}
}

void* LoadPoint::VarClone() // Вернуть ссылку на клонированное значение из нагрузки
{
	if (Point == nullptr)
		return nullptr;
	switch (Type >> 1)
	{
	case Dstring: return new string(*(string*)Point);
	case Dint: return new int(*(int*)Point);
	case Dfloat: return new float(*(float*)Point);
	case Ddouble: return new double(*(double*)Point);
	case Dchar: return new char(*(char*)Point);
	case Dbool: return new bool(*(bool*)Point);
	case DPPoint: return new (void*)(*(void**)Point);
	case DIP: {IC_type t = new vector<ip>; t->push_back(*(ip*)Point); return t; }
	case DIC: return ICCopy(*this).Point;
	case DLoadVect: {
		LoadVect_type t = new vector<LoadPoint>;
		t->resize(((LoadVect_type)Point)->size());
		copy(((LoadVect_type)Point)->begin(), ((LoadVect_type)Point)->end(), t->begin());
	}
	}
}

void LoadPoint::VarDel() // Удаление нагрузки ИП
{
	if (Point == nullptr)
		return;
	switch (Type)
	{
	case Cstring: delete (string*)Point; break;
		//	case Cint: delete (int*)Point; break;
	case Cfloat: delete (float*)Point; break;
		//	case Cdouble: delete (double*)Point; break;
	case Cchar: delete (char*)Point; break;
//	case Cbool: delete (bool*)Point; break;
	case CPPoint: delete (void**)Point; break;
	case CLoadVect: ((vector<LoadPoint>*)Point)->resize(0); delete ((vector<LoadPoint>*)Point); break;
	}
	Point = nullptr; Type = 0;
}

LoadPoint LoadPoint::Copy()
{
	LoadPoint LP;
	LP.Copy(*this);
	return LP;
}

void LoadPoint::Copy(LoadPoint LP)
{
	//	*this = LP.Clone();
	Type = LP.Type;
	if (LP.Type % 2 == 0)
	{
		Point = LP.Point;
		return;
	}
	switch (Type>>1)
	{
	case Dstring: Point = new string(*(string*)LP.Point); break;
	case Dint: Point = new int(*(int*)LP.Point); break;
	case Dfloat: Point = new float(*(float*)LP.Point); break;
	case Ddouble: Point = new double(*(double*)LP.Point); break;
	case Dchar: Point = new char(*(char*)LP.Point); break;
	case Dbool: Point = new bool(*(bool*)LP.Point); break;
	case DPPoint: Point = new (void*)(*(void**)LP.Point); break;
	case DIP: // ???
		break;
	case DIC:
		Point = ICCopy(LP).Point;
		break;
	}
}

void LoadPoint::Clear() // Сбросить нагрузку
{
	if (Type % 2 == 0) return;
	switch (Type)
	{
	case Cstring: delete (string*)Point; break;
	case Cint: delete (int*)Point; break;
	case Cfloat: delete (float*)Point; break;
	case Cdouble: delete (double*)Point; break;
	case Cchar: delete (char*)Point; break;
	case Cbool: delete (bool*)Point; break;
	case CPPoint: delete (void**)Point; break;
	case CIP: // ???
		break;
	case CIC:
		ICDel(Point);
		break;
	}
	Type = 0; Point = nullptr;
}
// Печать вектора
void LoadPoint::LoadPoint::VarClear() // Сброс нагрузки ИП в том числе и с переменной (переменная стирается)
{
	Type -= Type % 2;
	Type += 1;
	Clear();
}

void LoadPoint::VectorPrint(unsigned int Type, void* P, map<long int, string > AtrMnemo, string offset, string Sep, string End, string ArrayBracketStart, string ArrayBracketFin)
{
	switch ((Type % 1000) >> 1)
	{
	case Dstring: {
		cout << ArrayBracketStart;
		for (auto t = (*((vector<string>*)P)).begin(); t != (*(vector<string>*)P).end(); t++)
			cout << *t << (t == (*((vector<string>*)P)).begin() - 1 ? Sep : "");
		cout << ArrayBracketFin;
		break; }
	case Dint: {
		cout << ArrayBracketStart;
		for (auto t = (*((vector<int>*)P)).begin(); t != (*(vector<int>*)P).end(); t++)
			cout << *t << (t == (*((vector<int>*)P)).begin() - 1 ? Sep : "");
		cout << ArrayBracketFin;
	}break;
	case Dfloat: {
		cout << ArrayBracketStart;
		for (auto t = (*((vector<float>*)P)).begin(); t != (*(vector<float>*)P).end(); t++)
			cout << *t << (t == (*((vector<float>*)P)).begin() - 1 ? Sep : "");
		cout << ArrayBracketFin;
	}break;
	case Ddouble: {
		cout << ArrayBracketStart;
		for (auto t = (*((vector<double>*)P)).begin(); t != (*(vector<double>*)P).end(); t++)
		{
			cout << *t << ((t != (*((vector<double>*)P)).end() - 1) ? Sep : "");
		}
		cout << ArrayBracketFin;
	}break;
	case Dchar: {
		cout << ArrayBracketStart;
		for (auto t = (*((vector<char>*)P)).begin(); t != (*(vector<char>*)P).end(); t++)
			cout << *t << (t == (*((vector<char>*)P)).begin() - 1 ? Sep : "");
		cout << ArrayBracketFin;
	}break;
	case Dbool: {
		cout << ArrayBracketStart;
		for (auto t = (*((vector<bool>*)P)).begin(); t != (*(vector<bool>*)P).end(); t++)
			cout << *t << (t == (*((vector<bool>*)P)).begin() - 1 ? Sep : "");
		cout << ArrayBracketFin;
	}break;
	case DIP: {
		cout << ArrayBracketStart;
		for (auto t = (*((vector<ip>*)P)).begin(); t != (*(vector<ip>*)P).end(); t++)
			t->Load.print(AtrMnemo, offset, Sep, End);
		cout << ArrayBracketFin;
	}break;
	case DIC: {
		cout << ArrayBracketStart;
		for (auto t = (*((vector<ip>*)P)).begin(); t != (*(vector<ip>*)P).end(); t++)
			t->Load.print(AtrMnemo, offset, Sep, End);
		cout << ArrayBracketFin;
	}break;
	default:
		cout << "unknown load\n";
	}
}
// Печать матрицы
void LoadPoint::MatrixPrint(unsigned int Type, void* P, map<long int, string > AtrMnemo, string offset, string Sep, string End, string ArrayBracketStart, string ArrayBracketFin)
{
	switch ((Type % 1000) >> 1)
	{
	case Dstring: {
		cout << ArrayBracketStart;
		for (auto t = (*((vector< vector<string>>*)P)).begin(); t != (*(vector < vector<string>>*)P).end(); t++)
			VectorPrint(Type - 1000, (void*)&(*t), AtrMnemo, offset, Sep, End, ArrayBracketStart, ArrayBracketFin);
		cout << ArrayBracketFin;
		break; }
	case Dint: {
		cout << ArrayBracketStart;
		for (auto t = (*((vector < vector<long int>>*)P)).begin(); t != (*(vector < vector<long int>>*)P).end(); t++)
			VectorPrint(Type - 1000, (void*)&(*t), AtrMnemo, offset, Sep, End, ArrayBracketStart, ArrayBracketFin);
		cout << ArrayBracketFin;
	}break;
	case Dfloat: {
		cout << ArrayBracketStart;
		for (auto t = (*((vector < vector<float>>*)P)).begin(); t != (*(vector < vector<float>>*)P).end(); t++)
			VectorPrint(Type - 1000, (void*)&(*t), AtrMnemo, offset, Sep, End, ArrayBracketStart, ArrayBracketFin);
		cout << ArrayBracketFin;
	}break;
	case Ddouble: {
		cout << ArrayBracketStart;
		for (auto t = (*((vector < vector<double>>*)P)).begin(); t != (*(vector < vector<double>>*)P).end(); t++)
		{
			VectorPrint(Type - 1000, (void*)&(*t), AtrMnemo, offset, Sep, End, ArrayBracketStart, ArrayBracketFin);
			cout << End;
		}
		cout << ArrayBracketFin;
	}break;
	case Dchar: {
		cout << ArrayBracketStart;
		for (auto t = (*((vector < vector<char>>*)P)).begin(); t != (*(vector < vector<char>>*)P).end(); t++)
			VectorPrint(Type - 1000, (void*)&(*t), AtrMnemo, offset, Sep, End, ArrayBracketStart, ArrayBracketFin);
		cout << ArrayBracketFin;
	}break;
	case Dbool: {
		cout << ArrayBracketStart;
		for (auto t = (*((vector < vector<bool>>*)P)).begin(); t != (*(vector < vector<bool>>*)P).end(); t++)
			VectorPrint(Type - 1000, (void*)&(*t), AtrMnemo, offset, Sep, End, ArrayBracketStart, ArrayBracketFin);
		cout << ArrayBracketFin;
	}break;
	case DIP: {
		cout << ArrayBracketStart;
		for (auto t = (*((vector < vector<ip>>*)P)).begin(); t != (*(vector < vector<ip>>*)P).end(); t++)
			VectorPrint(Type - 1000, (void*)&(*t), AtrMnemo, offset, Sep, End, ArrayBracketStart, ArrayBracketFin);
		cout << ArrayBracketFin;
	}break;
	case DIC: {
		cout << ArrayBracketStart;
		for (auto t = (*((vector < vector<ip>>*)P)).begin(); t != (*(vector < vector<ip>>*)P).end(); t++)
			VectorPrint(Type - 1000, (void*)&(*t), AtrMnemo, offset, Sep, End, ArrayBracketStart, ArrayBracketFin);
		cout << ArrayBracketFin;
	}break;
	default:
		cout << "unknown load\n";
	}
}
// AtrMnemo - словарь мнемоник атрибутов
// AdrMap - список ссылок уже пройденных ИК при выводе ОА-графа
void LoadPoint::print(map<long int, string > AtrMnemo, string offset, string Sep, string End, string quote, string ArrayBracketStart, string ArrayBracketFin, int VectCol, map<void*, int>* AdrMap)
{
	LoadPoint LP =IndLoadReturn();
	if (LP.Point == nullptr)
	{
		cout << "null";
		return;
	}
	switch (LP.Type)
	{
	case Tstring:
	case Cstring: cout << quote << LP.toStr() << quote; break;
	case Tint:
	case Cint:	  cout << LP.toInt(); break;
	case Tfloat:
	case Cfloat:  cout << LP.toFloat(); break;
	case Tdouble:
	case Cdouble: cout << LP.toDouble(); break;
	case Tchar:
	case Cchar:   cout << LP.toChar(); break;
	case Tbool:
	case Cbool:   cout << LP.toBool(); break;
	case TIP:
	case CIP:
	case TIC:
	case CIC:
		/*		if (((ip*)Point)->Load.Type >> 1 == DIP || isIC())
					if(AtrMnemo.count(((ip*)Point)->atr))
						cout << AtrMnemo[((ip*)Point)->atr] << " ->\n";
					else
						cout << ((ip*)Point)->atr << " ->\n";
				else
					if (AtrMnemo.count(((ip*)Point)->atr))
						cout << AtrMnemo[((ip*)Point)->atr] << ((((ip*)Point)->Load.Type % 2) ? " # " : "=");
					else
						cout << ((ip*)Point)->atr << ((((ip*)Point)->Load.Type%2)? " # ": "=");
				((ip*)Point)->Load.print();
				break;
		*/
	{
/*		LoadPoint LP = *this;
		while (LP.isVectInd() || LP.isICInd())
			if (Type >> 1 == DLoadVectInd) {
				register int N = ((LoadVect_type)Point)->size();
				if (N <= Ind || N < -Ind || ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind).Point == nullptr) return; // Ошибка индекса
				LP = ((LoadVect_type)Point)->at(N * (Ind < 0) + Ind);
			}
			else if (Type >> 1 == DICInd)
				if (Ind < 0 || Ind / 3 >= ((IC_type)Point)->size()) return; // Ошибка индекса
				else switch (Ind % 3) {
				case 0: LP = { TIP, &((IC_type)Point)->at(Ind / 3) }; break;
				case 1: LP = { Tint, &((IC_type)Point)->at(Ind / 3).atr }; break;
				case 2: if (((IC_type)Point)->at(Ind / 3).Load.Point == nullptr) return; LP = ((IC_type)Point)->at(Ind / 3).Load;
				}
*/
		if (LP.Type >> 1 == DIP)
		{
			if (AtrMnemo.count(((ip*)LP.Point)->atr))
				cout << AtrMnemo[((ip*)LP.Point)->atr] <<  " = ";
			else
				cout << ((ip*)LP.Point)->atr << ((((ip*)LP.Point)->Load.Type%2==0) ? " = " : " # ");
			((ip*)LP.Point)->Load.print();
			break;
		}
		bool FMap = false; // Флаг создания списка пройденных адресов ОА-графа
		if (AdrMap == nullptr)
		{
			AdrMap = new map<void*, int>;
			FMap = true;
		}
		if (AdrMap->count(LP.Point)) // Обнаружение зацикливания ОА-графа
		{
			cout << offset << "IC id: " << (*AdrMap)[LP.Point] << endl;
			break;
		}
		(*AdrMap)[LP.Point] = AdrMap->size(); // Запомнить пройденную ИК для избежания зацикливания

		if (!((IC_type)LP.Point)->size()) return;
		for (auto i = ((IC_type)LP.Point)->begin(); i != ((IC_type)LP.Point)->end(); i++)
		{
			if (i->Load.Type >> 1 == DIP || i->Load.isIC())
				if (AtrMnemo.count(i->atr))
					cout << offset << AtrMnemo[i->atr] << " ->\n";
				else
					cout << offset << i->atr << " ->\n";
			else
				if (AtrMnemo.count(i->atr))
					cout << offset << AtrMnemo[i->atr] << ((i->Load.Type % 2) ? " # " : " = ");
				else
					cout << offset << i->atr << ((i->Load.Type % 2) ? " # " : " = ");
			i->Load.print(AtrMnemo, offset + "  ", Sep, End, quote, ArrayBracketStart, ArrayBracketFin, VectCol, AdrMap);
			if (i != ((IC_type)LP.Point)->end() - 1)
				cout << endl;
		}
		if (FMap)  // Удачить таблицу пройденных адресов
		{
			AdrMap->clear();
			delete AdrMap;
		}
		break;
	}
	case TLoadVect:
	case CLoadVect: // Вектор нагрузок
	{
		cout << ArrayBracketStart;
		register int c = 1;
		for (auto i : *(vector<LoadPoint>*) Point)
		{
			if (VectCol > 0 && c > 1 && (c - 1) % VectCol == 0)
				cout << End;
			i.print(AtrMnemo, offset, Sep, End, quote, ArrayBracketStart, ArrayBracketFin);
			if (c < ((vector<LoadPoint>*)
				Point)->size()) cout << Sep;
			c++;
		}
		cout << ArrayBracketFin<< endl;
		break;
	}
	case TLoadVectInd:
	case CLoadVectInd: // Вектор нагрузок
	{
		register int i = Ind;
		cout << "Vect Ind[" << Ind << "] ";
		if (((LoadVect_type)Point)->size() > abs(i) or -i == ((LoadVect_type)Point)->size())
			((LoadVect_type)Point)->at(i).print(AtrMnemo, offset, Sep, End, quote, ArrayBracketStart, ArrayBracketFin);
		break;
	}
	default:
		if (Type >= 2000) // Печать матрицы и вектора
		{
			MatrixPrint(Type, Point, AtrMnemo, offset, Sep, End, ArrayBracketStart, ArrayBracketFin);
		}
		else if (Type >= 1000)
			VectorPrint(Type, Point, AtrMnemo, offset, Sep, End, ArrayBracketStart, ArrayBracketFin);
	}
}

// Работа с ФУ
void FU::CommonMk(long int Mk, LoadPoint Load, FU* Sender)
{
	Mk %= FUMkRange;
	if (Mk < 0) // Команды для АЛУ
	{

		if (Alu == nullptr) // Если еще не создан АЛУ
		{ // Создать АЛУ
			Alu = (FU*)new ALU(Bus);
			//			cout << "FUType: " << FUtype << endl;
			Alu->ProgFU(0, { 0,nullptr }, this);
			((FU*)Alu)->Parent = this;
			Accum = { Cdouble, &((ALU*)Alu)->accum };
		}
		Alu->ProgFU(-Mk, Load, this);
		return;
	}
	switch (Mk)
	{
	case 937: // MkRedirectSet Установть МК для переадресации на другое ФУ (устанавливается начало диапазона МК для другого ФУ
		MkRedirect = Load.toInt();
		break;
	case 936: //ExecCouterSet Установить количество повторений программы
		ExecRepeat = Load.toInt(1);
		break;
	case ActiveMk: // ActiveSet Установить активность ФУ (true по умолчанию)
		Active = Load.toBool(true);
		break;
	case ProgMk: // 958 Prog Вызов подпрограммы
	case ProgCycleMk: //959 CycleProg Вызов цикла
	case ProgPostCycleMk: //960 PostCycleProg Вызов пост цикла
		if (Alu != nullptr)
			((ALU*)Alu)->Stack.push_back({}); //Буферизиация текущего стека
		if (Load.Point == nullptr)
			ProgExec(Prog, Mk - ProgMk);
		else
			ProgExec(Load, Mk - ProgMk);
		if (Alu != nullptr)
			((ALU*)Alu)->Stack.pop_back(); // Отмена буферизации текущего стека
		break;
	case YesMk: //961 YesProg Вызов подпрограммы по ДА
	case YesCycleMk: //962 YesCycleProg Вызов цикла по ДА
	case YesPostCycleMk: //963 YesPostCycleProg Вызов пост цикла по ДА
	case YesBreakMk: //967 YesProgBreak Вызов подпрограммы по ДА с последующим прерыванием основной программы
		if (Accum.toBool())
		{
			int PB = Mk == YesBreakMk;
			if (Alu != nullptr)
				((ALU*)Alu)->Stack.push_back({}); //Буферизиация текущего стека
			if (Mk == YesBreakMk) {
				Mk = YesMk;
			}
			if (Load.Point == nullptr)
				ProgExec(Prog, Mk - YesMk);
			else
				ProgExec(Load.Point, Mk - YesMk);
			if (Alu != nullptr)
				((ALU*)Alu)->Stack.pop_back(); // Отмена буферизации текущего стека
			ProgStop += PB; // Выйти из главной программы
		}
		break;
	case NoMk: //964 NoProg Вызов подпрограммы по НЕТ
	case NoCycleMk: //965 NoCycleProg Вызов цикла по НЕТ
	case NoPostCycleMk: //966 NoPostCycleProg Вызов пост цикла по НЕТ
	case NoBreakMk: //968 NoProgBreak Вызов подпрограммы по НЕТ с последующим прерыванием основной программы
		if (!Accum.toBool())
		{
			int PB = Mk == NoBreakMk;
			if (Alu != nullptr)
				((ALU*)Alu)->Stack.push_back({}); //Буферизиация текущего стека
			if (Mk == NoBreakMk) {
				ProgStop += 1;
				Mk = NoMk;
			}
			if (Load.Point == nullptr)
				ProgExec(Prog, Mk - NoMk);
			else
				ProgExec(Load.Point, Mk - NoMk);
			if (Alu != nullptr)
				((ALU*)Alu)->Stack.pop_back(); // Отмена буферизации текущего стека
			ProgStop += PB; // Выйти из главной программы
		}
		break;
	case 941: //
	break;
	case 946: // FUMkRangeSet Устаровить интервал индексов МК
		FUMkRange = Load.toInt();
		break;
	case 945: // ParentSet Установить родителя
		Parent = (FU*)Load.Point;
		break;
	case 944: // ParentOut Выдать ссылку на родителя
		Load.Write(Parent);
		break;
	case 943: // ParentOutMk Выдать МК со ссылкой на родителя
		MkExec(Load, {CFU,Parent });
		break;
	case 919: // AccumPointerSet Установить ссылку на аккумулятор
		if (Load.Type >> 1 == Ddouble)
			Accum = Load;
		break;
	case 920: // AccumPointerOut Выдать ссылку на аккумулятор
		Load.Write(Accum);
		break;
	case 921: // AccumPointerOutMk Выдать МК со ссылкой на аккумулятор
		MkExec(Load, Accum);
		break;
	case 931: // AccumSet Установить значение аккумулятора
		if (Accum.Point = nullptr)
		{
			Accum = { Cdouble,new double };
			AccumCreating = true; // Устанавливаем флаг самостоятельного создания аккумулятора ФУ-ом
		}
		Accum.Write(Load.toDouble());
		AccumCreating = false;
		break;
	case 923: // AccumOut Выдать значение аккумулятора
		Load.WriteFromLoad(Accum);
		break;
	case 925: // AccumOutMk Выдать МК со значением аккумулятора
		MkExec(Load, Accum);
		break;
	case FUIndSetMk: // 933 FUIndSet Установить индекс ФУ
		FUInd = Load.toInt();
		break;
	case 992: // FUIndOut Выдать индекс ФУ
		Load.Write(FUInd);
		break;
	case 993: //FUIndOutMk Выдать милликоманду с индексом ФУ
		MkExec(Load, { Cint, &FUInd });
		break;
	case CalcMk: // 927 Calc вычислить АЛВ
	case 929: // CalcContinue Вычислить АЛВ и продолжеть выполнение программы, если результат true
	case 930: // CalcBreak Вычислить АЛВ и продолжеть выполнение программы, если результат false
		if (Alu == nullptr)
		{
			Alu = (FU*)new ALU(this->Bus);
			Accum = { Cdouble, &((ALU*)Alu)->accum };
			AccumCreating = false;
			ALUCreating = true;
		}
		((FU*)Alu)->ProgFU(ProgExecMk, Load, this);
		break;
		if (Mk == 929 && Accum.toBool() || Mk == 930 && !Accum.toBool())
			ProgStop = 1; // Выход из программы
	case 932: // IntAluCalc Создать челочисленное АЛУ
		Alu = new IntAlu(this->Bus);
		Accum = { Tint, &((IntAlu*)Alu)->Accum };
		AccumCreating = false;
		ALUCreating = true;
		((FU*)Alu)->ProgFU(ProgExecMk, Load, this);
		break;
	case 989: // ProgStop Остановка программы (в нагрузке количество уровней выхода; если нагрузка nil, то присваивается 1)
		ProgStop = Load.toInt(2);
		break;
	case 988: // ProgStopAll Остановка всех запущенных на выполнение миллипрограммы для данного ФУ
		ProgStopAll = Load.toBool(true);
		break;
	case 987: //Next Переход к следующей итерации цикла
		CycleStop = Load.toInt();
		break;
	case 916:// ManualModeSet Установить режим ручного управления
		if (Modeling == nullptr) Modeling = new FUModeling();
		Modeling->ManualMode = Load.toBool();
		break;
	case SchedulerSetMk: //918: // SchedulerSet Установить контекст планировщика вычислений
		if (Load.Point == nullptr) break;
		if (Modeling == nullptr) Modeling = new FUModeling();
		Modeling->scheduler = (FU*)Load.Point;
		Modeling->ManualMode = true;
		break;
	case 924: // PrеfixProgSet
		PrefixProg = (IC_type)Load.Point;
		break;
	case 922: // PostfixProgSet
		PostfixProg = (IC_type)Load.Point;
		break;
	case 926: // BusSet
		Bus = (FU*)Load.Point;
		break;
	case ProgExecMk: // ProgExec Выполнить программу (если в нагрузке null, То выполнить из регистра Prog
		if (Load.Point == nullptr)
			ProgExec((vector<ip>*)Prog);
		else
			ProgExec((vector<ip>*)Load.Point);
		break;
	case 991: // ProgSet // Установить указатель на программу
		Prog = (IC_type)Load.Point;
		break;
	case 953: // ElseProgSet
		ElseProg = (IC_type)Load.Point;
		break;
	case 954: // ProgSetExec Установить указатель на программу и выполнить ее
		Prog = (IC_type)Load.Point;
		ProgExec((vector<ip>*)Prog);
		break;
	case ContextOutMk:  // 995 ContextOut Выдать указатель на контекст ФУ
		//		if (Load.Type >> 1 == Dvoid || Load.Type >> 1 == DPPoint || Load.Type >> 1 == DFU)
		//			Load.Point = this;
		Load.Write((FU*)this);
		break;
	case 928: // ALUSet Установить ссылку на АЛУ
		if (ALUCreating)
			delete Alu;
		Alu = (FU*)Load.Point;
		ALUCreating = false;
		break;
	case 998: //FUNameSet
		FUName = Load.toStr();
		break;
	case 996: // FUNameOut
		Load.Write(FUName);
		break;
	case 997: // FUNameOutMk
		if (Load.Point != nullptr)
			MkExec(Load.toInt(), { Cstring,&FUName });
		break;
	case ContextOutMkMk: // 999 ContextOutMK Выдать милликоманду с указателем на контекст ФУ
		if (Load.isInt())
			Bus->ProgFU(Load.toInt(), {TFU, this}, this);
		//MkExec(Load, { TFU, this });
		break;
	case BreakMk: //Выход из циклов
	    CycleStop = Load.toInt(1);
		if (CycleStop < 0) CycleStop = 0;
		break;
	case NextMk: // Продолжение циклов
		CycleStop = - Load.toInt(1); // При CycleStop < 0 выполняется оператор Continue
		if (CycleStop > 0) CycleStop = 0;
		break;
	case MkGlobalRangeSet: // Установить глобальный адрес МК для ФУ
		FUMkGlobalAdr = Load.toInt();
		break;
	case MkGlobalRangeOutMk:  // Выдать глобальный адрес МК для ФУ
		Load.Write(FUMkGlobalAdr);
		break;
	case MkGlobalRangeOutMkMK: // Выдать МК с глобальным адресом МК для ФУ
		MkExec(Load, { Cint, &FUMkGlobalAdr });
		break;
	case 917: // EventserSet Установить указатель на контроллер событий
		if (Modeling == nullptr) Modeling = new FUModeling();
		Modeling->eventser = (FU*)Load.Point;
		break;	
	case 938: // Заблокировать выполнение всех программ для ФУ
		ProgsBlock = Load.toBool();
		break;
}

}

// Запуск программы
// CycleType тип цикла: 0 - без цикла, 1 - цикл, 2 - цикл с постусловием
void FU::ProgExec(void* UK, unsigned int CycleMode, FU* ProgBus, vector<ip>::iterator* Start) // Исполнение программы из ИК
{
	if (ProgsBlock) return;
	if (UK == nullptr)
		if (Prog != nullptr)
			UK = Prog;
		else
			return;

	vector<ip>* Uk = (IC_type)UK;
	if (ProgBus == nullptr) ProgBus = Bus;
	bool RepeatF = false; // Флаг циклического повторения программы в ИК
	ExecCounter.push_back(ExecRepeat);
	ExecRepeat = 1;
	for (; ExecCounter.back() > 0; ExecCounter.back()--)
	{
		do
		{
			ProgStop = 0; // Останов программы (содержит количество уровней, из которых необходимо выйти
			RepeatF = false; // Флаг необходимости перехода на новый цикл выборки МК в ИК
			ProgStopAll = false; // Флаг остановки всей программы
			CycleStop = 0; // Счетчик выходов из циклов (если отрицательная величина, то Продолжение цикла)
			for (auto i = Start == nullptr ? Uk->begin() : *Start; i != Uk->end(); i++)
			{
				if (i->atr == SubIC) // Переход на другую ИК
				{
					ProgExec(i->Load, CycleMode, ProgBus, Start);
					return;
				}
				if (i->atr >= FUMkRange)
					ProgBus->ProgFU(i->atr, i->Load, this); // Если диапазон МК не принадлежит ФУ (выдаем на Bus)
				else // МК для данного ФУ
				{
					if (i->atr == YesContinueAtr || i->atr == NoContinueAtr)
					{
						if (CycleMode == 2) // Проверка цикла с постусловием (пропускаем первую проверку)
						{
							CycleMode = 1;
							continue;
						}
						if (i->Load.isProg() && Alu != nullptr) // Запуск вычисления АЛВ

							// Перейти к следующей итерации цикла continue
							if (i->Load.isProg())
							{
								if (Alu == nullptr)
								{
									Alu = (FU*)new ALU(this);
									((FU*)Alu)->Bus = Bus;
									ALUCreating = true;
									AccumCreating = false;
									if (Accum.Point != nullptr && AccumCreating)
										Accum.Clear();
									AccumCreating = false;
									Accum = { Cdouble,&((ALU*)Alu)->accum };
								}
								((FU*)Alu)->ProgExec(i->Load);
							}
						if (i->atr == YesContinueAtr && !Accum.toBool() || i->atr == NoContinueAtr && Accum.toBool())
						{
							CycleMode = 0; // Выход из цикла
							ProgStop += 1; // Выход из цикла
							break;
						} // Выход из цикла
						continue;
					}
					if (i->atr == RepeatAtr) { // Запустить программу заново
						RepeatF = true; break;
					}
					ProgFU(i->atr, i->Load, this); // Выполнение команды
				}

				if (CycleStop != 0) // Остановка циклов
					if (!CycleMode) // Если не в режиме цикла, то просто выходим из уровня
						return;
					else {
						if (CycleStop > 0)
						{
							CycleStop--; // уменьшение счетчика выходов из цикла
							if (!CycleStop) return;
						}
						else
						{
							CycleStop++; // уменьшение счетчика выходов из цикла
							if (CycleStop)
								return;
						}
					}
				if (ProgStop > 0) { ProgStop--; return; }
				if (ProgStopAll) { return; } // Внеочередной выход из подпрограммы
			}
		} while (RepeatF || CycleMode > 0);
	}
	ExecCounter.pop_back();
}

void FU::ProgNExec(vector<void *> Uk) // Исполнение нескольких программ из ИК
{
	
	for (auto i: Uk)
		ProgExec(i); // Выполнение каждой программы из списка
}
// Запуск программы по указателю из нарузки ИП
void FU::ProgExec(LoadPoint Uk, unsigned int CycleMode, FU* Bus, vector<ip>::iterator* Start) // Исполнение программы из ИК
{
	if (Uk.isEmpty())
		ProgExec(Prog, CycleMode, Bus, Start);
	if (Uk.isIC())
		ProgExec(Uk.Point, CycleMode, Bus, Start);
	else if (Uk.isDigit())
	{
		ExecRepeat = Uk.toInt(1);
		ProgExec(Prog, CycleMode, Bus, Start);
	}
}

void FU::MkExec(long int MK, LoadPoint Load, void* Receiver, bool Ext) // Выдача МК с нагрузкой
{
	if (MK < FUMkRange && !Ext && (Receiver==nullptr || Receiver==Bus)) // Если МК адресована самому ФУ
		ProgFU(MK, Load, this);
	else
		if (Receiver != nullptr)
			((FU*)Receiver)->ProgFU(MK, Load, this);
		else
			Bus->ProgFU(MK, Load, this);
}

void FU::MkExec(LoadPoint Mk, LoadPoint Load, void* Receiver, bool Ext) // Выдача МК с нагрузкой
{
	if (Mk.Point!=nullptr && Mk.isInt() && Mk.Point != nullptr)
	{
		register long MK = Mk.toInt();
		if (MK < FUMkRange && !Ext) // Если МК адресована самому ФУ
			ProgFU(MK, Load, this);
		else
			if (Receiver != nullptr)
				((FU*)Receiver)->ProgFU(MK, Load, this);
			else
				Bus->ProgFU(MK, Load, this);
	}
}

void FU::MkExec(LoadPoint Mk, long Var, void* Receiver, bool Ext) // Выдача МК с нагрузкой
{
	if (Mk.Point != nullptr && Mk.isInt() && Mk.Point != nullptr)
	{
		register long MK = Mk.toInt();
		long var = Var;
		if (MK < FUMkRange && !Ext) // Если МК адресована сомому ФУ
			ProgFU(MK, { Cint, &var }, this);
		else
			if (Receiver != nullptr)
				((FU*)Receiver)->ProgFU(MK, { Cint, &var }, this);
			else
				Bus->ProgFU(MK, { Cint, &var }, this);
	}
}

void FU::MkExec(LoadPoint Mk, long* Var, void* Receiver, bool Ext) // Выдача МК с нагрузкой
{
	if (Mk.Point != nullptr && Mk.isInt() && Mk.Point != nullptr)
	{
		register long MK = Mk.toInt();
		if (MK < FUMkRange && !Ext) // Если МК адресована сомому ФУ
			ProgFU(MK, { Cint, Var }, this);
		else
			if (Receiver != nullptr)
				((FU*)Receiver)->ProgFU(MK, { Cint, Var }, this);
			else
				Bus->ProgFU(MK, { Cint, Var }, this);
	}
}

void FU::MkExec(LoadPoint Mk, double Var, void* Receiver, bool Ext) // Выдача МК с нагрузкой
{
	if (Mk.Point != nullptr && Mk.isInt() && Mk.Point != nullptr)
	{
		register long MK = Mk.toInt();
		double var = Var;
		if (MK < FUMkRange && !Ext) // Если МК адресована сомому ФУ
			ProgFU(MK, { Cdouble, &var }, this);
		else
			if (Receiver != nullptr)
				((FU*)Receiver)->ProgFU(MK, { Cdouble, &var }, this);
			else
				Bus->ProgFU(MK, { Cdouble, &var }, this);
	}
}

void FU::MkExec(LoadPoint Mk, double* Var, void* Receiver, bool Ext) // Выдача МК с нагрузкой
{
	if (Mk.Point != nullptr && Mk.isInt() && Mk.Point != nullptr)
	{
		register long MK = Mk.toInt();
		if (MK < FUMkRange && !Ext) // Если МК адресована сомому ФУ
			ProgFU(MK, { Cdouble, Var }, this);
		else
			if (Receiver != nullptr)
				((FU*)Receiver)->ProgFU(MK, { Cdouble, Var }, this);
			else
				Bus->ProgFU(MK, { Cdouble, Var }, this);
	}
}

void FU::MkExec(LoadPoint Mk, bool Var, void* Receiver, bool Ext) // Выдача МК с нагрузкой
{
	if (Mk.Point != nullptr && Mk.isInt() && Mk.Point != nullptr)
	{
		register long MK = Mk.toInt();
		bool var = Var;
		if (MK < FUMkRange && !Ext) // Если МК адресована сомому ФУ
			ProgFU(MK, { Cbool, &var }, this);
		else
			if (Receiver != nullptr)
				((FU*)Receiver)->ProgFU(MK, { Cbool, &var }, this);
			else
				Bus->ProgFU(MK, { Cbool, &var }, this);
	}
}

void FU::MkExec(LoadPoint Mk, bool* Var, void* Receiver, bool Ext) // Выдача МК с нагрузкой
{
	if (Mk.Point != nullptr && Mk.isInt() && Mk.Point != nullptr)
	{
		register long MK = Mk.toInt();
		if (MK < FUMkRange && !Ext) // Если МК адресована сомому ФУ
			ProgFU(MK, { Cbool, Var }, this);
		else
			if (Receiver != nullptr)
				((FU*)Receiver)->ProgFU(MK, { Cbool, Var }, this);
			else
				Bus->ProgFU(MK, { Cbool, Var }, this);
	}
}

void FU::MkExec(LoadPoint Mk, string Var, void* Receiver, bool Ext) // Выдача МК с нагрузкой
{
	if (Mk.Point != nullptr && Mk.isInt() && Mk.Point != nullptr)
	{
		register long MK = Mk.toInt();
		string var = Var;
		if (MK < FUMkRange && !Ext) // Если МК адресована сомому ФУ
			ProgFU(MK, { Cstring, &var }, this);
		else
			if (Receiver != nullptr)
				((FU*)Receiver)->ProgFU(MK, { Cstring, &var }, this);
			else
				Bus->ProgFU(MK, { Cstring, &var }, this);
	}
}

void FU::MkExec(LoadPoint Mk, string* Var, void* Receiver, bool Ext) // Выдача МК с нагрузкой
{
	if (Mk.Point != nullptr && Mk.isInt() && Mk.Point != nullptr)
	{
		register long MK = Mk.toInt();
		if (MK < FUMkRange && !Ext) // Если МК адресована сомому ФУ
			ProgFU(MK, { Cstring, Var }, this);
		else
			if (Receiver != nullptr)
				((FU*)Receiver)->ProgFU(MK, { Cstring, Var }, this);
			else
				Bus->ProgFU(MK, { Cstring, Var }, this);
	}
}

void FU::MkExec(LoadPoint Mk, char Var, void* Receiver, bool Ext) // Выдача МК с нагрузкой
{
	if (Mk.Point != nullptr && Mk.isInt() && Mk.Point != nullptr)
	{
		register long MK = Mk.toInt();
		char var = Var;
		if (MK < FUMkRange && !Ext) // Если МК адресована сомому ФУ
			ProgFU(MK, { Cchar, &var }, this);
		else
			if (Receiver != nullptr)
				((FU*)Receiver)->ProgFU(MK, { Cchar, &var }, this);
			else
				Bus->ProgFU(MK, { Cchar, &var }, this);
	}
}

void FU::MkExec(LoadPoint Mk, char* Var, void* Receiver, bool Ext) // Выдача МК с нагрузкой
{
	if (Mk.Point != nullptr && Mk.isInt() && Mk.Point != nullptr)
	{
		register long MK = Mk.toInt();
		if (MK < FUMkRange && !Ext) // Если МК адресована сомому ФУ
			ProgFU(MK, { Cchar, Var }, this);
		else
			if (Receiver != nullptr)
				((FU*)Receiver)->ProgFU(MK, { Cchar, Var }, this);
			else
				Bus->ProgFU(MK, { Cchar, Var }, this);
	}
}


void FU::MkAwait(long int MK, LoadPoint Load, FU* Sender, double Delay) // ?????????? ?? ??? ???????? ?? ??????? ??? ?????????????
{
	if (this == nullptr)
		return;
	if (Modeling == nullptr || Modeling->eventser == nullptr)
	{
		ProgFU(MK, Load, Sender);
		return;
	}
	double t;
	ip IP= {MK, Load};
//	LoadPoint LP = { Tdouble, &t };
//	Modeling->eventser->ProgFU(EventserCurrentTimeOutMk, LP, this); // ????? ??????? ????? ?? ??????????? ???????
//	Modeling->qAwaitMk.insert({ t + Delay, {MK, Load, Sender} }); // ????????? ????? ?????????? ??????? ?? ? ??????? ????????

	Modeling->eventser->ProgFU(EventserFUSetMk, { CFU, this }, this); // ????????? ????? ??????? ? ?????? ????????? ???????
	Modeling->eventser->ProgFU(EventTimeSetMk, { Cdouble, &Delay }, this); // Установить вреям задержки ожидаемой МК
	Modeling->eventser->ProgFU(AwaitMkSetMk, { CIP, &IP}, this); // ????????? ????? ??????? ? ?????? ????????? ???????
}
/*
void FU::Scheduling(bool SchedulerFlag)
{
	if (Modeling == nullptr) return;
	if (Modeling->qmk.size() != 0)
	{
		Modeling->SchedulerFlag = SchedulerFlag;
		if (Modeling->qmk.size() == 0)
			cout << "Modeling error\n";
		else
		{
			ip t = Modeling->qmk.back();
			Modeling->qmk.pop_back();
			ProgFU(t.atr, t.Load, this);
			if (t.Load.Type % 2 == 1 && t.Load.Point != nullptr)
				t.Load.VarDel();
		}
	}
}
*/

void FU::Scheduling(bool SchedulerFlag)
{
	if (Modeling == nullptr)
	{
		cout << "Modeling error: Modeling=null\n";
		return;
	}
	Modeling->SchedulerFlag = SchedulerFlag;
	if (SchedulerFlag) {
		if (Modeling->qmk.size() != 0)
		{
			if (Modeling->qmk.size() == 0)
				cout << "Modeling error: qmk.size() = 0\n";
			else
			{
				ipSender t = Modeling->qmk.back();
				Modeling->qmk.pop_back();
				ProgFU(t.atr, t.Load, t.Sender);
				if (t.Load.Type % 2 == 1 && t.Load.Point != nullptr)
					t.Load.VarDel();
			}
		}
	}
	else
	{// MkReceive awaiting Приход ожидаемой МК
		if (Modeling->qAwaitMk.size() != 0)
		{
			if (Modeling->qAwaitMk.size() == 0)
				cout << "Modeling error: qAwaitMk.size() = 0\n";
			else
			{
				ipSender t = Modeling->qAwaitMk.begin()->second;
				Modeling->qAwaitMk.erase(Modeling->qAwaitMk.begin());
				ProgFU(t.atr, t.Load, t.Sender);
				if (t.Load.Type % 2 == 1 && t.Load.Point != nullptr)
					t.Load.VarDel();
			}
		}
	}
}

void FUModeling::EventModelingPrint() // Вывести состояние моделирования ФУ
{
	cout << "qmk: " << endl;
	for (auto i : qmk)
		cout << i.atr << "=" << i.Load.Type << endl;
	cout << "qAwaitMk: " << endl;
	for (auto i : qAwaitMk)
		cout << "Time: " << i.first << " MK: "<< i.second.atr << "=" << i.second.Load.Type << endl;
}

bool LoadCmp(LoadPoint x, LoadPoint y) // Сравнение двух нагрузок ИП
{
	// Доработать с указателями на ИК и ИП
	if (x.Point == nullptr || y.Point == nullptr)
		return true;
	int xt = x.Type >> 1, yt = y.Type >> 1;
	if ((xt == Dint || xt == Dfloat || xt == Ddouble || xt == Dchar) && (yt == Dint || yt == Dfloat || yt == Ddouble || yt == Dchar))
		switch (xt)
		{
		case Dint:
			switch (yt)
			{
			case Dint: return *(int*)x.Point == *(int*)y.Point;
			case Dfloat: return *(int*)x.Point == *(float*)y.Point;
			case Ddouble: return *(int*)x.Point == *(double*)y.Point;
			case Dchar: return *(int*)x.Point == *(char*)y.Point;
			}
		case Dfloat:
			switch (yt)
			{
			case Dint: return *(float*)x.Point == *(int*)y.Point;
			case Dfloat: return *(float*)x.Point == *(float*)y.Point;
			case Ddouble: return *(float*)x.Point == *(double*)y.Point;
			case Dchar: return *(float*)x.Point == *(char*)y.Point;
			}
		case Ddouble:
			switch (yt)
			{
			case Dint: return *(double*)x.Point == *(int*)y.Point;
			case Dfloat: return *(double*)x.Point == *(float*)y.Point;
			case Ddouble: return *(double*)x.Point == *(double*)y.Point;
			case Dchar: return *(double*)x.Point == *(char*)y.Point;
			}
		case Dchar:
			switch (yt)
			{
			case Dint: return *(char*)x.Point == *(int*)y.Point;
			case Dfloat: return *(char*)x.Point == *(float*)y.Point;
			case Ddouble: return *(char*)x.Point == *(double*)y.Point;
			case Dchar: return *(char*)x.Point == *(char*)y.Point;
			}
		}
	if (xt == Dbool && yt == Dbool)
		return *(bool*)x.Point == *(bool*)y.Point;
	if (xt == Dstring && yt == Dstring)
		return *(string*)x.Point == *(string*)y.Point;

	return false;
}

bool IPCmp(ip* x, ip* y) // Сравнение двух  ИП
{
	if (!LoadCmp(x->Load, y->Load))
		return false;
	else
		if (x->atr == y->atr || x->atr == 0 || y->atr == 0)
			return true;
		else
			return false;
}

// Работа с ИК
bool AtrSearch(void* uk, long int Atr) // Поиск атриубута в ИК
{
	for (auto& i : *(IC_type)uk)
		if (i.atr == Atr)
			return true;
	return false;
}

int AtrCounter(void* uk, long int Atr) // Подсчет количества ИП с заданнным атриубутом в ИК
{
	int c = 0;
	for (auto& i : *(IC_type)uk)
		if (i.atr == Atr)
			c++;
	return c;
}

void ICDel(void* Uk) // Удаление ИК
{
	for (auto&& i : *(IC_type)Uk)
		i.Load.Clear();
	delete (IC_type)Uk;
}

void ICDel(LoadPoint& Uk)// Удаление ИК
{
	if (!Uk.isIC()) return;
	ICDel(Uk.Point);
	Uk = { 0,nullptr };
}

LoadPoint ICCopy(LoadPoint uk, bool Copy) // Копирование ИК (Copy - флаг принудительного копирования всех ИК в ОА-графе)
{
	IC_type CapsNew = new vector<ip>;
	if (uk.Type >> 1 == DIP) // Если передается ИП
	{
		CapsNew->push_back(*(*(ip*)uk.Point).Clone());
		return { uk.Type, CapsNew };
	}
	if (uk.Point == nullptr || !uk.isIC()) // non-IC payloads cannot be safely cast to vector<ip>*; ICCopy is IC-only
		return { uk.Type, CapsNew };
	IC_type Uk = (IC_type)uk.Point;
	CapsNew->resize(((IC_type)Uk)->size());
	for (auto i = ((IC_type)Uk)->begin(), j = CapsNew->begin(); j != CapsNew->end(); i++, j++)
	{
		j->atr = i->atr;
		if (Copy and i->Load.isIC())
			j->Load = ICCopy(i->Load, true);
		//j->Load.Copy({ i->Load.Type | 1, i->Load.Point, i->Load.Ind }); // Преобразовать к константе, чтобы произошло копирование
		else
			j->Load.Copy(i->Load);
	}
	return { uk.Type, CapsNew };
}

void* GraphCopy(void* Uk, LocatTable* Table = nullptr) // Копирование ОА-графа
{
	return nullptr;
}

void GraphDel(void* Uk, LocatTable* Table = nullptr) // Удаление ОА-графа
{
	return;
}
// ------------------------



// Найти в ИК ИП с атрибутом Atr и выполнить программу либо по адр. в нагрузке, либо после найденной ИП, если атрибут не найден, возвращается true
bool AtrProgExec(vector<ip>* Prog, long int Atr, FU* Bus, bool AfterContinue)
{
	auto i = Prog->begin();
	for (; i != Prog->end() && i->atr != Atr; i++);
	if (i != Prog->end())
	{
		if (i->Load.Point != nullptr)
			Bus->ProgExec((vector<ip> *)i->Load.Point);
		if ((++i) != Prog->end() && AfterContinue)
			Bus->ProgExec(Prog, 0, nullptr, &i);
		return true;
	}
	return false;
}

ip* AtrSearchIP(void* IC, long int Atr) // Поиск в ИК ИП с заданным атрутом
{
	if (IC == nullptr) return nullptr;
	auto uk = (*(IC_type)IC).begin();
	for (; uk != ((IC_type)IC)->end() && uk->atr != Atr; uk++);
	if (uk == ((IC_type)IC)->end())
		return nullptr;
	else
		return uk._Ptr;
	return nullptr;
}

bool isIPinIC(void* iP, void* iC) //проверка, что ИП входит в ИК
{
	for (auto i = ((IC_type)iC)->begin(); i != ((IC_type)iC)->end(); i++) {
		if (IPCmp(i._Ptr, ((ip*)iP)))
			return true;
	}
	return false;
}

void ICCopyConcat(void* uk, void* uk2) // Конкатенация двух ИК
{
	//copy( ((IC_type)uk2)->begin(), ((IC_type)uk2)->end(), inserter(*((IC_type)uk), ((IC_type)uk)->end()));
	for (auto& i : *(IC_type)uk2)
		((IC_type)uk)->push_back(i.Copy());
}

int ICLen(void* uk) // Определитель длины ИК
{
	return (((IC_type)uk)->size());
}

vector<ip>::iterator IPSearch(void* ic, ip IP) // Поиск ИП в ИК (возвращается указатель на персую найденную ИП
{
	for (auto i = ((IC_type)ic)->begin(); i != ((IC_type)ic)->end(); i++)
		if (IPCmp(i._Ptr, &IP))
			return i;
	return ((IC_type)ic)->end();
}

vector<ip>::iterator IPSearch(void* ic, LoadPoint IP) // Поиск ИП в ИК (возвращается указатель на персую найденную ИП
{
	if (IP.isIP())
		return IPSearch(ic, *((ip*)IP.Point));
	else if (IP.isIC())
		return IPSearch(ic, *((IC_type)IP.Point)->begin());
	return ((IC_type)ic)->end();
}
void IPAdd(void* IC, ip IP) // Добавить ИП в конец ИК
{
	((vector<ip>*)IC)->push_back(IP.Copy());
}

void IPAdd(LoadPoint IC, ip IP) // Добавить ИП в конец ИК
{
	if (!IC.isIC())return;
	IPAdd(IC.Point, IP);
}

void* MakeLoadFromDouble(double x, unsigned int Type) // Создать нагрузку из типа double
{
	if (Type > 0) // Задан тип
	{
		LoadPoint P = { Type, nullptr };
		if (!P.isDigitBool()) return nullptr;
	}
	else // Автоматическое определерние типа
	{
		if (x == 0 || x == 1)
			Type = Tbool;
		else if (x == int(x))
			Type = Tint;
		else
			Type = Tdouble;
	}

	switch (Type >> 1)
	{
	case Dbool:
		return new bool((bool)x);
		break;
	case Dint:
		return new int((int)x);
		break;
	case Dfloat:
		return new float((float)x);
		break;
	case Ddouble:
		return new double((double)x);
		break;
	}
};

LoadVect_type LoadPoint::toVect() // Возвращает укаазатель на вектор
{
//	if (!isVect()) return nullptr;
	return (LoadVect_type)Point;
}
IC_type LoadPoint::IC() // Возвращает указатель на ИК
{
//	if (!isIC()) return nullptr;
	return (IC_type)Point;
}

LoadPoint LoadNew(long int t) //Создание нагрузки от перененной
{
	return { Cint, (void*)&t };
}
LoadPoint LoadNew(double t) //Создание нагрузки от перененной
{
	return { Cdouble, (void*)&t };
}
LoadPoint LoadNew(bool t) //Создание нагрузки от перененной
{
	return { Cbool, (void*)&t };
}
LoadPoint LoadNew(string t) //Создание нагрузки от перененной
{
	return { Cstring, (void*)&t };
}
LoadPoint LoadNew(float t) //Создание нагрузки от перененной
{
	return { Cfloat, (void*)&t };
}

LoadPoint CopyAdrCorrect(LoadPoint OriginalIC, LoadPoint CopyIC, LoadPoint Adr, set<void*> buf) // Корректировка адреса в копированной ИК (Переборный алгоритм)
{
	if (buf.count(OriginalIC.Point)) return {};
	buf.insert(OriginalIC.Point);
	if (Adr.Point == OriginalIC.Point)
		if (Adr.isICInd())
			return { TICInd + (CopyIC.Type & 1) , CopyIC.Point, Adr.Ind };
		else if(Adr.isVectInd())
			return { TLoadVectInd + (CopyIC.Type & 1) , CopyIC.Point, Adr.Ind };
		else
			return CopyIC;
	for (auto i = ((IC_type)OriginalIC.Point)->begin(), j = ((IC_type)CopyIC.Point)->begin();
		i != ((IC_type)OriginalIC.Point)->end() && j != ((IC_type)CopyIC.Point)->end(); i++, j++)
	{
		if (i._Ptr == Adr.Point) return { TIP, j._Ptr };
		if (&(i->atr) == Adr.Point) return { Tint, &(j->atr) };
		if (&(i->Load) == Adr.Point) return { TLoad, &(j->Load) };
//		if (i->Load.Point == Adr.Point) return j->Load;
		if (i->Load.isIC())
		{
			LoadPoint LP = CopyAdrCorrect(i->Load, j->Load, Adr, buf);
			if (LP.Point == nullptr)
				continue;
			return LP;
		}
	}
	return {};
}

string LoadMnemoToStr::LoadConv(LoadPoint Load) // Перевод в текст нагрузки
{
	if(Load.isNil()) return "";
	if (Load.isAtrMk()) return AtrConv(Load.toInt());
	//if (Load.isInt())return AtrConv(Load.toInt()); // !!! Заглушка
	return Load.toStr(); // Фесли не распознано
}

string LoadMnemoToStr::AtrConv(long Atr)           //  Перевод в текст Атрибута
{
	if ((void*)MnemoList == nullptr) return to_string(Atr);
	bool ProgsBlock = MnemoList->ProgsBlock;
	MnemoList->ProgsBlock = true; // Заблокировать выполнение всех программа для ФУ поиска лексем
	if (Atr < 0) { // Атрибут
		LoadPoint LP = {0,nullptr};
		void* UK=nullptr;
		vector<ip> IP = { { _AtrAtr, {Cint, &Atr} } };
		vector<ip> IP2 = { { _MnemoAtr, {0, nullptr} }};
		MnemoList->ProgFU(_ListFindAndMk, { CIC, (void*)&IP });
		MnemoList->ProgFU(_ListFindAndLineMk, { CIC, (void*)&IP2 });
//		MnemoList->ProgFU(415, { TIP, &UK });
		MnemoList->ProgFU(_ListLoadOutMk, { TLoad, &LP });
		if (LP.isNil()) return to_string(Atr);
		MnemoList->ProgsBlock = ProgsBlock;
		return LP.toStr();
	}
	else // Милликоманда
	{
		long Mk = Atr;
		long ShortMk = Mk % MkRange;
		long MkRangeStart = Mk - ShortMk;
		MnemoList->ProgsBlock = true;
		vector<ip> IP = { { _FUMkBegRangeAtr, {Cint, &MkRangeStart} } };
		vector<ip> IPMkList = { { _MkListAtr, {0, nullptr} } };
		vector<ip> IPMk = { { _MkAtr, {Cint, &ShortMk} } };
		vector<ip> IPMnemo = { { _MnemoAtr, {0, nullptr} } };
		string FuName, MkName;
		// Поиск Атрибута Диапазона МК
		LoadPoint Uk = { 0, nullptr };
		MnemoList->ProgFU(_ListFindAndMk, { CIC, &IP });
		MnemoList->ProgFU(_ListLoadOutMk, { TLoad, &Uk });
		if (Uk.Point == nullptr) return to_string(Atr);
		// Поиск имени ФУ
		Uk = { 0, nullptr };
		MnemoList->ProgFU(_ListFindAndLineMk, { CIC, &IPMnemo });
		MnemoList->ProgFU(_ListLoadOutMk, { TLoad, &Uk });
		if (Uk.Point == nullptr) return to_string(Atr);
		FuName=Uk.toStr(); // Запомнить название ФУ

		// Поиск списка списка МК
		Uk = { 0, nullptr };
		MnemoList->ProgFU(_ListFindAndLineMk, { CIC, &IPMkList });
		MnemoList->ProgFU(_ListLoadOutMk, { TLoad, &Uk });
		if (Uk.Point == nullptr) return to_string(Atr);

		MnemoList->ProgFU(_ListSubDownMk, { CAtr, &_MkListAtr }); // Переход в список МК
		  
		//Поиск МК
		Uk = { 0, nullptr };
		MnemoList->ProgFU(_ListFindAndMk, { CIC, &IPMk });
		MnemoList->ProgFU(_ListLoadOutMk, { TLoad, &Uk });
		if (Uk.Point == nullptr)
		{
			MnemoList->ProgFU(_ListSubUpMk, { 0, nullptr }); // Возвращение к корневому списку
			return to_string(Atr);
		}
		// Поиск мнемоники МК
		Uk = { 0, nullptr };
		MnemoList->ProgFU(_ListFindAndLineMk, { CIC, &IPMnemo });
		MnemoList->ProgFU(_ListLoadOutMk, { TLoad, &Uk });
		if (Uk.Point == nullptr) return to_string(Atr);
		MkName=Uk.toStr(); // Запомнить название Mk

		MnemoList->ProgFU(_ListSubUpMk, { 0, nullptr }); // Возвращение к корневому списку

		MnemoList->ProgsBlock = ProgsBlock;
		return FuName + "." + MkName;
	}
}
	void LoadMnemoToStr::ListSet(LoadPoint Load) // Установить ссылку на список или ФУ списка
	{
		if (Load.isNil()) // Обнуление указателя на ФУ список
		{
			//if (MnemoList != nullptr && !MnemoListExt) delete (List*)MnemoList;
			MnemoList = nullptr;
		}
		else if (Load.isFU()) {
			//if (MnemoList != nullptr && !MnemoListExt) delete (List*)MnemoList;
			MnemoList = (FU*)Load.Point;
		}
//		else if (Load.isIC())
//		{
//			if (MnemoList == nullptr)
//				MnemoList = new List(Bus, nullptr);
//			MnemoList->ProgFU(1, Load, nullptr); // Установка списка лексем в ФУ списка
//		}
	}

