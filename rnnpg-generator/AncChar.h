/*
 * AncChar.h
 *
 *  Created on: 2014年3月13日
 *      Author: xing
 */

#ifndef ANCCHAR_H_
#define ANCCHAR_H_

#include <string>
#include <iostream>
using namespace std;

class AncChar
{
public:
	bool isSameRhythm(const AncChar &another)
	{
		return tone == another.tone && numstr == another.numstr
				&& reprChar == another.reprChar;
	}

	string toString()
	{
		char buf[1024];
		snprintf(buf, sizeof(buf), "%s: Tone:%d, nstr:%s, repr:%s", ch.c_str(), tone, numstr.c_str(), reprChar.c_str());
		return string(buf);
	}

	string ch;
	int tone;
	string numstr;
	string reprChar;
};


#endif /* ANCCHAR_H_ */
