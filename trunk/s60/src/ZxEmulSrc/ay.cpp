// ay.cpp: implementation of the ay class.
//
//////////////////////////////////////////////////////////////////////

#include "..\stdafx.h"
#include "ay.h"
#include "wave.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ay::ay()
{
	Calc_Channel_Volumes();
	Reset();
}

ay::~ay()
{

}

void ay::SetAYregister(ayu8 number, ayu8 value)
{
	int freq;
	Registers[number]=value;
	switch (number) {
	case 0: case 1: //tone chA
		freq=((Registers[1]&0x0f)<<8)+Registers[0];
		if (freq==0) freq++;
		Tone_A_Counter+=(freq-Tone_A_Freq);
		Tone_A_Freq=freq;
		if (Tone_A_Counter<0) Tone_A_Counter=1;
		break;
	case 2: case 3: //tone chB
		freq=((Registers[3]&0x0f)<<8)+Registers[2];
		if (freq==0) freq++;
		Tone_B_Counter+=(freq-Tone_B_Freq);
		Tone_B_Freq=freq;
		if (Tone_B_Counter<0) Tone_B_Counter=1;
		break;
	case 4: case 5: //tone chC
		freq=((Registers[5]&0x0f)<<8)+Registers[4];
		if (freq==0) freq++;
		Tone_C_Counter+=(freq-Tone_C_Freq);
		Tone_C_Freq=freq;
		if (Tone_C_Counter<0) Tone_C_Counter=1;
		break;
	case 6: //noise
		freq=(Registers[6]&0x1f) * 2; //тон в два раза чаще ( подъем/спад )
		if (Noise_Freq!=freq) {
			Noise_Freq=freq;
			Noise_Counter=Noise_Freq;
		};
		break;
	case 7:	//io and mixer
		Tone_A_enable = ((value & 0x01)==0);
		if (!Tone_A_enable) { Tone_A_Freq=32767; Tone_A=true; };
		Tone_B_enable = ((value & 0x02)==0);
		if (!Tone_B_enable) { Tone_B_Freq=32767; Tone_B=true; };
		Tone_C_enable = ((value & 0x04)==0);
		if (!Tone_C_enable) { Tone_C_Freq=32767; Tone_C=true; };
		Noise_A_enable = ((value & 0x08)!=0);
		Noise_B_enable = ((value & 0x10)!=0);
		Noise_C_enable = ((value & 0x20)!=0);
		if (Noise_A_enable&Noise_B_enable&Noise_C_enable) Noise_Freq=31*2;
		break;
	case 8: // a vol
		Volume_A = (value & 0x0f) * 2 + 1;
		if ( Envelope_A = ((value & 0x10)!=0) ) Volume_A=Envelope_Volume;
		break;
	case 9: // b vol
		Volume_B = (value & 0x0f) * 2 + 1;
		if ( Envelope_B = ((value & 0x10)!=0) ) Volume_B=Envelope_Volume;
		break;
	case 10: //c vol
		Volume_C = (value & 0x0f) * 2 + 1;
		if ( Envelope_C = ((value & 0x10)!=0) ) Volume_C=Envelope_Volume;
		break;
	case 11: case 12: //env.freq
		freq=Registers[12]*256+Registers[11];
		if (freq==0) freq++;
		if ((Envelope_A||Envelope_B||Envelope_C)==false) Envelope_Freq=32767;
		if (freq!=Envelope_Freq) {
			Envelope_Freq=freq;
			Envelope_Counter=Envelope_Freq;
			Envelope_First_Period=true;
		};
		break;
	case 13:
		Envelope_Shape=value&0x0f;
		if (Envelope_Shape<4) Envelope_Shape=9;
		if (Envelope_Shape<8) Envelope_Shape=15;
		if (Envelope_Shape<12) Envelope_Volume=31;
		else Envelope_Volume=0;
		if (Envelope_A) Volume_A=Envelope_Volume;
		if (Envelope_B) Volume_B=Envelope_Volume;
		if (Envelope_C) Volume_C=Envelope_Volume;
		Envelope_First_Period=true;
		break;
	default:
		break;
	};
}

void ay::Reset()
{
	for (int i=0;i<14;i++)
		if (i!=7) SetAYregister(i,0);
		else SetAYregister(7,0x3f);
	Tone_A=0;
	Tone_B=0;
	Tone_C=0;
	Tone_A_Counter=0;
	Tone_B_Counter=0;
	Tone_C_Counter=0;
	Noise=0;
}

void ay::SetChipParameters(chips AYtype, ayu32 AYfreq)
{
	chip_type=AYtype;
	chip_freq=AYfreq;
}

void ay::Syntez(wave &sndout) //грубый синтез
{
	ays16 Tmp;
	int CurSteps=0;
	int DoSteps;

	int leftchann;
	int rightchann;

	for (int j=0;j<SamplesInFrame;j++){

		CurSteps+=AYStepsInSampleFloat;
		DoSteps=CurSteps>>8;
		CurSteps-=DoSteps<<8;

		if ((Tone_A_Counter-=DoSteps)<=0) {
			do Tone_A^=Tone_A_enable;
			while ((Tone_A_Counter+=Tone_A_Freq)<0);
		};
		if ((Tone_B_Counter-=DoSteps)<=0) {
			do Tone_B^=Tone_B_enable;
			while ((Tone_B_Counter+=Tone_B_Freq)<0);
		};
		if ((Tone_C_Counter-=DoSteps)<=0) {
			do Tone_C^=Tone_C_enable;
			while ((Tone_C_Counter+=Tone_C_Freq)<0);
		};
		if ((Noise_Counter-=DoSteps)<=0) {
			Noise=(rand())&1;  // For WIN32, was: (Random())&1;
			Noise_Counter+=Noise_Freq;
		};
		if ((Envelope_Counter-=DoSteps)<=0) {
			do CalcEnvelope();
			while ((Envelope_Counter+=Envelope_Freq)<0);
		};

		if ( (Noise|Noise_A_enable) & Tone_A ) {
			leftchann=LevelAL[Volume_A];
			rightchann=LevelAR[Volume_A];
		}
		else {
			leftchann=0; //(-LevelAL[Volume_A]);
			rightchann=0; //(-LevelAR[Volume_A]);
		};

		if ( (Noise|Noise_B_enable) & Tone_B ) {
			leftchann+=LevelBL[Volume_B];
			rightchann+=LevelBR[Volume_B];
		}
		else {
			//leftchann-=LevelBL[Volume_B];
			//rightchann-=LevelBR[Volume_B];
		};

		if ( (Noise|Noise_C_enable) & Tone_C ) {
			leftchann+=LevelCL[Volume_C];
			rightchann+=LevelCR[Volume_C];
		}
		else {
			//leftchann-=LevelCL[Volume_C];
			//rightchann-=LevelCR[Volume_C];
		};

		Tmp=leftchann; //Поменять на sndout.push((short)leftchann);
		sndout.push(Tmp);
		Tmp=rightchann;
		sndout.push(Tmp);
	};
}

ayu8 ay::GetAYregister(int num)
{
	return Registers[num];
}

void ay::Calc_Channel_Volumes()
{
	const int V_AL=100;
	const int V_AR=5;
	const int V_BL=67;
	const int V_BR=67;
	const int V_CL=5;
	const int V_CR=100;

	const int NumberOfChannels=2;
	const int BitsInSample=16;

	const int V_L = V_AL + V_BL + V_CL;
	const int V_R = V_AR + V_BR + V_CR;

	int V_MAX;
	int round;

	if (NumberOfChannels==2) {
		if (V_L>V_R) V_MAX=V_L;
		else V_MAX=V_L;
	}
	else V_MAX=V_L+V_R;

	if (BitsInSample==8) round=127;
	else round=32767;

	for (int i=0;i<32;i++) {
		LevelAL[i] = (AmplitudesYM[i] * round / 65535 * V_AL/V_MAX);
		LevelAR[i] = (AmplitudesYM[i] * round / 65535 * V_AR/V_MAX);
		LevelBL[i] = (AmplitudesYM[i] * round / 65535 * V_BL/V_MAX);
		LevelBR[i] = (AmplitudesYM[i] * round / 65535 * V_BR/V_MAX);
		LevelCL[i] = (AmplitudesYM[i] * round / 65535 * V_CL/V_MAX);
		LevelCR[i] = (AmplitudesYM[i] * round / 65535 * V_CR/V_MAX);
	};
}

void ay::CalcEnvelope()
{
	switch (Envelope_Shape) {
	case 8:
		Envelope_Volume=(Envelope_Volume-1)&31;
		break;
	case 9:
		if (Envelope_Volume!=0) Envelope_Volume--;
		break;
	case 10:
		if (Envelope_First_Period) {
			if (((--Envelope_Volume)&31)==31) {
				Envelope_Volume++;
				Envelope_First_Period=false;
			};
		}
		else {
			if (((++Envelope_Volume)&31)==0) {
				Envelope_Volume--;
				Envelope_First_Period=true;
			};
		};
		break;
	case 11:
		if (Envelope_First_Period)
			Envelope_First_Period=((31&(--Envelope_Volume))!=31);
		break;
	case 12:
		Envelope_Volume=(Envelope_Volume+1)&31;
		break;
	case 13:
		if (Envelope_Volume<31) Envelope_Volume++;
		break;
	case 14:
		if (Envelope_First_Period) {
			if (((++Envelope_Volume)&31)==0) {
				Envelope_Volume--;
				Envelope_First_Period=false;
			};
		}
		else {
			if ( ((--Envelope_Volume)&31) ==31) {
				Envelope_Volume++;
				Envelope_First_Period=true;
			};
		};
		break;
	case 15:
		if (Envelope_First_Period)
			Envelope_First_Period=((31&(++Envelope_Volume))!=0);
		break;
	};
	if (Envelope_A) Volume_A=Envelope_Volume;
	if (Envelope_B) Volume_B=Envelope_Volume;
	if (Envelope_C) Volume_C=Envelope_Volume;
}

void ay::CopyRegs(LPVOID src) {
	ayu8* bsrc=(ayu8*)src;
	for (int i=0; i<14; i++) {
		SetAYregister(i,bsrc[i]);
	};
}
