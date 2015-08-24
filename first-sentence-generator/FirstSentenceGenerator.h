/*
 * FirstSentenceGenerator.h
 *
 *  Created on: 10 Mar 2014
 *      Author: s1270921
 */

#ifndef FIRSTSENTENCEGENERATOR_H_
#define FIRSTSENTENCEGENERATOR_H_

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <set>
using namespace std;
#include "rnnlmlib.h"
#include "KenLMM.h"
#include "xutil.h"
#include "ToneRhythm.h"
#include "TonalPattern.h"
#include "Constraints.h"

class FirstSentenceGenerator
{
public:
	FirstSentenceGenerator()
	{
		rnnlm = NULL;
		kenlm = NULL;
		hiddenSize = 0;
		cout << "interpolate weights " << endl;
		for(int i = 0; i < FEATURE_SIZE; i ++)
		{
			interpolateWeights[i] = (double)1 / FEATURE_SIZE;
			cout << interpolateWeights[i] << endl;
		}
	}
	void setRNNLM(CRnnLM *_rnnlm) { rnnlm = _rnnlm; hiddenSize = rnnlm->getHiddenSize(); }
	void setKenLM(KenLMM *_kenlm) { kenlm = _kenlm; }
	void setRNNLMWeight(double rnnWeight)
	{
		interpolateWeights[0] = rnnWeight;
		interpolateWeights[1] = 1 - interpolateWeights[0];
	}
	void loadShixuehanying(const char *infile);
	void loadPingShuiYun(const char *infile);
	void getCandidatePhrase(const vector<string> &keywords, vector<string> &candiPhrase);
	void printShixuehanying();
	void getFirstSentence(const vector<string> &keywords, int topK, int senLen, int MAX_STACK_SIZE, vector<string> &topSents);
private:
	CRnnLM *rnnlm;
	KenLMM *kenlm;
	int hiddenSize;

	map<string,set<string> > shixuehanyingDict;

	enum FSIZE{FEATURE_SIZE = 2};
	double interpolateWeights[FEATURE_SIZE];

	ToneRhythm tr;
	TonalPattern tp;
//	vector<SenTP> firstSenPTs;

	Constraints constraints;

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

	double getLMLogProb(string curTrans, vector<string> &curWords, vector<double> &probs);

	static bool strlencmp(const string& s1, const string& s2)
	{
		return s1.length() < s2.length();
	}

	static bool phrlencmp(const string& s1, const string& s2)
	{
		return getPhraseLen(s1) < getPhraseLen(s2);
	}

	static int getPhraseLen(const string &s)
	{
		int len = 0;
		for(int i = 0; i < (int)s.length(); i ++)
			if(s[i] == ' ')
				len ++;
		return len + 1;
	}

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

	class StackItem
	{
	public:
		StackItem(int hsize) : cost(0), hiddenSize(hsize), posInSent(0),
		tonalPattern(0), validPos(0), curTPIdx(0)
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
		string toFirstSentString(int senLen, vector<SenTP> &firstSenPTs)
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
			+ " ||| " + firstSenPTs[curTPIdx].toString() + " ||| " );
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
		double max2(double a, double b)
		{
			return a > b ? a : b;
		}
		void interpolate(vector<double> rnnprobs, vector<double> kn3probs, double *interpolateWeights, double oldCost)
		{
			double logProb = 0;
			double zeroProb = 1e-24;
			assert(rnnprobs.size() == kn3probs.size());
			for(size_t i = 0; i < rnnprobs.size(); i ++)
			{
				double rnnprob = max2(rnnprobs[i], zeroProb);
				double kn3prob = max2(kn3probs[i], zeroProb);
				logProb += log(interpolateWeights[0] * rnnprob + interpolateWeights[1] * kn3prob);
			}

			cost = oldCost + logProb;
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
		vector<string> words;

		int tonalPattern;
		int validPos;
		int curTPIdx;

	//	 add more features: 0. rnnpg_cost; 1. P(Si|Fi) inverted phrase prob; 2. P(Si|Fi) inverted lexical prob
	//	 3. P(Fi|Si) phrase prob; 4. P(Fi|Si) lexical prob;
	//	 5. P(pos|ch) : the probability of a char 'ch' appearing at the position 'pos'
	//	 6. KN3 Language Model feature

		// there is only two features: RNNLM feature and KN3 LM feature
		enum FSIZE{FEATURE_SIZE = 2};
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
};


#endif /* FIRSTSENTENCEGENERATOR_H_ */
