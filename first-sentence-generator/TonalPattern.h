/*
 * TonalPattern.h
 *
 *  Created on: 13 Mar 2014
 *      Author: s1270921
 */

#ifndef TONALPATTERN_H_
#define TONALPATTERN_H_

#include <iostream>
#include <string>
#include <stdio.h>
using namespace std;
#include "xutil.h"

const char * const _STD_5QUATRAIN_TPAT_[] = {
	"AZPPZ\nPPZZP\nAPPZZ\nAZZPP\n",
	"AZZPP\nPPZZP\nAPPZZ\nAZZPP\n",
	"APPZZ\nAZZPP\nAZPPZ\nPPZZP\n",
	"PPZZP\nAZZPP\nAZPPZ\nPPZZP\n"
};

const char * const _STD_7QUATRAIN_TPAT_[] = {
	"APAZZPP\nAZPPZZP\nAZAPPZZ\nAPAZZPP\n",
	"APAZPPZ\nAZPPZZP\nAZAPPZZ\nAPAZZPP\n",
	"AZPPZZP\nAPAZZPP\nAPAZPPZ\nAZPPZZP\n",
	"AZAPPZZ\nAAZZZPP\nAPAZPPZ\nAZPPZZP\n"
};

class SenTP
{
public:
	int tonalPattern;
	int validPos;
	int senLen;

	void parse(const string &pat)
	{
		tonalPattern = 0;
		validPos = 0;

		senLen = pat.length();
		int base = 1;
		int preValidPos = 0;
		for(int i = 0; i < (int)pat.length(); i ++)
		{
			char ch = pat[i];
			if(ch == 'Z') // p 0, z 1
			{
				tonalPattern = tonalPattern | base;
				validPos = preValidPos | base;
			}
			else if(ch == 'P')
				validPos = preValidPos | base;
			base = base << 1;
			preValidPos = validPos;
		}
	}

	bool isValidPattern(int pat, int vPos)
	{
		return (tonalPattern & validPos & vPos) == (pat & validPos & vPos);
	}

	static string inttp2strtp(int tonalPattern, int validPos, int senLen)
	{
		string tonalStr = "";
		int base = 1;
		for(int i = 0; i < senLen; i ++)
		{
			if((validPos & base) != 0)
			{
				if((tonalPattern & base) != 0)
					tonalStr += "Z";
				else
					tonalStr += "P";
			}
			else
				tonalStr += "A";
			base = base << 1;
		}
		return tonalStr;
	}

	string toString()
	{
		return inttp2strtp(tonalPattern, validPos, senLen);
	}

	void printPZ()
	{
		int base = 1, i;
		printf("tonalPattern - ");
		for(i = 0; i < senLen; i ++)
		{
			if((tonalPattern & base) != 0)
				printf("Z");
			else
				printf("P");
			base = base << 1;
		}
		putchar('\n');

		printf("validPos - ");
		base = 1;
		for(i = 0; i < senLen; i ++)
		{
			if((validPos & base) != 0)
				printf("Y");
			else
				printf("N");
			base = base << 1;
		}
		putchar('\n');
	}
};

class PoemTP
{
public:
	vector<SenTP> poemtp;

	void addPat(string pat)
	{
		SenTP sentp;
		sentp.parse(pat);
		poemtp.push_back(sentp);
	}
	SenTP& getSenTP(int i)
	{
		return poemtp[i];
	}
};

class TonalPattern
{
public:
	TonalPattern() {	init();	}
	void init()
	{
		int i, j;
		for(i = 0; i < 4; i ++)
		{
			string tpat = _STD_5QUATRAIN_TPAT_[i];
			PoemTP poemtp;
			qua5tps.push_back(poemtp);
			int last = qua5tps.size() - 1;
			vector<string> fields;
			split(tpat, "\n", fields);
			for(j = 0; j < (int)fields.size(); j ++)
				qua5tps[last].addPat(fields[j]);
		}
		for(i = 0; i < 4; i ++)
		{
			string tpat = _STD_7QUATRAIN_TPAT_[i];
			PoemTP poemtp;
			qua7tps.push_back(poemtp);
			int last = qua7tps.size() - 1;
			vector<string> fields;
			split(tpat, "\n", fields);
			for(j = 0; j < (int)fields.size(); j ++)
				qua7tps[last].addPat(fields[j]);
		}
	}

	void getFirstSenTPs(int wordsPerSen, vector<SenTP> &sentps)
	{
		int i;
		if(wordsPerSen == 5)
		{
			for(i = 0; i < (int)qua5tps.size(); i ++)
				sentps.push_back(qua5tps[i].getSenTP(0));
		}
		else
		{
			for(i = 0; i < (int)qua7tps.size(); i ++)
				sentps.push_back(qua7tps[i].getSenTP(0));
		}
	}

	SenTP getSenTP(int wordsPerSen, int tpIndex, int senIndex)
	{
		vector<PoemTP> &quatps = wordsPerSen == 5 ? qua5tps : qua7tps;
		return quatps[tpIndex].getSenTP(senIndex);
	}
private:
	vector<PoemTP> qua5tps;
	vector<PoemTP> qua7tps;
};

#endif /* TONALPATTERN_H_ */
