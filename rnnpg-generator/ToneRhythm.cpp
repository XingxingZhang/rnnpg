/*
 * ToneRhythm.cpp
 *
 *  Created on: 2014年3月12日
 *      Author: xing
 */


#include "ToneRhythm.h"

const char * const ToneRhythm::tone_strs[] = { "上平", "下平", "上声", "去声", "入声" };
const char * const ToneRhythm::psyFiles[] = {"ShangPing", "XiaPing", "ShangSheng", "QuSheng", "RuSheng"};
const int ToneRhythm::FILE_COUNT = 5;


void ToneRhythm::loadAll(string psyTableD)
{
	ancCharDicts.clear();
	int i;
	for(i = 0; i < FILE_COUNT; i ++)
	{
		string path = psyTableD + "/" + psyFiles[i];
		ifstream fin(path.c_str());
		if(!fin.is_open())
		{
			cerr << "open file " << path << " failed!" << endl;
			continue;
		}
		string line;
		map<string, vector<AncChar> > ancCharDict;
		ancCharDicts.push_back(ancCharDict);
		int lastIndex = ancCharDicts.size() - 1;
		while(getline(fin, line))
		{
			vector<string> fields;
			split(line, "\t", fields);
			int tone = getToneIndex(fields[0]);
			string numstr = fields[1];
			string reprChar = fields[2];
			vector<string> chars;
			split(fields[3], " ", chars);

			for(int i = 0; i < chars.size(); i ++)
			{
				string ch = chars[i];
				AncChar ancChar;
				ancChar.ch = ch;
				ancChar.tone = tone;
				ancChar.numstr = numstr;
				ancChar.reprChar = reprChar;

				// cout << ancChar.toString() << endl;

				map<string, vector<AncChar> >::iterator iter =
						ancCharDicts[lastIndex].find(ch);
				if(iter == ancCharDicts[lastIndex].end())
				{
					vector<AncChar> v;
					v.push_back(ancChar);
					ancCharDicts[lastIndex][ch] = v;
				}
				else
					iter->second.push_back(ancChar);
			}
		}
		cout << "load " << path << " done, totally " << ancCharDicts[lastIndex].size() << endl;
	}
}


/**
 * return Ping 1 (0b1), Ze 2 (0b10), Ping and Ze 3 (0b11), Not recorded (0b00)
	Note when this character is not recorded in ShuiPingYun,
	then the function return 0b00
 * @param ch
 * @return
 */
int ToneRhythm::getTone(const string &ch)
{
	int ret = 0;
	for(int i = 0; i < ancCharDicts.size(); i ++)
	{
		map<string, vector<AncChar> >::iterator iter = ancCharDicts[i].find(ch);
		if(iter != ancCharDicts[i].end())
			ret = i < 2 ? (ret | 1) : (ret | 2);
	}
	return ret;
}

bool ToneRhythm::getRhythm(const string &ch, AncChar &ancChar)
{
	for(int i = 0; i < ancCharDicts.size(); i ++)
	{
		map<string, vector<AncChar> >::iterator iter = ancCharDicts[i].find(ch);
		if(iter != ancCharDicts[i].end())
		{
			ancChar = iter->second[0];
			return true;
		}
	}

	return false;
}



