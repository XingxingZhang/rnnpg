#ifndef __CONSTRAINTS_H__
#define __CONSTRAINTS_H__

#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;

#include "xutil.h"

class Pos
{
public:
	int lineNo;
	int inLinePos;

	Pos(int no, int pos)
	{
		lineNo = no;
		inLinePos = pos;
	}

	bool nextTo(const Pos &p)
	{
		int abs = inLinePos - p.inLinePos;
		abs = abs < 0 ? -abs : abs;
		return lineNo == p.lineNo && abs == 1;
	}
};

class CharsInfo
{
public:

	void add(string ch, int lineNo, int inLinePos)
	{
		ciIter iter = charsInfo.find(ch);
		Pos pos(lineNo, inLinePos);
		if(iter == charsInfo.end())
		{
			vector<Pos> plist;
			plist.push_back(pos);
			charsInfo[ch] = plist;
		}
		else
			iter->second.push_back(pos);
	}

	void addPoem(vector<string> &poem)
	{
		size_t i, j;
		for(i = 0; i < poem.size(); i ++)
		{
			vector<string> sent;
			split(poem[i], " ", sent);
			for(j = 0; j < sent.size(); j ++)
				add(sent[j], i, j);
		}
	}

	bool containsDiezi(int lineNo)
	{
		ciIter iter;
		for(iter = charsInfo.begin(); iter != charsInfo.end(); ++ iter)
		{
			if(iter->second.size() < 2) continue;
			for(size_t i = 0; i < iter->second.size() - 1; i ++)
			{
				Pos p1 = iter->second[i];
				Pos p2 = iter->second[i+1];
				if(p1.lineNo == lineNo && p1.nextTo(p2))
					return true;
			}
		}

		return false;
	}

	bool constainsDiscRep(int fromLineNo)
	{
		ciIter iter;
		for(iter = charsInfo.begin(); iter != charsInfo.end(); ++ iter)
		{
			int size = iter->second.size();
			if(iter->second.size() == 2 && iter->second[size - 1].lineNo >= fromLineNo)
			{
				Pos p1 = iter->second[0];
				Pos p2 = iter->second[1];
				if(!p1.nextTo(p2))
					return true;
			}
		}
		return false;
	}

	bool repeatGT(int fromLineNo, int num)
	{
		ciIter iter;
		for(iter = charsInfo.begin(); iter != charsInfo.end(); ++ iter)
		{
			int size = iter->second.size();
			if(iter->second.size() > num && iter->second[size - 1].lineNo >= fromLineNo)
				return true;
		}

		return false;
	}

	bool repetitionsGT(int fromLineNo, int num)
	{
		int cnt = 0;
		ciIter iter;
		for(iter = charsInfo.begin(); iter != charsInfo.end(); ++ iter)
		{
			int size = iter->second.size();
			if(size >= 2 && iter->second[size - 1].lineNo >= fromLineNo)
				cnt ++;
		}
		return cnt > num;
	}
private:
	map<string, vector<Pos> > charsInfo;
	typedef map<string, vector<Pos> >::iterator ciIter;
};

static const int NO_REP = 0;
static const int DZ_REP = 1;
static const int DISC_REP = 2;
static const int FREQ_GT2_REP = 4;
static const int REP_GE2_REP = 8;

static int seg5_1[3][2] = {
		{0, 1},
		{2, 3},
		{4, 4}
};
static int seg5_2[3][2] = {
		{0, 1},
		{2, 2},
		{3, 4}
};
static int seg7_1[4][2] = {
		{0, 1},
		{2, 3},
		{4, 5},
		{6, 6}
};
static int seg7_2[4][2] = {
		{0, 1},
		{2, 3},
		{4, 4},
		{5, 6}
};

class Constraints
{
public:
	/**
	 * 0 no repetition
	 * 1 - 0b01		contains diezi. e.g. AABCD
	 * 2 - 0b10		dis-continuous repetition. e.g. ABCAD
	 * 4 - 0b100	repeat more than 2 times. e.g. AABAC
	 * 8 - 0b1000	greater than or equals to 2 repetitions. e.g. ABBCA
	 * @param sent
	 * @return
	 */
	int containsRep(string sentstr)
	{
		vector<string> sent;
		split(sentstr, " ", sent);
		CharsInfo charsInfo;
		const int lineNo = 0;
		for(size_t i = 0; i < sent.size(); i ++)
		{
			string ch = sent[i];
			charsInfo.add(ch, lineNo, i);
		}
		int ret = 0;
		if(charsInfo.containsDiezi(lineNo))
			ret = ret | DZ_REP;
		if(charsInfo.constainsDiscRep(lineNo))
			ret = ret | DISC_REP;
		if(charsInfo.repeatGT(lineNo, 2))
			ret = ret | FREQ_GT2_REP;
		if(charsInfo.repetitionsGT(lineNo, 1))
			ret = ret | REP_GE2_REP;

		return ret;
	}
	/**
	 * Check if the poem meet the character repetition constraints
	 * check from the second sentence, so poem.size() > 1
	 * @param poem
	 * @return
	 */
	bool isRepetitionOK(vector<string> &poem)
	{
		string firstSent = poem[0];
		vector<string> sent;
		split(firstSent, " ", sent);
		int senLen = sent.size();
		int ret1 = containsRep(firstSent);
		if(ret1 != 0)
		{
			// repetition (only once) in second sentence is allowed
			int ret2 = containsRep(poem[1]);
			if((ret2 & FREQ_GT2_REP) != 0 && (ret2 & REP_GE2_REP) != 0)
				return false;
			CharsInfo charsInfo;
			charsInfo.addPoem(poem);
			if(charsInfo.repeatGT(2, 2))
				return false;
			if(charsInfo.repetitionsGT(2, 0))
				return false;

			return true;
		}
		else
		{
			CharsInfo charsInfo;
			charsInfo.addPoem(poem);
			if(charsInfo.repeatGT(0, 2))
				return false;
			if(senLen == 5)
			{
				// println("sen 5, > 1 repetition " + charsInfo.repetitionsGT(0, 1));
				if(charsInfo.repetitionsGT(0, 1))
					return false;
			}
			else // senLen == 7
			{
//				println("sen 7, > 2 repetition " + charsInfo.repetitionsGT(0, 2));
				if(charsInfo.repetitionsGT(0, 2))
					return false;
			}
			return true;
		}
	}

	int find(int segPos[][2], int M, int pos)
	{
		for(int i = 0; i < M; i ++)
			if(pos >= segPos[i][0] && pos <= segPos[i][1])
				return i;
		return -1;
	}

	bool isSegOK(int segPos[][2], int M, int start, int end)
	{
		int si = find(segPos, M, start);
		int ei = find(segPos, M, end);
		if(start != segPos[si][0] || end != segPos[ei][1])
			return false;
		return true;
	}

	bool isSegmentOK(vector<string> &words, int senLen)
	{
		int start = 0, end = -1;
		size_t i;
		for(i = 0; i < words.size(); i ++)
		{
			vector<string> vword;
			split(words[i], " ", vword);
			end = start + vword.size() - 1;
			if(senLen == 5)
			{
				if(!isSegOK(seg5_1, 3, start, end) && !isSegOK(seg5_2, 3, start, end))
					return false;
			}
			else
			{ // senLen == 7
				if(!isSegOK(seg7_1, 4, start, end) && !isSegOK(seg7_2, 4, start, end))
					return false;
			}
			start = end + 1;
		}
		return true;
	}
};

#endif
