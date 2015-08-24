/*
 * Decoder.h
 *
 *  Created on: 30 Jan 2014
 *      Author: s1270921
 */

#ifndef DECODER_H_
#define DECODER_H_

#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <cassert>
#include <queue>
#include <utility>
#include <map>
#include <set>
using namespace std;

#include "RNNPG.h"
#include "TranslationTable.h"
#include "QuatrainPositionProb.h"
#include "KenLMM.h"
#include "ToneRhythm.h"
#include "TonalPattern.h"

class StackItem
{
public:
	StackItem(int hsize) : cost(0), hiddenSize(hsize), posInSent(0), tonalPattern(0), validPos(0), curTPIdx(-1)
	{
		hiddenNeu = new neuron[hiddenSize];
		for(int i = 0; i < FEATURE_SIZE; i ++)
			featVals[i] = 0;
	}
	void ecopy(StackItem &sitem)
	{
		cost = sitem.cost;
		neuron* tmp = hiddenNeu;
		hiddenNeu = sitem.hiddenNeu;
		sitem.hiddenNeu = tmp;
		hiddenSize = sitem.hiddenSize;
		posInSent = sitem.posInSent;
		curTrans = sitem.curTrans;
	}
	void renewHiddenNeu(neuron *newHiddenNeu)
	{
		for(int i = 0; i < hiddenSize; i ++)
			hiddenNeu[i].ac = newHiddenNeu[i].ac;
	}
	string toSubsequentSentString(int senLen, SenTP &sentp)
	{
		string str;
		char buf[128];
		for(int i = 0; i < FEATURE_SIZE; i ++)
		{
			sprintf(buf, " %f", featVals[i]);
			str.append(buf);
		}
		str.append( " ||| " );
		sprintf(buf, "%f", cost);
		str.append(buf);
		str.append( " ||| " + SenTP::inttp2strtp(tonalPattern, validPos, senLen)
		+ " ||| " + sentp.toString() + " ||| " );
		sprintf(buf, "%d", curTPIdx);
		str.append(buf);
	/*
		return curSen + " ||| " + curCost + " ||| " + TonalPattern.inttp2strtp(tonalPattern, validPos, curSen.length())
					+ " ||| " + firstSenPTs.get(curTPIdx) + " ||| " + this.curTPIdx;
					*/
		return str;
	}
	void getFeatValString(string &str)
	{
		str.clear();
		char buf[128];
		for(int i = 0; i < FEATURE_SIZE; i ++)
		{
			sprintf(buf, " %f", featVals[i]);
			str.append(buf);
		}
		str.append( " ||| " );
		sprintf(buf, "%f", cost);
		str.append(buf);
	}
	void updateCost()
	{
		cost = 0;
		for(int i = 0; i < FEATURE_SIZE; i ++)
			cost += featVals[i];
	}
	void updateCost(double *featWeights)
	{
		cost = 0;
		for(int i = 0; i < FEATURE_SIZE; i ++)
		{
			cost += featVals[i] * featWeights[i];
		}
	}
	~StackItem()
	{
		delete []hiddenNeu;
	}
	static bool stack_item_cmp(StackItem *firstItem, StackItem *secondItem)
	{
		return firstItem->cost > secondItem->cost;
	}
	static bool stack_item_rerank_cmp(StackItem *firstItem, StackItem *secondItem)
	{
		return firstItem->featVals[0] > secondItem->featVals[0];
	}
// private:
	double cost;
	neuron *hiddenNeu;
	int hiddenSize;
	int posInSent;
	string curTrans;
	string word;

	// used for checking tonal pattern
	int tonalPattern;
	// record if in certain position, there is only one tone
	// be careful some characters have more than one tone
	// or some characters are not in Pingshuiyun
	int validPos;
	// tonal pattern index used
	int curTPIdx;

	// add more features: 0. rnnpg_cost; 1. P(Si|Fi) inverted phrase prob; 2. P(Si|Fi) inverted lexical prob
	// 3. P(Fi|Si) phrase prob; 4. P(Fi|Si) lexical prob;
	// 5. P(pos|ch) : the probability of a char 'ch' appearing at the position 'pos'
	// 6. KN3 Language Model feature
	enum FSIZE{FEATURE_SIZE = 7};
	double featVals[FEATURE_SIZE];
};

//bool stack_item_cmp(StackItem *firstItem, StackItem *secondItem);

class Stack
{
public:
	Stack(int msize = 200, int hsize = 200) : maxSize(msize), curSize(0)
	{
		arr = new StackItem*[maxSize];
		for(int i = 0; i < maxSize; i ++)
			arr[i] = NULL;
	}
	// do recombine before push into stack
	void push(StackItem *sitem)
	{
		pq.push(make_pair(sitem->cost, curSize));
		transIndex[sitem->curTrans] = curSize;
		arr[curSize++] = sitem;
	}
	bool recombine(StackItem *sitem)
	{
		map<string,int>::iterator iter = transIndex.find(sitem->curTrans);
		if(iter != transIndex.end())
		{
			int index = iter->second;
			if(sitem->cost > arr[index]->cost)
			{
				delete arr[index];
				arr[index] = sitem;
				pq.push(make_pair(sitem->cost, index));
			}
			else
				delete sitem;

			return true;
		}
		return false;
	}
	bool prune(StackItem *sitem)
	{
		while(!pq.empty())
		{
			P top = pq.top();
			double cost = top.first;
			int index = top.second;
			if(cost != arr[index]->cost)
			{
				pq.pop();
				continue;
			}
			if(sitem->cost > cost)
			{
				map<string,int>::iterator iter = transIndex.find(arr[index]->curTrans);
				if(iter != transIndex.end())
					transIndex.erase(iter);

				delete arr[index];
				arr[index] = sitem;
				pq.push(make_pair(sitem->cost, index));
				transIndex[sitem->curTrans] = index;

				return true;
			}

			return false;
		}
		// this should not be executed
		cout << "warnning : only when the stack is full that you can use this function" << endl;

		return false;
	}
	bool isFull() { return curSize >= maxSize; }
	StackItem *pop()
	{
		return arr[--curSize];
	}
	StackItem *get(int i)
	{
		return arr[i];
	}
	int size()
	{
		return curSize;
	}
	void sortByCost()
	{
		if(curSize > 1)
			sort(arr, arr + curSize, StackItem::stack_item_cmp);
	}
	void rerankByRNNPG()
	{
		if(curSize > 1)
			sort(arr, arr + curSize, StackItem::stack_item_rerank_cmp);
	}
	~Stack()
	{
		for(int i = 0; i < curSize; i ++)
			if(arr[i] != NULL) delete arr[i];
		delete []arr;
	}
private:
	StackItem **arr;
	int maxSize;
	int curSize;

	typedef pair<double,int> P;
	priority_queue<P, vector<P>, greater<P> > pq;
	map<string,int> transIndex;
};

class Decoder
{
public:
	Decoder(RNNPG *_rnnpg, TranslationTable *_transTable = NULL, KenLMM *_kenlm = NULL)
		:rnnpg(_rnnpg), transTable(_transTable), posProb(NULL), posProbCut5(1), posProbCut7(1), kenlm(_kenlm)
	{
		hiddenSize = rnnpg->getHiddenSize();
		channelOption = 1;
		rerank = 0;
		disableRNN = 0;
		int i;
		for(i = 0; i < MAX_SEN_LEN; i ++)
			contextHiddenNeu[i] = new neuron[hiddenSize];

		for(i = 0; i < FEATURE_SIZE; i ++)
			featWeights[i] = 1;

		ngramFeatureOn = 0;
		interpolateWeight = 0;

		useToneRhythm = false;
	}

	int decode(vector<string> &prevSents, int stackSize, int K, vector<string> &topSents);
	int decode(const char* infile, const char* outfile, int stackSize, int K, int startId = 0);

	int decodeTransTable(vector<string> &prevSents, int stackSize, int K, vector<string> &topSents);
	int decodeTransTable(const char* infile, const char* outfile, int stackSize, int K, int startId = 0);
	int decodeWithConstraits(vector<string> &prevSents, int stackSize, int K, int tonalPatternIndex, vector<string> &topSents);

	void setChannelOption(int option) { channelOption = option; }
	void setRerank(int rrank) { rerank = rrank; }
	void setDisableRNN(int disable) { disableRNN = disable; }
	void setQuatrainPositionProb(QuatrainPositionProb *qpp) { posProb = qpp; }
	void setPosProbCut5(double cut5) { posProbCut5 = cut5; }
	void setPosProbCut7(double cut7) { posProbCut7 = cut7; }
	void setKenLM(KenLMM *_kenlm) { kenlm = _kenlm; }
	void setNgramFeatureOn(int featOn) { ngramFeatureOn = featOn; }
	void setInterpolateWeight(double interWeight) { interpolateWeight = interWeight; }
	void loadToneRhythm(const char* pingshuiyunFile)
	{
		tr.loadAll(pingshuiyunFile);
		useToneRhythm = true;
	}

	void loadWeights(const char* weightF, bool verbose = true);

	~Decoder()
	{
		for(int i = 0; i < MAX_SEN_LEN; i ++)
			delete []contextHiddenNeu[i];
	}
private:
	RNNPG *rnnpg;
	TranslationTable *transTable;
	int hiddenSize;
	int channelOption;
	int rerank;
	int disableRNN;

	enum SLEN{MAX_SEN_LEN = 8};
	neuron *contextHiddenNeu[MAX_SEN_LEN];

	enum FSIZE{FEATURE_SIZE = 7};
	double featWeights[FEATURE_SIZE];

	QuatrainPositionProb *posProb;

	double posProbCut5;
	double posProbCut7;

	KenLMM *kenlm;
	int ngramFeatureOn;
	double interpolateWeight;

	bool useToneRhythm;
	TonalPattern tp;
	ToneRhythm tr;

	/**
	 * remove the </s> symbol in string 's'
	 * e.g. '</s> 空 山 新 雨 后 </s>'  ==> '空 山 新 雨 后'
	 */
	string removeS(string s)
	{
		int left = s.find('>');
		int right = s.rfind( '<' );

		return s.substr(left + 2, right - left - 3);
	}

	double getInvertedLexLogProb(const char *first, const char *second);
	double getLexLogProb(const char *first, const char *second);
	double getLogPosProb(const char *phrase, int startPos, int senLen);
	double getLMLogProb(string curTrans, vector<string> &curWords);
	double getLMLogProb(string curTrans, vector<string> &curWords, vector<double> &probs);

	bool repeatGT(const string &trans, const string &word, int N)
	{
		vector<string> twords, words;
		split(trans, " ", twords);
		split(word, " ", words);
		int begin = twords.size();
		twords.reserve(twords.size() + words.size());
		twords.insert(twords.end(), words.begin(), words.end());

		for(int i = begin; i < (int)twords.size(); i ++)
		{
			int cnt = 0;
			for(int j = 0; j < i; j ++)
				if(twords[j] == twords[i])
					cnt ++;
			if(cnt >= N)
				return true;
		}

		return false;
	}

	bool badRepeat(const string &trans, const string &word)
	{

		vector<string> twords, words;
		split(trans, " ", twords);
		split(word, " ", words);
		int begin = twords.size();
		twords.reserve(twords.size() + words.size());
		twords.insert(twords.end(), words.begin(), words.end());

		for(int i = begin; i < (int)twords.size(); i ++)
		{
			int cnt = 0;
			for(int j = 0; j < i; j ++)
				if(twords[j] == twords[i])
					cnt ++;
			if(cnt > 1)
				return true;
			if(cnt == 1 && twords[i] != twords[i-1])
				return true;
		}

		return false;
	}

	bool containUsedWords(set<string> &prevWords, const string &second)
	{
		vector<string> words;
		split(second, " ", words);
		for(int i = 0; i < (int)words.size(); i ++)
			if(prevWords.find(words[i]) != prevWords.end())
				return true;

		return false;
	}

	void loadPreviousWords(vector<string> &prevSents, set<string> &prevWords)
	{
		prevWords.clear();
		size_t i, j;
		vector<string> words;
		for(i = 0; i < prevSents.size(); i ++)
		{
			split(prevSents[i], " ", words);
			for(j = 0; j < words.size(); j ++)
				prevWords.insert(words[j]);
		}
	}

	int getPhraseLen(const char*phr)
	{
		int cnt = 0;
		for(int i = 0; phr[i] != '\0'; i ++)
			if(phr[i] == ' ')
				cnt ++;
		return cnt + 1;
	}
};


#endif /* DECODER_H_ */
