	#include "stdafx.h"
	#include "Lex.h"
	#include <string.h>

	using namespace std;

	void Lex::LexOut(bool Copy,int MK) // ������ ������� �����������
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
		// �������� ����� �� � ���������
		MK %= FUMkRange;
		switch (MK)
 		{
		case 0: // Reset
			S = 0; // ����� ��������� ������������� ��������
				   //		LexAccum = "";
			UnicAtr.clear();
			ReceiverMK.back() = 0;
			Receiver.back() = Bus;
			ErrProg = nullptr;
			StopProg = nullptr;
			TabErrProg = nullptr;
			LexAccum="";
			break;
		case 2: // ReceiverPush �������� ������ ����� � �� �������� ������� � ���� � ���������� �� ��� ���������
			Receiver.push_back(Receiver.back());
			if(Load.Point==nullptr)
				ReceiverMK.push_back(ReceiverMK.back());
			else
				ReceiverMK.push_back(Load.toInt());
			break;
		case 3: // ReceiverPop ���������� �� �������� �� ����� � �������� ��
			{
			long int t = ReceiverMK.back();
			if (!ReceiverMK.size()) break;
			ReceiverMK.pop_back();
			Receiver.pop_back();
			MkExec(Load, {Cint,&t});
			break;
			}
		case 4: // ReceiverPopMk ���������� ����� �������� �� ����� � ������ �� � ���
		{
			Load.Write(ReceiverMK.back());
			if (!ReceiverMK.size()) break;
			ReceiverMK.pop_back();
			Receiver.pop_back();
		}
			break;
		case 1: // ReceiverPopCend ���������� �� �������� �� ����� � ������ ������� (��� ������� �������� �������� ������� �������)
			if (!ReceiverMK.size()) break;
			ReceiverMK.pop_back();
			Receiver.pop_back();
			if (Load.Point == nullptr)
				LexOut();
				//Receiver.back()->ProgFU(MK, { TIP, &LexBuf[ib] });
			else
    			Receiver.back()->ProgFU(ReceiverMK.back(), Load, this);
			break;
		case 5: //ReceiverMKSet ���������� �� ��� ��������� ������ 
			if (Load.Type >> 1 == Dint) ReceiverMK.back() = Load.toInt(); break;
		case 6: // FinMkAdd �������� ��������� ��
			FinMk.insert(Load.toInt());
			break;
		case 7: // FinMkClear �������� ������ ��������� ��
			FinMk.clear();
			break;
		case 8: // FinMkProgExec ��������� ���������, ���� ������ � ��������� ���������
			if (FinMk.count(ReceiverMK.back()))
				ProgExec(Load);
			break; 
		case 9: // NoFinMkProgExec ��������� ���������, ���� ������ �� � ��������� ���������
			if (!FinMk.count(ReceiverMK.back()))
				ProgExec(Load);
			break;
		case 10: //ErrProgSet
			if (Load.isProg())
				ErrProg = Load.Point;
			break;
		case 11: // TabErrorProgSet ������ ���������
			if(Load.isProg())
				TabErrProg = Load.Point;
			break;
		case 12: // StartProgSet ���������� ���������, ����������� ����� ������� ���������� 
			StartProg = (IC_type)Load.Point;
			break;
		case 13: // StopProgSet ���������� ���������, ����������� ��� ��������� ���������� ������������ ������� �� �� Stop 
			StopProg = (IC_type)Load.Point;
			break;
		case 14: // FinProgSet  ���������� ���������, ����������� ����� ��������� ������� ������
			FinProg = (IC_type)Load.Point;
			break;
		case 15:// UnicAtrSet ���������� ���������� �������
			LastUnicAtr = Load.toInt();
			UnicAtr[LastUnicAtr] = {0,Bus};
			break;
		case 16:// UnicMkSet ���������� �� ��� ����������� ��������
			if (UnicAtr.size())
				UnicAtr[LastUnicAtr].Mk = Load.toInt();
			break;
		case 17:// UnicFuSet ���������� �������� ��� ����������� ��������
			if (UnicAtr.size())
				UnicAtr[LastUnicAtr].Fu = (FU*)Load.Point;
			break;
		case 18:// UnicReset �������� ������ ���������� ���������
			UnicAtr.clear();
			break;
		case 20: // LexBufSizeSet ���������� ������ ������ ������
			if (Load.Type >> 1 != Dint) break;
			for (int i = 0; i < SizeBuf; LexBuf[i++].Load.Clear());
			delete[] LexBuf;
			SizeBuf = Load.toInt(2);
			LexBuf = new ip[SizeBuf];
			for (int i = 0; i < SizeBuf; LexBuf[i++].Load = { 0,nullptr });
			LexBuf[0].atr=SeperatAtr;// ���������� ������� ����������� ��� ��� ���������� �������������
			ib = 0;
			ProgLevel = 0; // ������� ������� ����������� ������
			break;
		case 21: // SepSet ���������� ���������
			Seps.insert(Load.toStr());
			break;
		case 22: // SepErase ������� ����������� �� ������ ������������
			Seps.erase(Load.toStr());
			break;
		case 23: // SepClear �������� ������ ������������
			Seps.clear();
			break;
		case 24: // FalseAdd �������� ��������� ���
			FalseConst.insert(Load.toStr());
			break;
		case 25: // TrueAdd �������� ��������� ������
			TrueConst.insert(Load.toStr());
			break;
		case 19: // TrueFalseClear �������� ������ �������� ������ � ���
			FalseConst.clear();
			TrueConst.clear();
			break;
		case 26: // NoUnucToReseiver ������ �������, �� �������� ���������� ��������� (��� ������� �������� �������� ������� �������)
			if (Load.Point == nullptr)
				MkExec(ReceiverMK.back(), LexBuf[ib].Load, Receiver.back());
			else
			{
				ib = (ib + 1) % SizeBuf;
				LexBuf[ib].Load.Clear(); // ������� �������� ��
				LexBuf[ib].copy(Load);
				MkExec(Load, LexBuf[ib].Load, Receiver.back());
			}
			break;
		case 27: //Replace �������� ������� ������� � ������
		case 28: //ReplaceToReceiver ������ ������� �� �������� �� ����������, ������� ������� ������� � ������
		case 29: //ReplaceCopyToReceiver ������ ����� ������� �� �������� �� ����������, ������� ������� ������� � ������
		case 30: //ToReceiver ������ ������� �� �������� �� ���������� (���� �������� nil, �� �������� ������� �������)
		case 31: //CopyToReceiver ������ ����� ������� �� �������� �� ����������
			if(MK==30 && Load.Point==nullptr)	{
				LexOut(false);
				break;
			}
			if(MK==30 || MK==31) ib = (ib + 1) % SizeBuf;
			LexBuf[ib].Load.Clear(); // ������� �������� ��
			LexBuf[ib].copy(Load);
			if(MK!=27)LexOut(MK==31);
			break;
		case 35: //OutMk ������ MK c ��������� �������� (���� nil � ��������, �� �������� �� Receiver)
			if (Load.Point == nullptr)
			{
				//LexOut(false);
				Receiver.back()->ProgFU(ReceiverMK.back(), { TIP, &LexBuf[ib] }, this);
			}
			else
				if(Load.Type>>1==Dint)
					Receiver.back()->ProgFU(Load.toInt(), { TIP, &LexBuf[ib] }, this);
			break;
		case 36: // CopyOutMk ������ �� � ������ ��������� ������� (���� nil � ��������, �� �������� �� Receiver)
			if (Load.Point!=nullptr) 
				Receiver.back()->ProgFU(*(int*)Load.Point, { TIP, LexBuf[ib].Clone() }, this);
			else
				Receiver.back()->ProgFU(ReceiverMK.back(), { TIP, LexBuf[ib].Clone() }, this);
			break;
		case 37: // LoadOut ������ �������� ������� �������
			Load.Write(LexBuf[ib].Load);
			break;
		case 38: // LoadOutMk ������ �� � ��������� ������� �������
			MkExec(Load, LexBuf[ib].Load);
			break;
		case 39: // LoadCopyOut ������ �������� ������� �������
			Load.Write(LexBuf[ib].Load.Clone());
			break;
		case 40: // LoadCopyOutMk ������ �� � ������ �������� ������� �������
			MkExec(Load, LexBuf[ib].Load.Clone());
			break;
		case 41: // AtrOut ������ ������� ������� �������
			Load.Write(LexBuf[ib].atr);
			break;
		case 42: // AtrOutMk ������ �� � ��������� ������� �������
			MkExec(Load, { Cint, &LexBuf[ib].atr });
			break;
		case 45: //PrevOutMk ������ �� � ���������� �������� (���� �������� nil, �� �������� �� Receiver)
		case 46: //PrevPrevOutMk ������ �� � �������������� �������� (���� �������� nil, �� �������� �� Receiver)
			if (Load.Point==nullptr)
				Receiver.back()->ProgFU(ReceiverMK.back(), { TIP, &LexBuf[(ib - MK + 44) % SizeBuf] }, this);
			else
				Receiver.back()->ProgFU(Load.toInt(), {TIP, &LexBuf[(ib - MK + 44) % SizeBuf]}, this);
			break;
		case 47: //PrevCopyOutMk ������ �� � ������ ���������� ������� (���� �������� nil, �� �������� �� Receiver)
		case 48: //PrevPrevCopyOutMk ������ �� � ������ �������������� ������� (���� �������� nil, �� �������� �� Receiver)
			if (Load.Point == nullptr)
				Receiver.back()->ProgFU(ReceiverMK.back(), { TIP, LexBuf[(ib - MK + 46) % SizeBuf].Clone() }, this);
			else	
				Receiver.back()->ProgFU(Load.toInt(), {TIP, LexBuf[(ib - MK + 46) % SizeBuf].Clone()}, this);
			break;
		case 50: // AtrSet ���������� ������� ��������� �������
			LexBuf[ib].atr=Load.toInt();
			break;
		case 51: // PrevAtrSet ���������� ������� ���������� �������
			LexBuf[(ib - 1 + SizeBuf) % SizeBuf].atr = Load.toInt();
			break;
		case 55: // LoadSet ���������� ��������� � ��������� �������
			LexBuf[ib].Load = Load;
			break;
		case 56: // PrevLoadSet ���������� ��������� � ���������� �������
			LexBuf[(ib - 1 + SizeBuf) % SizeBuf].Load = Load;
			break;
		case 57: // LoadCopySet ���������� ����� ��������� � ��������� �������
			LexBuf[ib].Load.Clone(Load);
			break;
		case 58: // PrevLoadCopySet ���������� ����� ��������� � ���������� �������
			LexBuf[(ib - 1 + SizeBuf) % SizeBuf].Load.Clone(Load);
			break;
		case 65: // VarSet ���������� ��� ���������� ��� �������� ��������� �������
			LexBuf[ib].Load.Type |= 1;
			LexBuf[ib].Load.Type --;
			break;
		case 66: // PrevVarSet ���������� ��� ���������� ��� �������� ���������� �������
			LexBuf[(ib - 1 + SizeBuf) % SizeBuf].Load.Type |= 1;
			LexBuf[(ib - 1 + SizeBuf) % SizeBuf].Load.Type--;
			break;

		case 70: // SendToReceiver ��������� ������� �� �������� ���������� (���� � �������� nil, �� ���������� ������� �������)
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
				ib = (ib + 1) % SizeBuf; //���������� �������� ������ ������ �������� ������ �� 1
				LexBuf[ib].Load.Clear(); //�������� �������� ��
				if(Load.isIP())
					LexBuf[ib] = ((ip*)Load.Point)->Copy(); //���������� ������� � ����� �������� ������ � ���� �� {�������, ���, ���������}
				else
					LexBuf[ib] = ((IC_type)Load.Point)->begin()->Copy(); //���������� ������� � ����� �������� ������ � ���� �� {�������, ���, ���������}

				LexOut();
			}
*/

			break;
		case 71: // CendCopyToReceiver ��������� ����� ������� �� �������� ���������� (���� � �������� nil, �� ���������� ������� �������)
			if (Load.Point == nullptr)
				LexOut(true);
			else
				Receiver.back()->ProgFU(ReceiverMK.back(), Load.Clone(), this);
//			MkExec(ReceiverMK.back(), Load.Copy());
			break;

		case 85: // RegSet ���������� ���������� ���������
			RegVect.push_back({ Load.toStr(), nullptr});
			break;
		case 86: // RegProgSet ���������� ��������� ��������� �������, ���������� ���������� ����������
			if(RegVect.size())
				RegVect.back().Prog=Load.Point;
			break;
		case 90: // IfStopExec ���������, ���� ������ ����������
			if (!Work)
				ProgExec(Load);
			break;
		case 91: // IfRunExec ���������, ���� ������ � ������� ���������
			if (Work)
				ProgExec(Load);
			break;

		case 94: // LexStrOut ������ ������ � ��������� ������������ ��������
			Load.Write(LexAccum);
			break;
		case 95: // LexStrOutMk ������ �� �� ������� � ��������� ������������ ��������
			MkExec(Load, { Cstring, &LexAccum });
			break;
		case 96: //TabModeSet ���������� ����� ��������� ������ �� �������� ��������� (�� ��������� ture)
			TabMode = Load.toBool(true);
			break;
		case 98: // LexReset �������� ��������� ������������ �������
			S = 0;
			LexBuf[0].atr = SeperatAtr;
			ProgLevel = 0; // ������� ���������			LexAccum="";
			break;
		case 99: // Stop ���������� ����������� ������ (��� �� ���������� ��������� ��� ����������� ����������� �������)
			Work = false;
			ProgExec(StopProg); // ��������� ��������� �� �������� ������������ �������
			S = 0;
			LexBuf[0].atr = SeperatAtr;
			ProgLevel = 0; // ������� ���������
			break;
		case 100: // Lexing
		{	string str = Load.toStr()+" ";
			str += EOL; // �������� ������� ����� ������
			S = 0; // --- ��������� ���������� ��������� ��������
			if (Load.Type >> 1 == Dstring && Load.toStr() == "")
			{
				ib = (ib + 1) % SizeBuf;
				LexBuf[ib].Load.Clear(); // ������� �������� ��
				LexBuf[ib] = { StrAtr, Cstring, new string("") };
				LexOut();
			}
			Work = true;
			if (TabMode) {
				long int tabCounter = 0;
				while (str[tabCounter] == '\t')
					tabCounter++;
				if (tabCounter > ProgLevel + 1) // ��������� ������ ��� �� ���� �������
				{
					ProgExec(ErrProg); // ����� ������ ������������ �������
					ProgExec(TabErrProg); // ������ ���������
					break;
				}
				if (tabCounter > ProgLevel && ProgBracket!="")
				{
					ib = (ib + 1) % SizeBuf;
					LexBuf[ib].Load.Clear(); // ������� �������� ��
					//string t = ProgBracket;
					LexBuf[ib] = { SeperatAtr,Cstring , new string(ProgBracket) };
					LexOut();
				}
				if(ProgFinBracket!="")
					for (int i = ProgLevel - tabCounter; i > 0; i--) // ������ ������������ ���������� ������
					{
						ib = (ib + 1) % SizeBuf;
						LexBuf[ib].Load.Clear(); // ������� �������� ��
						//string *t = new string(ProgFinBracket);
						LexBuf[ib] = { SeperatAtr,Cstring , new string(ProgFinBracket) };
						LexOut();
					}
				ProgLevel = tabCounter; // ��������� ������� ����������� �������
				if (tabCounter) // ������ ������� ���������
					str = str.substr(tabCounter, string::npos);
			}
			// State-machine entry. After the loop we explicitly flush any
			// in-progress state-12 (newline) accumulation AND emit a final
			// empty separator so the parser can dispatch its `else` /
			// Sep="" transition at end-of-input -- without this an input
			// like `Cons.OutLn=A+B` parks in ALEAfter and the final
			// AleEmit never fires.
			for (auto i = str.begin(); i != str.end() && Work; i++)
				switch (S) //LEXER
				{
				// ��������� ���������
				case 0:
				{
					// ����� �� ���������
					// ���� �������, �� ��������� ���������� ������� � LexAccum � ��������� �� ��������� ��� ��������� �������
				/*
					if (������� ���������)
					{
						LexAccum=���������� �������
						ProgExec(RegVect[k].Prog); k - ��� ������ ��������� ��������� � RegVect
						���������� ���������� ������� (i+=����� �������)
					}
					*/
					if ((*i == ' ' || *i == '\n' || *i == '\t' || *i == '\r')) //�����������; 0 -> 0
					{
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (*i == ProgBracket[0]) // ������������ ����������� ������
					{
						auto j = i+1;
						for (; *j == ' ' || *j == '\t'; j++);
						if (j == str.end() || *j == '\n') // �������� ����������� ������ � ������
							ProgLevel++;
					}
					if (*i == '"') //�������; 0 -> 4
					{
						LexAccum = ""; //�������� �������
						S = 4; //������� � 4 ���������
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if ((*i == '+' || *i == '-') && (i==str.begin() ||
						(LexBuf[(ib + SizeBuf) % SizeBuf].atr == SeperatAtr) && *(i - 1) != ')') && *(i - 1) != '}') // ������ (+,-); 0 -> 10
					{
						LexAccum = *i; //������ � �������� ����������
						S = 10; //������� � ��������� 10
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (*i == '/') // ������ (/); 0 -> 5
					{
						LexAccum = *i; //������ � �������� ����������
						S = 5; //������� � ��������� 5
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (*i == '\n') //������� �� ����� ������; 0 -> 12
					{
						//LexAccum = *i; //������ � �������� ����������
						// TO DO tabCounter = 0 (���������� ��������������� ��������)
						
						S = 12; //������� � ��������� 12
						//Debug(*i, S, LexAccum); // --- �������
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

					if ((Digit.count(*i)) && (*i != '0')) // --- ����� (1..9); 0 -> 1
					{
						LexAccum = *i; //������ � �������� ����������
						S = 1; //������� � ��������� 1
						//Debug(*i, S, LexAccum); // --- ����/���
						//TO DO numberFormat = DEC (���������� ��������������� ��������)
						break;
					}
					if (ABC.count(*i)) //�����; 0 -> 3
					{
						LexAccum = *i; //������ � �������� ����������
						S = 3; //������� � ��������� 3
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (*i == '0') // ����� (0); 0 -> 9
					{
						LexAccum = *i; //������ � �������� ����������
						S = 9; //������� � ��������� 9
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (DigitSeps.count(*i))  // --- ������ ���������� ������� � ����� ����� (. ,); 0 -> 2
					{
						LexAccum = '0'; // --- ���������� ������� 0 � �������� ����������, ��� ��� .5 = 0.5; ,5 = 0,5
						LexAccum += *i; //���������� ������� � �������� ����������
						//TO DO numberFormat = DEC (���������� ��������������� ��������)
						S = 2; //������� � ��������� 2
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					Work = false; //��������� ����� �������� ������ ������� �� false
					if (ErrProg != nullptr) ProgExec(ErrProg, 0, Bus, nullptr); //��������� ������
					break;
				}
				// ��������� ����� �����
				case 1: 
					if (Digit.count(*i)) // --- ����� (0..9); 1 -> 1
					{
						LexAccum += *i; //���������� ������� � �������� ����������
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (*i == '_') //������ ���������� ����� (_); 1 -> 1
					{
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (DigitSeps.count(*i))  // --- ������ ���������� ������� � ����� ����� (. ,); 1 -> 2
					{
						LexAccum += *i; //���������� ������� � �������� ����������
						S = 2; //������� � ��������� 2
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (Seps.count(str.substr(distance(str.begin(), i), 1)) || (*i == ' ' || *i == '\n' || *i == '\t' || *i == '\r')) //�����������; 1 -> 0
					{
						i--; // ��� ��������� ����������
						int  *tint = new int;
						*tint = atoi(LexAccum.c_str()); //������ ������� � ����������
						ib = (ib + 1) % SizeBuf; //���������� �������� ������ ������ �������� ������ �� 1
						LexBuf[ib].Load.Clear(); //�������� �������� ��
						LexBuf[ib] = { IntAtr,Cint , tint }; //���������� ������� � ����� �������� ������ � ���� �� {�������, ���, ���������}
						LexOut();
						S = 0; //������� � ��������� 0
						break;
					}
					Work = false; //��������� ����� �������� ������ ������� �� false
					ProgExec(ErrProg, 0, Bus, nullptr); //���������� ������
					break;
				// ��������� ������� �����
				case 2:
					if (Digit.count(*i))  // --- ����� (0..9); 2 -> 2
					{
						LexAccum += *i; //���������� ������� � �������� ����������
					//	Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (*i == '_') //������ ���������� ����� (_); 2 -> 2
					{
					//	Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (Seps.count(str.substr(distance(str.begin(), i), 1)) || (*i == ' ' || *i == '\n' || *i == '\t' || *i == '\r')) //�����������; 2 -> 0
					{
						i--; // ��� ��������� ����������
						double *ft = new double;
						*ft = atof(LexAccum.c_str()); //������ ������� � ����������
						ib = (ib + 1) % SizeBuf; //���������� �������� ������ ������ �������� ������ �� 1
						LexBuf[ib].Load.Clear(); //�������� �������� ��
						LexBuf[ib] = { DoubleAtr, Tdouble, ft }; //���������� ������� � ����� �������� ������ � ���� �� {�������, ���, ���������}
						LexOut();
						S = 0; //������� � ��������� 0
						break;
					}
					Work = false;  //��������� ����� �������� ������ ������� �� false
					ProgExec(ErrProg, 0, Bus, nullptr); //���������� ������
					break;
				//��������� ���������
				case 3:
					if (Seps.count(str.substr(distance(str.begin(), i), 1)) || (*i == ' ' || *i == '\n' || *i == '\t' || *i == '\r')) //�����������; 3 -> 0
					{
						i--; // ��� ��������� ����������
						if (find(TrueConst.begin(), TrueConst.end(), LexAccum) != TrueConst.end())
						{
							bool* t = new bool(true);
							ib = (ib + 1) % SizeBuf; //���������� �������� ������ ������ �������� ������ �� 1
							LexBuf[ib].Load.Clear(); //�������� �������� ��
							LexBuf[ib] = { BoolAtr, Cbool, t };  //���������� ������� � ����� �������� ������ � ���� �� {�������, ���, ���������}
							LexOut();
							S = 0;
							break;
						}
						if (find(FalseConst.begin(), FalseConst.end(), LexAccum) != FalseConst.end())
						{
							bool* t = new bool(false);
							ib = (ib + 1) % SizeBuf; //���������� �������� ������ ������ �������� ������ �� 1
							LexBuf[ib].Load.Clear(); //�������� �������� ��
							LexBuf[ib] = { BoolAtr, Cbool, t };  //���������� ������� � ����� �������� ������ � ���� �� {�������, ���, ���������}
							LexOut();
							S = 0;
							break;
						}
						string *st = new string;
						*st = LexAccum; //������ ������� � ����������
						ib = (ib + 1) % SizeBuf; //���������� �������� ������ ������ �������� ������ �� 1
						LexBuf[ib].Load.Clear(); //�������� �������� ��
						LexBuf[ib] = { MnemoAtr, Cstring, st };  //���������� ������� � ����� �������� ������ � ���� �� {�������, ���, ���������}
						LexOut();
						S = 0; //������� � ��������� 0
						break;
					}
					if (ABC.count(*i) || Digit.count(*i)) // --- ����� ���� �����; 3 -> 3
					{
						LexAccum += *i; //���������� ������� � �������� ����������
					//	Debug(*i, S, LexAccum); // --- �������
						break;
					}
					Work = false;  //��������� ����� �������� ������ ������� �� false
					ProgExec(ErrProg, 0, Bus, nullptr); //���������� ������
					break;
				//��������� �������
				case 4:
					if (*i != '"' && *i != '\\') //����� ������ ����� ������� � \; 4 -> 4
					{
						LexAccum += *i;  //���������� ������� � �������� ����������
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}		
					else 
						if (*i == '\\')//������ (\); 4 -> 8
						{
							S = 8; //������� � ��������� 8
						//Debug(*i, S, LexAccum); // --- �������
							break;
						}
						else 
							if (*i == '"')//�������; 4 -> 0
							{
								string *st2 = new string;
								*st2 = LexAccum; //������ ������� � ����������
								ib = (ib + 1) % SizeBuf; //���������� �������� ������ ������ �������� ������ �� 1
								LexBuf[ib].Load.Clear(); //�������� �������� ��
								LexBuf[ib] = { StrAtr, Cstring, st2 }; //���������� ������� � ����� �������� ������ � ���� �� {�������, ���, ���������}
								LexOut();
								S = 0; //������� � ��������� 0
							//	Debug(*i, S, LexAccum); // --- �������
								break;
							}
					Work = false;  //��������� ����� �������� ������ ������� �� false
					ProgExec(ErrProg, 0, Bus, nullptr); //���������� ������
					break;
				//��������� ��������� �����������
				case 5:
					if (*i == '/') //������ (/); 5 -> 11
					{
						LexAccum += *i; //���������� ������� � �������� ����������
						S = 11; //������� � ��������� 11
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (*i == '*') //������ (*); 5 -> 6
					{
						LexAccum += *i; //���������� ������� � �������� ����������
						S = 6; //������� � ��������� 6
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					{//
						string* t=new string(LexAccum);
						ib = (ib + 1) % SizeBuf;
						LexBuf[ib].Load.Clear(); // ������� �������� ��
						LexBuf[ib] = { SeperatAtr,Cstring , new string(*t) };
						LexOut();
						i--;
						S = 0;

						//					Work = false;  //��������� ����� �������� ������ ������� �� false
						//					ProgExec(ErrProg, 0, Bus, nullptr); //���������� ������
						break;
					}
				//��������� �������������� �����������
				case 6:
					if (*i != '/') //����� ������ ����� (/); 6 -> 6
					{
						LexAccum += *i;  //���������� ������� � �������� ����������
					//	Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (*i == '/') //������ (/); 6 -> 7
					{
						LexAccum += *i; //���������� ������� � �������� ����������
						S = 7; //������� � ��������� 7
					//	Debug(*i, S, LexAccum); // --- �������
						break;
					}
					Work = false;  //��������� ����� �������� ������ ������� �� false
					ProgExec(ErrProg, 0, Bus, nullptr); //���������� ������
					break;
				//��������� �������������� �����������
				case 7:
					if (*i != '*') //����� ������ ����� (*); 7 -> 6
					{
						LexAccum += *i;  //���������� ������� � �������� ����������
						S = 6; //������� � ��������� 6
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					else //������ (*); 7 -> 0
					{
						string* multiLineComment = new string;
						*multiLineComment = LexAccum; //������ ������� � ����������
						ib = (ib + 1) % SizeBuf; //���������� �������� ������ ������ �������� ������ �� 1
						LexBuf[ib].Load.Clear(); //�������� �������� ��
						LexBuf[ib] = { StrAtr, Cstring, multiLineComment }; //���������� ������� � ����� �������� ������ � ���� �� {�������, ���, ���������}
						LexOut();
						S = 0; //������� � ��������� 0
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					Work = false;  //��������� ����� �������� ������ ������� �� false
					ProgExec(ErrProg, 0, Bus, nullptr); //���������� ������
					break;
				//��������� �������������
				case 8:
					if (*i == '"' || *i == 'n' || *i == '\'') //������� (", n, '); 8 -> 4
					{
						LexAccum += *i; //���������� ������� � �������� ����������
						//TO DO ��������� ������������� (���������� ��������������� ��������)
						S = 4; //������� � ��������� 4
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					else //����� ������ ����� (n, ", '); 8 -> 4
						{
							LexAccum += *i; //���������� ������� � �������� ����������
							S = 4; //������� � ��������� 4
							//Debug(*i, S, LexAccum); // --- �������
							break;
						}
					Work = false;  //��������� ����� �������� ������ ������� �� false
					ProgExec(ErrProg, 0, Bus, nullptr); //���������� ������
					break;
				//������������ ������� ���������
									//������������ ������� ���������
				/*
				case 9:
					if (*i == '0') //����� (0); 9 -> 1
					{
						S = 1; //������� � ��������� 1
						LexAccum += *i; //���������� ������� � �������� ����������
						//TO DO numberFormat = DEC (���������� ��������������� ��������)
						break;
					}
					if (*i == 'x') //������ (x); 9 -> 1
					{
						S = 1; //������� � ��������� 1
						LexAccum += *i; //���������� ������� � �������� ����������
						//TO DO numberFormat = HEX (���������� ��������������� ��������)
						break;
					}
					if (*i == 'b') //������ (b); 9 -> 1
					{
						S = 1; //������� � ��������� 1
						LexAccum += *i; //���������� ������� � �������� ����������
						//TO DO numberFormat = BIN (���������� ��������������� ��������)
						break;
					}
					Work = false;  //��������� ����� �������� ������ ������� �� false
					ProgExec(ErrProg); //���������� ������
					break;

					*/
				case 9:
					if (*i == '_') //������ ���������� ����� (_); 9 -> 9
					{
			//			Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (Digit.count(*i)) // --- ����� (0..9); 9 -> 1
					{
						LexAccum += *i; //���������� ������� � �������� ����������
						//TO DO numberFormat = DEC (���������� ��������������� ��������)
						S = 1; //������� � ��������� 1
				//		Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (*i == 'b') //������ (b); 9 -> 13
					{
						//LexAccum += *i; //���������� ������� � �������� ���������� �� �����, ��� ��� b �������� ������������ BIN �����
						//TO DO numberFormat = BIN (���������� ��������������� ��������)
						S = 13; //������� � ��������� 13
				//		Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (*i == 'o') //������ (o); 9 -> 15
					{
						//LexAccum += *i; //���������� ������� � �������� ���������� �� �����, ��� ��� o �������� ������������ OCT �����
						//TO DO numberFormat = OCT (���������� ��������������� ��������)
						S = 15; //������� � ��������� 15
				//		Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (*i == 'x') //������ (x); 9 -> 17
					{
						//LexAccum += *i; //���������� ������� � �������� ���������� �� �����, ��� ��� x �������� ������������ HEX �����
						//TO DO numberFormat = HEX (���������� ��������������� ��������)
						S = 17; //������� � ��������� 17
				//		Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (DigitSeps.count(*i))  // --- ������ ���������� ������� � ����� ����� (. ,); 9 -> 2
					{
						LexAccum += "."; //���������� ������� � �������� ����������
						//TO DO numberFormat = DEC (���������� ��������������� ��������)
						S = 2; //������� � ��������� 2
				//		Debug(*i, S, LexAccum); // --- �������
						break;
					}
					i--;
					S = 1;
					break;
					Work = false;  //��������� ����� �������� ������ ������� �� false
					ProgExec(ErrProg); //���������� ������
					break;
					
					//��������� ���
				case 10:
					if ((Digit.count(*i)) && (*i != '0')) // --- ����� (1..9); 10 -> 1
					{
						LexAccum += *i; //���������� ������� � �������� ����������
						//TO DO numberFormat = DEC (���������� ��������������� ��������)
						S = 1; //������� � ��������� 1
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (*i == '0') //����� 0; 10 -> 9
					{
						LexAccum += *i; //���������� ������� � �������� ����������
						S = 9; //������� � ��������� 9
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (Seps.count(str.substr(distance(str.begin(), i), 1)) || (*i == ' ' || *i == '\n' || *i == '\t' || *i == '\r') || //�����������; 10 -> 0
					  ABC.count(*i));
					{
						i--; // ��� ��������� ����������
						string* sep = new string;
						*sep = LexAccum; //������ ������� � ����������
						ib = (ib + 1) % SizeBuf; //���������� �������� ������ ������ �������� ������ �� 1
						LexBuf[ib].Load.Clear(); //�������� �������� ��
						LexBuf[ib] = { SeperatAtr, Cstring, sep };  //���������� ������� � ����� �������� ������ � ���� �� {�������, ���, ���������}
						LexOut();
						S = 0; //������� � ��������� 0
						break;
					}
					Work = false;  //��������� ����� �������� ������ ������� �� false
					ProgExec(ErrProg, 0, Bus, nullptr); //���������� ������
					break;
				//��������� ��������� �����������
				case 11:
					if (*i != '\n') //����� ������ ����� (\n); 11 -> 11
					{
						LexAccum += *i;  //���������� ������� � �������� ����������
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (*i == '\n') //������ (\n); 11 -> 12
					{
						LexAccum += *i; //���������� ������� � �������� ����������
						// TO DO tabCounter = 0 (���������� ��������������� ��������)
						S = 12; //������� � ��������� 12
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					Work = false; //��������� ����� �������� ������ ������� �� false
					ProgExec(ErrProg, 0, Bus, nullptr); //���������� ������
					break;
				//��������� ���������
				case 12:
					if (*i != '\n')//����� ������ ����� (\n); 12 -> 0
					{
						/*
						TO DO
						If (bracketAmount > 0)
						������ ��������� �� ���-�� { ������ bracketAmount

						If (bracketAmount < 0)
						������ ��������� �� ���-�� } ������ bracketAmount
						*/
						i--; // ��� ��������� ����������
						string* st3 = new string;
						*st3 = LexAccum; //������ ������� � ����������
						ib = (ib + 1) % SizeBuf; //���������� �������� ������ ������ �������� ������ �� 1
						LexBuf[ib].Load.Clear(); //�������� �������� ��
						LexBuf[ib] = { StrAtr, Cstring, st3 }; //���������� ������� � ����� �������� ������ � ���� �� {�������, ���, ���������}
						LexOut();
						S = 0; //������� � ��������� 0
					//	Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (*i == '\n') //������ (\n); 12 -> 12
					{
					//	LexAccum += *i; //���������� ������� � �������� ����������
						// TO DO tabCounter = 0 (���������� ��������������� ��������)
					//	Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (*i == '\t') //������ (\n); 12 -> 12
					{
						LexAccum += *i; //���������� ������� � �������� ����������
						// TO DO tabCounter++ (���������� ��������������� ��������)
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					Work = false; //��������� ����� �������� ������ ������� �� false
					ProgExec(ErrProg, 0, Bus, nullptr); //���������� ������
					break;
				case 13:
					if (DigitBIN.count(*i)) // --- ����� (0..1); 13 -> 13
					{
						LexAccum += *i; //���������� ������� � �������� ����������
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (*i == '_') //������ ���������� ����� (_); 13 -> 13
					{
					//	Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (DigitSeps.count(*i))  // --- ������ ���������� ������� � ����� ����� (. ,); 13 -> 14
					{
						LexAccum += "."; //���������� ������� � �������� ����������
						S = 2; //������� � ��������� 2
					//	Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (Seps.count(str.substr(distance(str.begin(), i), 1)) || (*i == ' ' || *i == '\n' || *i == '\t' || *i == '\r')) //�����������; 13 -> 0
					{
						i--; // ��� ��������� ����������
						int* tint = new int;
						*tint = atoi(LexAccum.c_str()); //������ ������� � ����������
						ib = (ib + 1) % SizeBuf; //���������� �������� ������ ������ �������� ������ �� 1
						LexBuf[ib].Load.Clear(); //�������� �������� ��
						LexBuf[ib] = { IntAtr, Cint , tint }; //���������� ������� � ����� �������� ������ � ���� �� {�������, ���, ���������}
						S = 0; //������� � ��������� 0

					//	Debug(*i, S, LexAccum); // --- �������
						std::cout << "LexBuf[" << ib << "] = {IntAtr, TBinInt, " << *tint << "}\n"; // --- �������

						break;
					}
					Work = false; //��������� ����� �������� ������ ������� �� false
					ProgExec(ErrProg); //���������� ������
					break;
					// ��������� ������� ����� �������� �����
				case 14:
					if (DigitBIN.count(*i))  // --- ����� (0..1); 14 -> 14
					{
						LexAccum += *i; //���������� ������� � �������� ����������
					//	Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (*i == '_') //������ ���������� ����� (_); 14 -> 14
					{
					//	Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (Seps.count(str.substr(distance(str.begin(), i), 1)) || (*i == ' ' || *i == '\n' || *i == '\t' || *i == '\r')) //�����������; 14 -> 0
					{
						i--; // ��� ��������� ����������
						double* ft = new double;
						*ft = atof(LexAccum.c_str()); //������ ������� � ����������
						ib = (ib + 1) % SizeBuf; //���������� �������� ������ ������ �������� ������ �� 1
						LexBuf[ib].Load.Clear(); //�������� �������� ��
						LexBuf[ib] = { DoubleAtr, Cdouble, ft }; //���������� ������� � ����� �������� ������ � ���� �� {�������, ���, ���������}
						LexOut();
						S = 0; //������� � ��������� 0

					//	Debug(*i, S, LexAccum); // --- �������
						std::cout << "LexBuf[" << ib << "] = {DoubleAtr, TBinDouble, " << *ft << "}\n"; // --- �������

						break;
					}
					Work = false;  //��������� ����� �������� ������ ������� �� false
					ProgExec(ErrProg); //���������� ������
					break;
					// ��������� ����� ����� ������������ �����
				case 15:
					if (DigitOCT.count(*i)) // --- ����� (0..7); 15 -> 15
					{
						LexAccum += *i; //���������� ������� � �������� ����������
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (*i == '_') //������ ���������� ����� (_); 15 -> 15
					{
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (DigitSeps.count(*i))  // --- ������ ���������� ������� � ����� ����� (. ,); 15 -> 16
					{
						LexAccum += "."; //���������� ������� � �������� ����������
						S = 2; //������� � ��������� 2
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (Seps.count(str.substr(distance(str.begin(), i), 1)) || (*i == ' ' || *i == '\n' || *i == '\t' || *i == '\r')) //�����������; 15 -> 0
					{
						i--; // ��� ��������� ����������
						int* tint = new int;
						*tint = atoi(LexAccum.c_str()); //������ ������� � ����������
						ib = (ib + 1) % SizeBuf; //���������� �������� ������ ������ �������� ������ �� 1
						LexBuf[ib].Load.Clear(); //�������� �������� ��
						LexBuf[ib] = { IntAtr, Cint, tint }; //���������� ������� � ����� �������� ������ � ���� �� {�������, ���, ���������}
						S = 0; //������� � ��������� 0

						//Debug(*i, S, LexAccum); // --- �������
						std::cout << "LexBuf[" << ib << "] = {IntAtr, TOctInt, " << *tint << "}\n"; // --- �������

						break;
					}
					Work = false; //��������� ����� �������� ������ ������� �� false
					ProgExec(ErrProg); //���������� ������
					break;
					// ��������� ������� ����� ������������ �����
				case 16:
					if (DigitOCT.count(*i))  // --- ����� (0..7); 16 -> 16
					{
						LexAccum += *i; //���������� ������� � �������� ����������
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (*i == '_') //������ ���������� ����� (_); 16 -> 16
					{
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (Seps.count(str.substr(distance(str.begin(), i), 1)) || (*i == ' ' || *i == '\n' || *i == '\t' || *i == '\r')) //�����������; 16 -> 0
					{
						i--; // ��� ��������� ����������
						double* ft = new double;
						*ft = atof(LexAccum.c_str()); //������ ������� � ����������
						ib = (ib + 1) % SizeBuf; //���������� �������� ������ ������ �������� ������ �� 1
						LexBuf[ib].Load.Clear(); //�������� �������� ��
						LexBuf[ib] = { DoubleAtr, Cdouble, ft }; //���������� ������� � ����� �������� ������ � ���� �� {�������, ���, ���������}
						LexOut();
						S = 0; //������� � ��������� 0

						//Debug(*i, S, LexAccum); // --- �������
						std::cout << "LexBuf[" << ib << "] = {DoubleAtr, TOctDouble, " << *ft << "}\n"; // --- �������

						break;
					}
					Work = false;  //��������� ����� �������� ������ ������� �� false
					ProgExec(ErrProg); //���������� ������
					break;
					// ��������� ����� ����� ����������������� �����
				case 17:
					if (DigitHEX.count(*i)) // --- ����� (0..9, A..F, a..f); 17 -> 17
					{
						LexAccum += *i; //���������� ������� � �������� ����������
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (*i == '_') //������ ���������� ����� (_); 17 -> 17
					{
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (DigitSeps.count(*i))  // --- ������ ���������� ������� � ����� ����� (. ,); 17 -> 2
					{
						LexAccum += "."; //���������� ������� � �������� ����������
						S = 2; //������� � ��������� 2
					//	Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (Seps.count(str.substr(distance(str.begin(), i), 1)) || (*i == ' ' || *i == '\n' || *i == '\t' || *i == '\r')) //�����������; 17 -> 0
					{
						i--; // ��� ��������� ����������
						string* st2 = new string;
						*st2 = LexAccum; //������ ������� � ����������
						ib = (ib + 1) % SizeBuf; //���������� �������� ������ ������ �������� ������ �� 1
						LexBuf[ib].Load.Clear(); //�������� �������� ��
						LexBuf[ib] = { IntAtr, Cint, st2 }; //���������� ������� � ����� �������� ������ � ���� �� {�������, ���, ���������}
						S = 0; //������� � ��������� 0

						//Debug(*i, S, LexAccum); // --- �������
						std::cout << "LexBuf[" << ib << "] = {IntAtr, THexInt, " << *st2 << "}\n"; // --- �������

						break;
					}
					Work = false; //��������� ����� �������� ������ ������� �� false
					ProgExec(ErrProg); //���������� ������
					break;
					// ��������� ������� ����� ����������������� �����
				case 18:
					if (DigitHEX.count(*i))  // --- ����� (0..9, A..F, a..f); 18 -> 18
					{
						LexAccum += *i; //���������� ������� � �������� ����������
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (*i == '_') //������ ���������� ����� (_); 18 -> 18
					{
						//Debug(*i, S, LexAccum); // --- �������
						break;
					}
					if (Seps.count(str.substr(distance(str.begin(), i), 1)) || (*i == ' ' || *i == '\n' || *i == '\t' || *i == '\r')) //�����������; 18 -> 0
					{
						i--; // ��� ��������� ����������
						string* st2 = new string;
						*st2 = LexAccum; //������ ������� � ����������
						ib = (ib + 1) % SizeBuf; //���������� �������� ������ ������ �������� ������ �� 1
						LexBuf[ib].Load.Clear(); //�������� �������� ��
						LexBuf[ib] = { DoubleAtr, Cdouble, st2 }; //���������� ������� � ����� �������� ������ � ���� �� {�������, ���, ���������}
						S = 0; //������� � ��������� 0

						//Debug(*i, S, LexAccum); // --- �������
						std::cout << "LexBuf[" << ib << "] = {DoubleAtr, THexDouble, " << *st2 << "}\n"; // --- �������

						break;
					}
					Work = false;  //��������� ����� �������� ������ ������� �� false
					ProgExec(ErrProg); //���������� ������
					break;
				default:
					break;
				}
		}
		if (Work)
		{
			// Flush a pending newline-state token (case 12 only emits on a
			// non-newline char, so a trailing \n was never dispatched).
			if (S == 12)
			{
				string* st = new string(LexAccum);
				ib = (ib + 1) % SizeBuf;
				LexBuf[ib].Load.Clear();
				LexBuf[ib] = { StrAtr, Cstring, st };
				LexOut();
				S = 0;
			}
			// Empty separator at end-of-input so the parser dispatches its
			// `else` (or Sep="") transition and flushes any final emit.
			ib = (ib + 1) % SizeBuf;
			LexBuf[ib].Load.Clear();
			LexBuf[ib] = { SeperatAtr, Cstring, new string("") };
			LexOut();
		}
		ProgExec(FinProg); // ���������
		break;
		default:
			CommonMk(MK, Load, Sender);
		}
	}
/*
	void Lex::Debug(char i, int S, string LexAccum) // --- ��� �������, ����� �������
	{
		cout << " Current symbol: " << i << "; "; // --- �������
		cout << "New condition: " << S << "; "; // --- �������
		cout << " LexAccum: " << LexAccum << endl; // --- �������
	}
*/
	Lex::Lex(FU *BusContext, FU *Templ) 
	{
		FUtype = 3;
		Bus = BusContext;
		Receiver.back() = BusContext;
		copy(ABC_templ.begin(), ABC_templ.end(), inserter(ABC, ABC.end()));
		copy(Digit_templ.begin(), Digit_templ.end(), inserter(Digit, Digit.end())); // --- ���������� � ��������� �����
		copy(DigitHEX_templ.begin(), DigitHEX_templ.end(), inserter(DigitHEX, DigitHEX.end())); // --- ���������� � ��������� �����
		copy(DigitOCT_templ.begin(), DigitOCT_templ.end(), inserter(DigitOCT, DigitOCT.end())); // --- ���������� � ��������� �����
		copy(Digit_seps_templ.begin(), Digit_seps_templ.end(), inserter(DigitSeps, DigitSeps.end())); // --- ���������� � ��������� ������������ ������� � ����� ������ �����
		copy(Seps_templ.begin(), Seps_templ.end(), inserter(Seps, Seps.end()));
		DigitBIN.insert('0'); DigitBIN.insert('1');
		LexBuf = new ip[SizeBuf];
		for (int i = 0; i < SizeBuf; LexBuf[i++].Load = { 0,nullptr });
		LexBuf[0].atr = SeperatAtr;
		ProgFU(0, {0,nullptr});
	}

	FU* Lex::Copy() // ��������� ����������� ��
	{
		return new Lex(Bus, this);
	}

	FU* Lex::TypeCopy() // ������� �� ������ �� ���� (�� ������� ��������
	{
		return new Lex(Bus, nullptr);
	}
