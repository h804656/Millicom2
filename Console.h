// �� �������
#pragma once
#include "Consts.h"
#include <fstream>
#include <string>
#include <map>
#include "List.h"

struct FormatSpec
{
	char fill = ' ';
	char align = '>';     
	int width = 0;
	int precision = -1;
	bool alt = false;
	char altType = ' ';   
	bool zeroes = false;
	int zeroesCount = 0;
};

struct FormatToken
{
	bool isIndex = false;
	string text;
	int index = -1;
	FormatSpec spec;
};

class MkRef { // Парочка Мк и ссылка для записи результата
public:
	long int Mk = -1;
	LoadPoint Ref = {0,nullptr};
};

class Console : public FU
{
private:
	void LoadPrint(LoadPoint Load, string offcet = ""); // Печать нагрузки
	LoadMnemoToStr MnemoToStr; // Преобразователь нагрузки в мнемонику и использованием таблицы лексем 
	map<long int, string> AtrMnemo; // Атрибуты мнемоник
	string prefix="";
	string filename = "";
	ofstream fout; // ??
	ofstream fin; // ??
	FILE* streamOut=nullptr, *streamIn=nullptr; // Входной/выходной файлы
	string Sep = " ", VectSep=",", End = "\n"; // Строка-разделитель и строка в конце вывода
	string quote = ""; // Символ кавычек при выводе строки
	string FloatPoint = "."; // Разделитель дробной части
	string ArrayBracketStart = "[", ArrayBracketFin = "]"; // Начальная и конечная скобки для вывода вектора
	vector<string> TrueVar = { "true","True" }; // Вектор значений "правда"
	vector<string> FalseVar = { "false","False" }; // Вектор значений "ложь"
//	vector<LoadPoint> *NewVect; // Указатель на вектор
	void* NoVarToOutProg = nullptr; // Подрограммы реакции на ошибку "Нет переменной для вывода"
	void* WrongFormatProg = nullptr; // Подрограммы реакции на ошибку "Неправильный формат"
	void* WrongVectFormatProg = nullptr; // Подрограммы реакции на ошибку "Неправильный формат"
	LoadPoint Var = {0,nullptr};
	void* WrongInterFormat = nullptr; // Подпрограмма обработки ошибки 'Неправильный формат введенных данных'
	vector<MkRef> VarOutBuf; // Вектора Мк и указателей для вывода результата
	set<string> True = { "true", "True" }, False = {"false", "False"}; // Множество значений true false
	string inStr; // Последняя введенная строка
	void* InputFormatErrProg = nullptr; // Программа ошибки формата входных данных
	int VectCol = 0; // Количество колонок при выводе вектора

	string Template = ""; // Строка шаблона для вывода
	vector<FormatToken> Tokens; // Вектор токенов шаблона
	vector<LoadPoint> Operands; // Вектор указателей на операнды
	int NOp = 0; // Идекс текущего операнда
	int OpCount = 0; // Счетчик пришедших операндов
	string Buffer; // Буффер для вывода
	void ParseTemplate(); // Функция парсинга строки
	string FormatValue(LoadPoint& lp, FormatSpec& spec); // Функция форматирования операнда
	void ExecuteTemplate(); // Функция сборки строки
	void ReportError(const string& where, const string& msg);
	std::ostream& out(); // per-FU output sink: this FU's redirect file if open, else cout
public:
	bool Quiet = false; // --run: suppress this (compiler-trace) Console's output
	void ProgFU(long int MK, LoadPoint Load, FU* Sender = nullptr) override;
	FU* Copy() override; // Программа копирования ФУ
	FU* TypeCopy() override; // Создать ФУ такого же типа (не копируя контекст
	Console(FU* BusContext, FU* Templ) : FU(BusContext) { Bus = BusContext; FUtype = 1; };
	Console() : FU() { Console(nullptr, nullptr); };
private:
};
