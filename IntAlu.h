#pragma once
#include "Consts.h"

class IntContext {
public:
	long int* AccumulatUk; // ��������� �� �����������
	long int Accumulat; // �����������
	long int AutoInc; // ��� ������������� �������������
	long int Fin; // �������� �������� �����
};

class IntAlu : public FU {
private:
//	void ProgExec(void* Uk, unsigned int CycleMode = 0, FU* Bus = nullptr, vector<ip>::iterator* Start = nullptr) override; // ���������� ��������� �� ��
//	void ProgExec(LoadPoint Uk, unsigned int CycleMode = 0, FU* Bus = nullptr, vector<ip>::iterator* Start = nullptr) override; // ���������� ��������� �� ��

	vector<int> ExecCounter = { 1 }; // ���������� �������� ��� Exec

	FU* Receiver = nullptr; // �������� ������
	long int * AccumulatUk= &Accumulat; // ��������� �� �����������
	vector <IntContext> Stack; // ���� �������������
	long int Compare = 0; // ������������� ��������
	long int RandFrom = 0, RandTo = 0; // �������� ������������ ��������� ��������
	long int* CompareUk = &Compare; // ��������� �� ������������ ��������
	void* EmptyStackErrProg = nullptr; // ��������� ��������� ������ pop �� ������� �����
	void* BiggerProg=nullptr, * LessProg = nullptr, * EQProg = nullptr, * NEQProg = nullptr, * BiggerEQProg = nullptr, * LessEQProg = nullptr;
	void* NoCorrectTypeErrProg = nullptr; // ��������� ��� ������ ������ ������������� (�������� ����)
	void* NoIntTypeErrProg = nullptr; // ��������� ��� ������ ������ �������� ����
	void* NoBoolIntTypeErrProg = nullptr; // ��������� ��� ������ ������ �������� ���� ��� boolean 
public:
	long int Accumulat = 0; // �����������
	long int AutoInc = 0; // ��� ������������� �������������
	long int Fin = 0; // �������� �������� �����
	void ProgFU(long int MK, LoadPoint Load, FU* Sender=nullptr);
	FU* Copy() override; // ��������� ����������� ��
	FU* TypeCopy() override; // ������� �� ������ �� ���� (�� ������� ��������
	IntAlu(FU* BusContext, FU* Templ=nullptr) : FU(BusContext) { ProgFU(0, { 0, nullptr }); Bus = BusContext; FUtype = 7; };
	IntAlu(){ IntAlu(nullptr, nullptr); };
};