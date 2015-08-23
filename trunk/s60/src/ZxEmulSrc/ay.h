// ay.h: interface for the ay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AY_H__5B0758DE_354B_4970_9EBF_41D14696A176__INCLUDED_)
#define AFX_AY_H__5B0758DE_354B_4970_9EBF_41D14696A176__INCLUDED_

#include "..\StdAfx.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const ayu16 AmplitudesAY[] = {0, 836, 1212, 1773, 2619, 3875, 5397, 8823, 10392, 16706, 23339, 29292, 36969, 46421, 55195, 65535};
const ayu16 AmplitudesYM[] = {0, 0, 0xF8, 0x1C2, 0x29E, 0x33A, 0x3F2, 0x4D7, 0x610, 0x77F, 0x90A, 0xA42,
		0xC3B, 0xEC2, 0x1137, 0x13A7, 0x1750, 0x1BF9, 0x20DF, 0x2596, 0x2C9D, 0x3579,
		0x3E55, 0x4768, 0x54FF, 0x6624, 0x773B, 0x883F, 0xA1DA, 0xC0FC, 0xE094, 0xFFFF};

class ay  
{
	enum chips {ay8910,ym2149};
public:
	void CalcEnvelope();
	void Calc_Channel_Volumes(void);
	ayu8 GetAYregister(int num);
	void Syntez(wave &sndout);
	void SetChipParameters(chips AYtype, ayu32 AYfreq);
	void Reset();
	void SetAYregister (ayu8 number, ayu8 value);
	void CopyRegs(LPVOID src);
	ay();
	virtual ~ay();

private:
	int StepDelay;
	int Tone_A, Tone_B, Tone_C;
	int Tone_A_enable, Tone_B_enable, Tone_C_enable;
	int	Tone_A_Freq, Tone_B_Freq, Tone_C_Freq;
	int Tone_A_Counter, Tone_B_Counter, Tone_C_Counter;
	int Noise_A, Noise_B, Noise_C;
	int Noise_A_enable, Noise_B_enable, Noise_C_enable;
	int Noise;
	int Noise_Freq;
	int Noise_Counter;
	int Envelope_A, Envelope_B, Envelope_C;
	int Envelope_Freq;
	int Envelope_Shape;
	int Envelope_Volume;
	int Envelope_Counter;
	int Envelope_First_Period;
	int Volume_A, Volume_B, Volume_C;
	int NoiseSeed;
	int LeftVoice;
	int RightVoice;
	ayu32 chip_freq;
	int Registers[14];

public:
	ays16 LevelAL[32];
public:
	ays16 LevelAR[32];
public:
	ays16 LevelBL[32];
public:
	ays16 LevelBR[32];
public:
	ays16 LevelCL[32];
public:
	ays16 LevelCR[32];
private:
	ays16 chip_type;
};

#endif // !defined(AFX_AY_H__5B0758DE_354B_4970_9EBF_41D14696A176__INCLUDED_)
