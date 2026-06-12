#pragma once
#include "Consts.h"
#include <set>
#include <map>
#include <vector>

struct MkFu
{
	int Mk;
	FU* Fu;
};

struct RegProg // ������� ����������� ��������� � ��������� �� ����������� ���� ���������� �������
{
	string Reg;
	void* Prog = nullptr;
};

class Lex : public FU {
private:
	void LexOut(bool Copy=false, int MK = -1);
	vector<RegProg> RegVect; // ������ ���������� ��������� ��� ����������� ������
	string LexAccum; // ����� ��� �������� ���������� �������
	set<long int> FinMk; // ��������� �������� �� ��� �����������, ����������� ��� ���������� ���������
	string ABC_templ = "_QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm"; // ������� ��������
	string Digit_templ = "0123456789"; // --- ������� �����
	string DigitOCT_templ = "01234567"; // --- ������� �����
	string DigitHEX_templ= "0123456789ABCDEF"; // --- ������� �����
	string Digit_seps_templ = "."; // --- ������� ������������ �����
	set<string> Seps_templ = {"<=>", "//=", "!!=", "&&=", "||=", "&&=", "^^=", ">>|", "|<<", \
	">>", "<<", "<-", "->",  "==", ":=" "^=", "*=", "%=", "/=", "//", "++", "--", "-=","+=","<>", ">=", "<=", "!!", "!=", "||", "|=", "&&", "&=", "^^", "^=",\
	"<", ">", "~", "`", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "+", "=", "-", "{", "}", "[", "]", "?", ";", ":", ",", ".", "\\", "/", "|", "\""};// ������� ������������
	set<char> ABC, Digit, DigitHEX, DigitOCT, DigitBIN, DigitSeps; // --- DigitSeps - ���������, �������� ����������� ����� � ������� ����� �����
	set<string> Seps;
	IC_type StartProg = nullptr, StopProg = nullptr, FinProg = nullptr; // ���������, ����������� ��� ������� �������� ������������ �������
	string EOL = "\n"; // ������� ��������� ������
	set<string> TrueConst = { "true","True" }; // ��������� true
	set<string> FalseConst = { "false","False" }; // ��������� false
	long int ProgLevel = 0; // ������� ��������� �����������
	string ProgBracket = "{", ProgFinBracket = "}"; // ������� ����������� �������
	void* TabErrProg = nullptr; // ��������� ��������� ������ ���������
	void* ErrProg = nullptr; // ���������, ����������� ��� ������
	bool TabMode = false; // ����� ����������� ������ �� ������ ���������
public:
	// When true, the lexer silently skips the 4-char `\**\` prefix used by
	// CompileCC.oap-style block declarations (`\**\Root.Set=` -> `Root.Set=`).
	// Toggle compile-time only; flip to false to remove the behavior.
	bool StarStarSlashStripMode = true;
	// When true, a `!` that immediately follows a Mnemo (no whitespace) is
	// dropped -- CompileCC.oap uses `MkTable.Set=ConsMkTable!` as a
	// load-by-reference hint that our compiler accepts as a no-op syntactic
	// suffix. Toggle compile-time only.
	bool MnemoRefBangStripMode = false; // !-after-mnemo now emits a real Sep token consumed by the OAP '!' handler
	void ProgFU(long int MK, LoadPoint Load, FU* Sender=nullptr);
	FU* Copy() override; // ��������� ����������� ��
	FU* TypeCopy() override; // ������� �� ������ �� ���� (�� ������� ��������
	Lex(FU *BusContext=nullptr, FU *Templ=nullptr);
	Lex() { Lex(nullptr, nullptr); };
	vector<FU*> Receiver = { nullptr }; // ���� ���������� �� ��-���������
	vector<long int> ReceiverMK = { 0 }; // ���� �� ��� ����������
	long int S = 0; // ����� ��������� ������������� ��������
	bool Work = true; // ���� �������� ������ �������
	bool lexFileActive = false; // re-entrancy guard for LexFile (mk101): skip if a LexFile lex is already running
//	int MnemoAtr = -2, SeperatAtr = -4, IntAtr = -10, DoubleAtr = -3, BoolAtr = -5, StrAtr = -6, ConstAtr = -13;
	long int MnemoAtr = -2, SeperatAtr = -4, IntAtr = -10, DoubleAtr = -13, BoolAtr = -13, StrAtr = -13, ConstAtr = -13;
	long int ib = 0, SizeBuf = 5; // ������� ������� �������� �� �������� ������� � ������ ������
	ip *LexBuf; // ����� �������� ������
	map<long int,MkFu> UnicAtr; // ������ ������������� ���������, �� ������� ���� ��������� ������� ��
	long int LastUnicAtr = 0; // ��������� ���������� ������� (����� ����� ���� ����� �������� � ���� �� � ����� ��)
	// ����� �������� ������ {"dsfsdf",-3,"dfsdfs",""}
//	void Debug(char i, int S, string FigureBuf); // --- ��� �������, ����� �������
};