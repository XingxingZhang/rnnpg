/*
 * ToneRhythm.h
 *
 *  Created on: 2014年3月12日
 *      Author: xing
 */

#ifndef TONERHYTHM_H_
#define TONERHYTHM_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
using namespace std;

#include "xutil.h"
#include "AncChar.h"

enum _PSY_{SHANG_PING, XIA_PING, SHANG_SHENG, QU_SHENG, RU_SHENG};
enum _MD_TONE_{NOPZ, PING, ZE, PING_ZE};

class ToneRhythm
{
public:
//	static enum PSY_{SHANG_PING, XIA_PING, SHANG_SHENG, QU_SHENG, RU_SHENG};
	static const char * const tone_strs[];
//	static enum MD_TONE_{NOPZ, PING, ZE, PING_ZE};

	void loadAll(string psyTableD);

	/**
	 * return Ping 1 (0b1), Ze 2 (0b10), Ping and Ze 3 (0b11), Not recorded (0b00)
		Note when this character is not recorded in ShuiPingYun,
		then the function return 0b00
	 * @param ch
	 * @return
	 */
	int getTone(const string &ch);

	bool getRhythm(const string &ch, AncChar &ancChar);

	static char getToneName(int tVal)
	{
		if (tVal == NOPZ)
			return 'N';
		else if (tVal == PING)
			return 'P';
		else if (tVal == ZE)
			return 'Z';
		else if (tVal == PING_ZE)
			return 'A';
		return 'N';
	}

private:
	vector<map<string, vector<AncChar> > > ancCharDicts;
	static const char * const psyFiles[];
	static const int FILE_COUNT;

	int getToneIndex(string tonestr)
	{
		for(int i = 0; i < 5; i ++)
			if(tonestr == string(tone_strs[i]))
				return i;
		return -1;
	}
};

#endif /* TONERHYTHM_H_ */
