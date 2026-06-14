// ФУ Консоль
#include "stdafx.h"
#include "Console.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <io.h>      // _isatty/_fileno: gate ANSI colors on a TTY
#include <regex>
#include <iomanip>

using namespace std;
/* Коды цветов для ANSI-совместимых терминалов:
Black: 30
Red: 31
Green: 32
Yellow: 33
Blue: 34
Magenta: 35
Cyan: 36
White: 37
Reset: 0
*/
static const char* Clr(const char* code) { static bool tty = _isatty(_fileno(stdout)) != 0; return tty ? code : ""; }

void Console::LoadPrint(LoadPoint Load, string offset) // Печать нагрузки
{
	map<void*, int>* AdrMap=nullptr;
	LoadPoint LP = Load.IndLoadReturn();
	if (LP.Point == nullptr)
	{
		out() << "null";
		return;
	}
	switch (LP.Type>>1)
	{
	case DAtr:	  out() << Clr("\033[1;35m") << MnemoToStr.AtrConv(Load.toInt()) << Clr("\033[0m"); break;
	case DMk:     out() << Clr("\033[1;32m") << MnemoToStr.AtrConv(Load.toInt()) << Clr("\033[0m"); break;
	case Dstring: out() << Clr("\033[1;33m") << quote << LP.toStr() << quote << Clr("\033[0m"); break;
	case Dint:	  out() << LP.toInt(); break;
	case Dfloat:  out() << LP.toFloat(); break;
	case Ddouble: out() << LP.toDouble(); break;
	case Dchar:   out() << LP.toChar(); break;
	case Dbool:   out() << LP.toBool(); break;
	case DIP:
	case DIC:
	{
		if (LP.Type >> 1 == DIP){
		
			out() << MnemoToStr.AtrConv(((ip*)LP.Point)->atr) << ((((ip*)LP.Point)->Load.Type % 2 == 0) ? " = " : " # ");
			out() << MnemoToStr.LoadConv(((ip*)LP.Point)->Load);
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
			out() << offset << "IC id: " << (*AdrMap)[LP.Point] << endl;
			break;
		}
		(*AdrMap)[LP.Point] = AdrMap->size(); // Запомнить пройденную ИК для избежания зацикливания

		if (!((IC_type)LP.Point)->size()) {
			out() << offset << "Empry IC";
			return;
		}
		for (auto i = ((IC_type)LP.Point)->begin(); i != ((IC_type)LP.Point)->end(); i++)
		{
			if (i->Load.Type >> 1 == DIP || i->Load.isIC())
				out() << offset <<MnemoToStr.AtrConv(i->atr)<< " ->\n";
			else
				if (AtrMnemo.count(i->atr))
					out() << offset << MnemoToStr.AtrConv(i->atr) << ((i->Load.Type % 2) ? " # " : " = ");
				else
					out() << offset << MnemoToStr.AtrConv(i->atr) << ((i->Load.Type % 2) ? " # " : " = ");
			LoadPrint(i->Load, offset + "  "); // i->Load.print(AtrMnemo, offset + "  ", Sep, End, quote, ArrayBracketStart, ArrayBracketFin, VectCol, AdrMap);
			if (i != ((IC_type)LP.Point)->end() - 1)
				out() << endl;
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
		out() << ArrayBracketStart;
		register int c = 1;
		for (auto i : *(vector<LoadPoint>*)LP.Point)
		{
			if (VectCol > 0 && c > 1 && (c - 1) % VectCol == 0)
				out() << End;
			//i.print(AtrMnemo, offset, Sep, End, quote, ArrayBracketStart, ArrayBracketFin);
			LoadPrint(LP,offset);
			if (c < ((vector<LoadPoint>*)
				LP.Point)->size()) out() << Sep;
			c++;
		}
		out() << ArrayBracketFin << endl;
		break;
	}
	case TLoadVectInd:
	case CLoadVectInd: // Вектор нагрузок
	{
		register int i = LP.Ind;
		out() << "Vect Ind[" << LP.Ind << "] ";
		if (((LoadVect_type)LP.Point)->size() > abs(i) or -i == ((LoadVect_type)LP.Point)->size())
			((LoadVect_type)LP.Point)->at(i).print(AtrMnemo, offset, Sep, End, quote, ArrayBracketStart, ArrayBracketFin);
		break;
	}
	default:
		if (LP.Type >= 2000) // Печать матрицы и вектора
		{
			LP.MatrixPrint(LP.Type, LP.Point, AtrMnemo, offset, Sep, End, ArrayBracketStart, ArrayBracketFin);
		}
		else if (LP.Type >= 1000)
			LP.VectorPrint(LP.Type, LP.Point, AtrMnemo, offset, Sep, End, ArrayBracketStart, ArrayBracketFin);
		break;
	}
}

void Console::ReportError(const string& where, const string& msg) {
	cerr << "[Console::" << where << "] " << msg << endl;
}

void Console::ParseTemplate() {
	Tokens.clear();
	string textBuff;

	for (int i = 0; i < (int)Template.size(); ++i) {
		if (Template[i] != '{') {
			textBuff += Template[i];
			continue;
		}

		if (!textBuff.empty()) {
			Tokens.push_back({ false, textBuff, -1, {} });
			textBuff.clear();
		}

		++i; 

		if (i >= (int)Template.size()) {
			ReportError("ParseTemplate", "'{' is not closed at the of the template");
			break;
		}

		FormatToken token;
		token.isIndex = true;

		string num;
		while (i < (int)Template.size() && isdigit(Template[i])) {
			num += Template[i];
			++i;
		}

		if (num.empty()) {
			ReportError("ParseTemplate", "no index after '{'  in pose " + to_string(i));
		}
		else {
			token.index = stoi(num);
		}

		if (i < (int)Template.size() && Template[i] == ':') {
			++i;

			while (i < (int)Template.size() && Template[i] != '}') {
				char c = Template[i];

				if (c == '0') {
					token.spec.zeroes = true;
				}
				else if (isdigit(c)) {
					token.spec.width = token.spec.width * 10 + (c - '0');
				}
				else if (c == '.') {
					++i;
					string p;
					while (i < (int)Template.size() && isdigit(Template[i])) {
						p += Template[i];
						++i;
					}
					if (p.empty())
						ReportError("ParseTemplate", "no precision after'.'");
					else
						token.spec.precision = stoi(p);
					continue;
				}
				else if (c == '<' || c == '>' || c == '^') {
					token.spec.align = c;
				}
				else if (c == '#') {
					token.spec.alt = true;
				}
				else if (c == 'x' || c == 'o' || c == 'b') {
					token.spec.altType = c;
				}
				else {
                    ReportError("ParseTemplate", string("unknown specificator '") + c + "'");
                }
				++i;
			}
		}

		if (i >= (int)Template.size() || Template[i] != '}') {
			ReportError("ParseTemplate", "no '}' for index " + to_string(token.index));
		}
		else {
			++i;
		}

		--i;

		Tokens.push_back(token);
	}

	if (!textBuff.empty())
		Tokens.push_back({ false, textBuff, -1, {} });
}

string Console::FormatValue(LoadPoint& lp, FormatSpec& spec) {
	string result = lp.toStr();

	if (spec.alt) { //Альтернативные системы счисления
		long int v = lp.toInt();

		if (spec.altType == 'x') {
			stringstream ss;
			ss << hex << v;
			result = ss.str();
		}

		else if (spec.altType == 'o') {
			stringstream ss;
			ss << oct << v;
			result = ss.str();
		}

		else if (spec.altType == 'b') {
			result = "";

			while (v) {
				result = char('0' + (v % 2)) + result;
				v /= 2;
			}
		}
	}

	if (spec.precision >= 0) { //Точность для float/double
		double v = lp.toDouble();
		stringstream ss;
		ss << fixed << setprecision(spec.precision) << v;
		result = ss.str();
	}

	if (spec.width > result.size()) { //Выравнивание и строка заполнения
		int pad = spec.width - result.size();
		string fill(pad, spec.zeroes ? '0' : ' ');

		if (spec.align == '<') 
			result = result + fill;
		else
			result = fill + result;
	}

	return result;
}

void Console::ExecuteTemplate() {
	Buffer.clear();

	for (auto& t : Tokens) {
		if (!t.isIndex) {
			Buffer += t.text;
			continue;
		}

		if (t.index < 0 || t.index >= (int)Operands.size()) {
			ReportError("ExecuteTemplate", "index {" + to_string(t.index) + "} out of range (total operands: " + to_string(Operands.size()) + ")");
			continue;
		}

		if (Operands[t.index].Point == nullptr) {
			ReportError("ExecuteTemplate", "operand {" + to_string(t.index) + "} havent set (Point == nullptr)");
			continue;
		}

		Buffer += FormatValue(Operands[t.index], t.spec);
	}

	if (!Quiet) out() << Buffer;;
}

std::ostream& Console::out() { return fout.is_open() ? static_cast<std::ostream&>(fout) : std::cout; }

void Console::ProgFU(long int MK, LoadPoint Load, FU* Sender)
{
	MK %= FUMkRange; // Оставить только свои МК
	switch (MK)
	{
	case 0: // Reset
		AtrMnemo.clear();
		break;
	case 1: // Out Вывод
	case 2: // OutLn Вывод и перевод строки
	case 3: // LnOut Перевод строки и вывод
	case 4: // LnOutLn Перевод строки, вывод и снова перевод строки
		if (Quiet) break;
		out() << prefix;
		if (MK == 3 || MK == 4) out() << endl;
		if (Load.Point != nullptr)
			LoadPrint(Load);
		//Load.print(AtrMnemo,"",Sep,End, quote, ArrayBracketStart,ArrayBracketFin, VectCol);
		if (MK == 2 || MK == 4) out() << endl;
		break;
	case 5: // LoadInfoOut Вывести сведения о нагрузке
		out() << "LoadInfo : Type " << Load.Type << " Ind " << Load.Ind;
		if (Load.Point == nullptr) out() << " Point=null";
		else out() << Load.Type % 2 ? " Const" : "Var";
		out() << endl;
		break;
	case 10: // Ln Перевод строки
		out() << endl;
		break;
	case 15: //SepSet Установить строку-разделитель
		Sep = Load.toStr();
		break;
	case 16: //EndSet Установить строку в конце вывода
		End = Load.toStr();
		break;
	case 17: // ArrayBracketStartSet Установить строку, обозначающую открывающуюся скобку при вывод вектора
		ArrayBracketStart = Load.toStr();
		break;
	case 18: // ArrayBracketStartSet Установить строку, обозначающую закрывающуюся скобку при вывод вектора
		ArrayBracketFin = Load.toStr();
		break;
	case 19: // QuoteSet Установить символ кавычек при воводе строки или символа
		quote = Load.toStr();
		break;
	case 20: // PrefixSet Установить префикс перед выводом
		if ((Load.Type) >> 1 == Dstring)
			prefix = *(string*)Load.Point;
		break;
	case 21: // VectColSet Установить количество колонок при выводе вектора
		VectCol = Load.toInt();
		break;
	case 25: // FileNameSet
		filename = Load.toStr();
		break;
	case 30: // OutFileSet Установить файл для вывода (при пустой нагрузке вывод на консоль)
		if (Load.Point == nullptr)
			{ if (fout.is_open()) fout.close(); }
		else
			{ if (fout.is_open()) fout.close(); fout.open(Load.toStr()); }
		break;

	case 31: // StdOutFileAppend Установить файл для дополнения
		if (Load.Point == nullptr)
			{ if (fout.is_open()) fout.close(); }
		else
			{ if (fout.is_open()) fout.close(); fout.open(Load.toStr(), ios::app); }
		break;

	case 35: // StdInFileSet Установить файл для ввода
		if (Load.Point == nullptr)
			//			std::istream& in == cin;
			;
		else
			freopen_s(&streamIn, Load.toStr().c_str(), "r", stdin);
		break;
	case 40: // AtrMnemoAdd Добавить мнемоники атрибутов
		if (Load.Type >> 1 == DIP)
			AtrMnemo[((ip*)Load.Point)->atr] = ((ip*)Load.Point)->Load.toStr();
		else if (Load.isIC())
			for (auto& i : *(IC_type)Load.Point)
				AtrMnemo[i.atr] = i.Load.toStr();
		break;
	case 41: // AtrMnemoClear Очистить мнемоники атрибутов
		AtrMnemo.clear();
		break;

	case 44: // TrueValSet
		TrueVar.clear();
		if (Load.Point != nullptr)
			TrueVar.push_back(Load.toStr());
		break;
	case 45: // TrueValAdd
		if (Load.Point != nullptr)
			TrueVar.push_back(Load.toStr());
		break;
	case 46: // TrueValSet
		FalseVar.clear();
		if (Load.Point != nullptr)
			FalseVar.push_back(Load.toStr());
		break;
	case 47: // TrueValAdd
		if (Load.Point != nullptr)
			FalseVar.push_back(Load.toStr());
		break;

	case 50: //VectIn ввод вектора 
		break;
	case 55: //MatrIn ввод матрицы	
		break;
	case 60: //VarClear Очистить буфер адресов для результата ввода
		VarOutBuf.clear();
		break;
	case 61: //VarSet Записать адрес переменной для записи результата ввода
		VarOutBuf.clear();
		VarOutBuf.push_back({ -1, Load });
		break;
	case 62: //VarAdd Добавить адрес переменной для записи результата ввода
		VarOutBuf.push_back({ -1, Load });
		break;
	case 70: //TemplSet Установить шаблон для вывода
		Template = Load.toStr();
		break;
	case 80: //VarOut – выдать адрес переменной
		Load.Write(Var);
		break;
	case 85: // VarOutMk
		MkExec(Load, Var);
		break;
	case 86: // InStrOut Выдать последнюю введенную строку
		Load.Write(inStr);
		break;
	case 87: // InStrOutMk  Выдать МК с последней введенной строкой
		MkExec(Load, { Cstring, &inStr });
		break;
	case 90: // TrueFalseClear Очистить буфер наименований true и false
		False.clear();
		break;
	case 91: // TrueAdd
		True.insert(Load.toStr());
		break;
	case 92: // FalseAdd
		False.insert(Load.toStr());
		break;
	case 94: // InputErrProgSet Установить программу обработки ошибки неправильного формата входных данных
		InputFormatErrProg = Load.Point;
	case 100: // Input Ввод данных
	case 101: // InputMk Ввод данных и выдача МК с ними
	{
		std::cmatch results;
		regex regular_str("[\\D\\s]+");
		regex regular_int("([\\d]+)");
		regex regular_float("\\d+\\.\\d+");
		regex regular_true("[Tt][Rr][Uu][Ee]");
		regex regular_false("[Ff][Aa][Ll][Ss][Ee]");
		regex regular_vector("^\\[(\\d+,?\\s?)+\\]$"); //интовый вектор
		regex regular_matrix("^\\[(\\d+[,;]?\\s?)+\\]$"); //интовая матрица
		regex regular_char("\\w{1}");

		getline(cin, inStr);
		Var.Clear(); // Очистить предыдущее значение
		//	if (MK == 101 && Load.Point != nullptr) // Выдать МК
		{
			if (std::regex_match(inStr.c_str(), regular_float)) {


				//out() << "its float";
				double res = stof(inStr);
				Var = { Cdouble,new double(res) };
			}
			else  if (std::regex_match(inStr.c_str(), regular_int)) {

				//out() << "its int";
				long int res = stoi(inStr);
				Var = { Cint,new long int(res) };
			}
			else if (std::regex_match(inStr.c_str(), regular_true)) {

				Var = { Cbool, new bool(true) };

			}
			else if (std::regex_match(inStr.c_str(), regular_false)) {

				Var = { Cbool, new bool(false) };

			}
			else if (std::regex_match(inStr.c_str(), regular_char)) {

				//out() << "its char";
				char res;
				res = inStr[0];
				Var = { Cchar,new char(res) };
			}
			else if (std::regex_match(inStr.c_str(), regular_str)) {

				///out() << "its string";
				Var = { Cstring,new string(inStr) };
			}
			else if (std::regex_match(inStr.c_str(), regular_vector)) {

				long int res;
				long int counter = -1;
				long int* intbufArray = new long int[256];
				vector<LoadPoint> Ar;
				long int i = 0;
				for (sregex_iterator it = sregex_iterator(inStr.begin(), inStr.end(), regular_int);
					it != sregex_iterator(); it++) {
					smatch match = *it;
					counter++;
					res = stoi(match.str(0));
					intbufArray[i] = res;
					i++;
				}
				long int* intArray = new long int[counter];

				for (int i = 0; i < counter; i++) {
					intArray[i] = intbufArray[i];
				}
				delete[] intbufArray;
				//delete[] intArray;

				Var = { CintArray, new long int* (intArray) };
			}
			// Распознание вектора
			/*
			else if (std::regex_match(inStr.c_str(), regular_vector)) {
				int res1;
				float res2;
				std::vector <LoadPoint> Ar;
				for (sregex_iterator it = sregex_iterator(inStr.begin(), inStr.end(), regular_number);
					it != sregex_iterator(); it++) {
					smatch match = *it;
					if (std::regex_match(match.str(0).c_str(), regular_float)) {
						res2 = stof(match.str(0));
						Ar.push_back(res2);
					}
					else if (std::regex_match(match.str(0).c_str(), regular_int)) {
						res1 = stoi(match.str(0));
						Ar.push_back(res1);
					}
				}

			}
			*/
			else if (std::regex_match(inStr.c_str(), regular_matrix)) {

				out() << "its matrix";

			}

			// Опеределяем тип введенных данных
			// Преобразуем в соответсвующий тип и записываем в Var
			// Var={Тип, new Соотвествующий тип данных} Типы данных Cbool, Cint, Cchar, Cdouble, Cstring, CLoadVect
			  // Регулятка - определить тип Var={Cbool, new bool(true)}
			if (MK == 101) // Выдать МК
				MkExec(Load, Var);
			else // Записать в переменную
				Load.WriteFromLoad(Var);
			break;
		}
		// Выдать по списку заранее установленных адресов и МК в VarOutBuf
//		for (auto& i : VarOutBuf)
//		{
//			// Выдача результата ввода на VarOutBuf
//		}
		break;
	}
	case 105: //InputBool Ввод буленова значения
	case 106: //InputBoolMk Ввод буленова значения и выдача МК с ним
		//	if(неправильный формат)
		if (false)
			ProgExec(InputFormatErrProg);
		else
			if (MK == 106) // Выдать МК
				MkExec(Load, Var);
			else // Записать в переменную
				Load.WriteFromLoad(Var);
		break;
	case 110: //InputInt
	case 111: //InputIntMk
		break;
		// ......


	case 200: // NoVarToOutProgSet Установить подрограмму реакции на ошибку "Нет переменной для ввода"
		NoVarToOutProg = Load.Point;
		break;
	case 201: // WrongFormatProgSet Установить подрограмму реакции на ошибку "Неправильный формат входных данных"
		WrongFormatProg = Load.Point;
		break;

	case 290: //TemplSet Установить шаблон для вывода
	{
		Template = Load.toStr();
		// Подсчет количества операндов (чтобы понять, сколько операндов ожидать
		// Operands.resize();
		// Установка счетчика операндов
		// Изменение размера вектора ссылок на операнды
		ParseTemplate(); //Распарсить шаблон

		int maxIndex = -1;
		for (auto& t : Tokens) {
			if (t.isIndex)
				maxIndex = max(maxIndex, t.index);
		}

		if (maxIndex >= 0) {
			Operands.resize(maxIndex + 1);
		}
		else {
			Operands.clear();
		}

		OpCount = 0;
		break;
	}

	case 295: // OpAdd Добавить очередной операнд
		if (OpCount >= (int)Operands.size()) {
			ReportError("OpAdd", "more operands then expected (expected " + to_string(Operands.size()) + ", received " + to_string(OpCount + 1) + ")");
			break;
		}
		if (OpCount < static_cast<int>(Operands.size())) {
			Operands[OpCount++] = Load;
			if (OpCount == (int)Operands.size()) {
				ExecuteTemplate();
				OpCount = 0; // сброс для повторного использования
				for (auto& op : Operands)
					op = { 0, nullptr };
			}
		}
		break;

	case 300: // Op00Set Установить операдн с индексом 0
	{
		int idx = static_cast<int>(MK) - 300;
		if (idx >= (int)Operands.size()) {
			ReportError("Op00Set", "index 0 out of range (total operands: "
				+ to_string(Operands.size()) + ")");
			break;
		}
		Operands[idx] = Load;

		// Проверяем что все операнды установлены
		{
			bool allSet = true;
			for (auto& op : Operands)
				if (op.Point == nullptr) { allSet = false; break; }
			if (allSet)
				ExecuteTemplate();
		}
		break;
	}
	case 301: // Op01Set Установить операдн с индексом 1
	{
		int idx = static_cast<int>(MK) - 300;
		if (idx >= (int)Operands.size()) {
			ReportError("Op00Set", "index 1 out of range (total operands: "
				+ to_string(Operands.size()) + ")");
			break;
		}
		Operands[idx] = Load;

		// Проверяем что все операнды установлены
		{
			bool allSet = true;
			for (auto& op : Operands)
				if (op.Point == nullptr) { allSet = false; break; }
			if (allSet)
				ExecuteTemplate();
		}
		break;
	}
	case 302: // Op02Set Установить операдн с индексом 2
	{
		int idx = static_cast<int>(MK) - 300;
		if (idx >= (int)Operands.size()) {
			ReportError("Op00Set", "index 2 out of range (total operands: "
				+ to_string(Operands.size()) + ")");
			break;
		}
		Operands[idx] = Load;

		// Проверяем что все операнды установлены
		{
			bool allSet = true;
			for (auto& op : Operands)
				if (op.Point == nullptr) { allSet = false; break; }
			if (allSet)
				ExecuteTemplate();
		}
		break;
	}
	case 303: // Op03Set Установить операдн с индексом 3
	{
		int idx = static_cast<int>(MK) - 300;
		if (idx >= (int)Operands.size()) {
			ReportError("Op00Set", "index 3 out of range (total operands: "
				+ to_string(Operands.size()) + ")");
			break;
		}
		Operands[idx] = Load;

		// Проверяем что все операнды установлены
		{
			bool allSet = true;
			for (auto& op : Operands)
				if (op.Point == nullptr) { allSet = false; break; }
			if (allSet)
				ExecuteTemplate();
		}
		break;
	}
	case 304: // Op04Set Установить операдн с индексом 4
	{
		int idx = static_cast<int>(MK) - 300;
		if (idx >= (int)Operands.size()) {
			ReportError("Op00Set", "index 4 out of range (total operands: "
				+ to_string(Operands.size()) + ")");
			break;
		}
		Operands[idx] = Load;

		// Проверяем что все операнды установлены
		{
			bool allSet = true;
			for (auto& op : Operands)
				if (op.Point == nullptr) { allSet = false; break; }
			if (allSet)
				ExecuteTemplate();
		}
		break;
	}
	case 305: // Op05Set Установить операдн с индексом 5
	{
		int idx = static_cast<int>(MK) - 300;
		if (idx >= (int)Operands.size()) {
			ReportError("Op00Set", "index 5 out of range (total operands: "
				+ to_string(Operands.size()) + ")");
			break;
		}
		Operands[idx] = Load;

		// Проверяем что все операнды установлены
		{
			bool allSet = true;
			for (auto& op : Operands)
				if (op.Point == nullptr) { allSet = false; break; }
			if (allSet)
				ExecuteTemplate();
		}
		break;
	}
	case 306: // Op06Set Установить операдн с индексом 6
	{
		int idx = static_cast<int>(MK) - 300;
		if (idx >= (int)Operands.size()) {
			ReportError("Op00Set", "index 6 out of range (total operands: "
				+ to_string(Operands.size()) + ")");
			break;
		}
		Operands[idx] = Load;

		// Проверяем что все операнды установлены
		{
			bool allSet = true;
			for (auto& op : Operands)
				if (op.Point == nullptr) { allSet = false; break; }
			if (allSet)
				ExecuteTemplate();
		}
		break;
	}
	case 307: // Op07Set Установить операдн с индексом 7
	{
		int idx = static_cast<int>(MK) - 300;
		if (idx >= (int)Operands.size()) {
			ReportError("Op00Set", "index 7 out of range (total operands: "
				+ to_string(Operands.size()) + ")");
			break;
		}
		Operands[idx] = Load;

		// Проверяем что все операнды установлены
		{
			bool allSet = true;
			for (auto& op : Operands)
				if (op.Point == nullptr) { allSet = false; break; }
			if (allSet)
				ExecuteTemplate();
		}
		break;
	}
	case 308: // Op08Set Установить операдн с индексом 8
	{
		int idx = static_cast<int>(MK) - 300;
		if (idx >= (int)Operands.size()) {
			ReportError("Op00Set", "index 8 out of range (total operands: "
				+ to_string(Operands.size()) + ")");
			break;
		}
		Operands[idx] = Load;

		// Проверяем что все операнды установлены
		{
			bool allSet = true;
			for (auto& op : Operands)
				if (op.Point == nullptr) { allSet = false; break; }
			if (allSet)
				ExecuteTemplate();
		}
		break;
	}
	case 309: // Op09Set Установить операдн с индексом 9
	{
		int idx = static_cast<int>(MK) - 300;
		if (idx >= (int)Operands.size()) {
			ReportError("Op00Set", "index 0 out of range (total operands: "
				+ to_string(Operands.size()) + ")");
			break;
		}
		Operands[idx] = Load;

		// Проверяем что все операнды установлены
		{
			bool allSet = true;
			for (auto& op : Operands)
				if (op.Point == nullptr) { allSet = false; break; }
			if (allSet)
				ExecuteTemplate();
		}
		break;
	}
	case 310: // Op10Set Установить операдн с индексом 10
	{
		int idx = static_cast<int>(MK) - 300;
		if (idx >= (int)Operands.size()) {
			ReportError("Op00Set", "index 10 out of range (total operands: "
				+ to_string(Operands.size()) + ")");
			break;
		}
		Operands[idx] = Load;

		// Проверяем что все операнды установлены
		{
			bool allSet = true;
			for (auto& op : Operands)
				if (op.Point == nullptr) { allSet = false; break; }
			if (allSet)
				ExecuteTemplate();
		}
		break;
	}
	case 311: // Op11Set Установить операдн с индексом 11
	{
		int idx = static_cast<int>(MK) - 300;
		if (idx >= (int)Operands.size()) {
			ReportError("Op00Set", "index 11 out of range (total operands: "
				+ to_string(Operands.size()) + ")");
			break;
		}
		Operands[idx] = Load;

		// Проверяем что все операнды установлены
		{
			bool allSet = true;
			for (auto& op : Operands)
				if (op.Point == nullptr) { allSet = false; break; }
			if (allSet)
				ExecuteTemplate();
		}
		break;
	}
	case 312: // Op12Set Установить операдн с индексом 12
	{
		int idx = static_cast<int>(MK) - 300;
		if (idx >= (int)Operands.size()) {
			ReportError("Op00Set", "index 12 out of range (total operands: "
				+ to_string(Operands.size()) + ")");
			break;
		}
		Operands[idx] = Load;

		// Проверяем что все операнды установлены
		{
			bool allSet = true;
			for (auto& op : Operands)
				if (op.Point == nullptr) { allSet = false; break; }
			if (allSet)
				ExecuteTemplate();
		}
		break;
	}
	case 313: // Oз13Set Установить операдн с индексом 13
	{
		int idx = static_cast<int>(MK) - 300;
		if (idx >= (int)Operands.size()) {
			ReportError("Op00Set", "index 13 out of range (total operands: "
				+ to_string(Operands.size()) + ")");
			break;
		}
		Operands[idx] = Load;

		// Проверяем что все операнды установлены
		{
			bool allSet = true;
			for (auto& op : Operands)
				if (op.Point == nullptr) { allSet = false; break; }
			if (allSet)
				ExecuteTemplate();
		}
		break;
	}
	case 314: // Op14Set Установить операдн с индексом 14
	{
		int idx = static_cast<int>(MK) - 300;
		if (idx >= (int)Operands.size()) {
			ReportError("Op00Set", "index 14 out of range (total operands: "
				+ to_string(Operands.size()) + ")");
			break;
		}
		Operands[idx] = Load;

		// Проверяем что все операнды установлены
		{
			bool allSet = true;
			for (auto& op : Operands)
				if (op.Point == nullptr) { allSet = false; break; }
			if (allSet)
				ExecuteTemplate();
		}
		break;
	}
	case 315: // Op15Set Установить операдн с индексом 15
	{
		int idx = static_cast<int>(MK) - 300;

		if (Operands.empty()) {
			ReportError("Op__Set", "template havent setted, operand " + to_string(idx) + " ignoring");
			break;
		}
		if (idx >= (int)Operands.size()) {
			ReportError("Op__Set", "index " + to_string(idx) + " out of range (total operands: " + to_string(Operands.size()) + ")");
			break;
		}
		if (idx < static_cast<int>(Operands.size())) {
			Operands[idx] = Load;

			bool allSet = true;
			for (auto& op : Operands) {
				if (op.Point == nullptr) { allSet = false; break; }
			}
			if (allSet)
				ExecuteTemplate();
		}
		break;
	}

	case 400: // MnemoTableSet Установить ссылку на ФУ мнемоник или список мнемоник
		if (Load.isFU()){
			MnemoToStr.MnemoList = (FU*)Load.Point;
		MnemoToStr.MnemoListExt = true; // Установить флаг внешнего ФУ списка лексем
	}
		else {
			MnemoToStr.MnemoList = new List(Bus, nullptr);
			MnemoToStr.MnemoList->ProgFU(1, Load, Sender); // Установка списка лексем в ФУ списка
			MnemoToStr.MnemoListExt = false; // Установить флаг внешнего ФУ списка лексем
		}
		break;
	default:
		CommonMk(MK, Load);
		break;
	}
}

FU* Console::Copy() // Программа копирования ФУ
{
	return new Console(Bus, this);
}

FU* Console::TypeCopy() // Создать ФУ такого же типа (не копируя контекст
{
	return new Console(Bus, nullptr);
}
