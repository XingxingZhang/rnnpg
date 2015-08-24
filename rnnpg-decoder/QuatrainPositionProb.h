/*
 * QuatrainPositionProb.h
 *
 *  Created on: 3 Mar 2014
 *      Author: s1270921
 */

#ifndef QUATRAINPOSITIONPROB_H_
#define QUATRAINPOSITIONPROB_H_

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <climits>
using namespace std;
#include "xutil.h"

class QuatrainPositionProb
{
public:
	QuatrainPositionProb()
	{
		qua5 = new QPItem[5];
		qua7 = new QPItem[7];
		freqCut5 = freqCut7 = INT_MAX;
	}

	~QuatrainPositionProb()
	{
		delete []qua5;
		delete []qua7;
	}

	/*
	 * return P(ch|pos) for 5 char quatrains
	 */
	double getProb5(int pos, const char *ch)
	{
		if(pos < 0 || pos >= 5)
		{
			cerr << "getProb5 should be in [0, 4] " << pos << endl;
			return 0;
		}
		return qua5[pos].getProb(ch, freqCut5);
	}

	/**
	 * return P(pos|ch) for 5 char quatrains
	 */
	double getProb5(const char *ch, int pos)
	{
		if(pos < 0 || pos >= 5)
		{
			cerr << "getProb5 should be in [0, 4] " << pos << endl;
			return 0;
		}
		int tot = find(ch, chfreq5);
		if(tot == 0)
			return 0;

		int freq = qua5[pos].getFreq(ch);
		return (double)freq / tot;
	}

	int getFreq5(int pos, const char *ch)
	{
		if(pos < 0 || pos >= 5)
		{
			cerr << "getFreq5 should be in [0, 4] " << pos << endl;
			return 0;
		}
		return qua5[pos].getFreq(ch, freqCut5);
	}

	/**
	 * P(ch|pos) for 7 char quatrains
	 */
	double getProb7(int pos, const char *ch)
	{
		if(pos < 0 || pos >= 7)
		{
			cerr << "getProb7 should be in [0, 6] " << pos << endl;
			return 0;
		}
		return qua7[pos].getProb(ch, freqCut7);
	}

	/**
	 * P(pos|ch) for 7 char quatrains
	 */
	double getProb7(const char *ch, int pos)
	{
		if(pos < 0 || pos >= 7)
		{
			cerr << "getProb7 should be in [0, 6] " << pos << endl;
			return 0;
		}
		int tot = find(ch, chfreq7);
		if(tot == 0)
			return 0;

		int freq = qua7[pos].getFreq(ch);
		return (double)freq / tot;
	}

	double getFreq7(int pos, const char *ch)
	{
		if(pos < 0 || pos >= 7)
		{
			cerr << "getFreq7 should be in [0, 6] " << pos << endl;
			return 0;
		}
		return qua7[pos].getFreq(ch, freqCut7);
	}

	/**
	 * do not call this function
	 */
	void computeFreqCut(double cutRatio)
	{
		freqCut5 = computeFreqCut(qua5, 5, cutRatio);
		freqCut7 = computeFreqCut(qua7, 7, cutRatio);
		cout << "freq cut 5 " << freqCut5 << endl;
		cout << "freq cut 7 " << freqCut7 << endl;
	}

	void load(const char *infile)
	{
		FILE *fin = xfopen(infile, "r", "QuatrainPositionProb load");

		char buf[1024];
		size_t i, j;
		while(fgets(buf,sizeof(buf),fin))
		{
			vector<string> sens;
			split(buf, "\t\r\n", sens);
			for(i = 0; i < sens.size(); i ++)
			{
				vector<string> words;
				split(sens[i], " ", words);
				if(words.size() == 5)
					for(j = 0; j < words.size(); j ++)
					{
						qua5[j].add(words[j].c_str());
						add2map(words[j].c_str(), chfreq5);
					}
				else if(words.size() == 7)
					for(j = 0; j < words.size(); j ++)
					{
						qua7[j].add(words[j].c_str());
						add2map(words[j].c_str(), chfreq7);
					}
			}
		}

		fclose(fin);
		cout << "load quatrain position data done!" << endl;
	}

	void show()
	{
		int i, pos;
		for(i = 0; i < 5; i ++)
			cout << i << ", total = " << qua5[i].total << endl;

		map<string,int>::iterator iter;
		for(iter = chfreq5.begin(); iter != chfreq5.end(); ++ iter)
		{
			cout << iter->first << ": ";
			char ch[1024];
			strcpy(ch, iter->first.c_str());
			int tot = find(ch, chfreq5);
			for(pos = 0; pos < 5; pos ++)
			{
				int freq = qua5[pos].getFreq(ch);
				cout << "(" << freq << ", " << (double)freq / tot << ") ";
			}
			cout << endl;
		}


		for(i = 0; i < 7; i ++)
			cout << i << ", total = " << qua7[i].total << endl;

		// map<string,int>::iterator iter;
		for(iter = chfreq7.begin(); iter != chfreq7.end(); ++ iter)
		{
			cout << iter->first << ": ";
			char ch[1024];
			strcpy(ch, iter->first.c_str());
			int tot = find(ch, chfreq7);
			for(pos = 0; pos < 7; pos ++)
			{
				int freq = qua7[pos].getFreq(ch);
				cout << "(" << freq << ", " << (double)freq / tot << ") ";
			}
			cout << endl;
		}
	}

private:
	struct QPItem
	{
		int total;
		map<string,int> charFreq;

		QPItem() : total(0)
		{	}

		void add(const char *ch)
		{
			map<string,int>::iterator iter = charFreq.find(ch);
			if(iter != charFreq.end())
				iter->second ++;
			else
				charFreq[ch] = 1;
			total ++;
		}

		double getProb(const char *ch, int freqCut = INT_MAX)
		{
			return (double)getFreq(ch, freqCut) / total;
		}

		int getFreq(const char *ch, int freqCut = INT_MAX)
		{
			map<string,int>::iterator iter = charFreq.find(ch);
			if(iter != charFreq.end())
				return iter->second > freqCut ? freqCut : iter->second;
			else
				return 0;
		}
	};
	QPItem *qua5;
	map<string,int> chfreq5;
	map<string,int> chfreq7;
	QPItem *qua7;
	// do not use it, 0.001 words have very hight frequency ...
	int freqCut5;		// if freq > freqCut: freq = freqCut
	int freqCut7;		// if freq > freqCut: freq = freqCut

	int computeFreqCut(QPItem *qua, int size, double ratio)
	{
		vector<int> freqs;
		int i;
		for(i = 0; i < size; i ++)
		{
			map<string,int>::iterator iter;
			for(iter = qua[i].charFreq.begin(); iter != qua[i].charFreq.end(); ++ iter)
				freqs.push_back(iter->second);
		}
		sort(freqs.begin(), freqs.end());
		int N = freqs.size() * ratio;
		if(N > 0) N --;

		return freqs[N];
	}

	void add2map(const char *s, map<string,int> &cf)
	{
		map<string,int>::iterator iter = cf.find(s);
		if(iter != cf.end())
			iter->second ++;
		else
			cf[s] = 1;
	}
	int find(const char *s, map<string,int> &cf)
	{
		map<string,int>::iterator iter = cf.find(s);
		if(iter == cf.end())
			return 0;
		else
			return iter->second;
	}
};

#endif
