//============================================================================
// Name        : FirstSentenceGenerator.cpp
// Author      : Xingxing Zhang
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include "FirstSentenceGenerator.h"

void FirstSentenceGenerator::loadShixuehanying(const char* infile)
{
	FILE *fin = xfopen(infile, "r", "load shixuehanying...");
	char buf[1024*5];
	while(fgets(buf,sizeof(buf),fin))
	{
		if(buf[0] == '<')
			continue;
		vector<string> fields;
		split(buf, "\t\r\n", fields);
		if(fields.size() != 3) continue;
		set<string> s;
		shixuehanyingDict[fields[1]] = s;
		map<string,set<string> >::iterator iter = shixuehanyingDict.find(fields[1]);
		vector<string> words;
		split(fields[2], "*", words);
		for(size_t i = 0; i < words.size(); i ++)
			iter->second.insert(words[i]);
	}
	fclose(fin);

	cout << "load shixuehanying done" << endl;
	// printShixuehanying();
}

void FirstSentenceGenerator::printShixuehanying()
{
	map<string,set<string> >::iterator iter;
	int i = 0;
	for(iter = shixuehanyingDict.begin(); iter != shixuehanyingDict.end(); ++ iter)
	{
		cout << "cluster name " << ++ i << " -- " << iter->first << endl;
		set<string>::iterator sIter;
		for(sIter = iter->second.begin(); sIter != iter->second.end(); ++ sIter)
			cout << *sIter << " - ";
		cout << endl;
	}
}

void FirstSentenceGenerator::getCandidatePhrase(const vector<string> &keywords, vector<string> &candiPhrase)
{
	set<string> s;
	size_t i;
	for(i = 0; i < keywords.size(); i ++)
	{
		map<string,set<string> >::iterator iter = shixuehanyingDict.find(keywords[i]);
		s.insert(iter->second.begin(), iter->second.end());
	}
	candiPhrase.clear();
	candiPhrase.insert(candiPhrase.end(), s.begin(), s.end());
}

void FirstSentenceGenerator::getFirstSentence(const vector<string> &keywords, int topK, int senLen, int stackSize, vector<string> &topSents)
{
	cout << "interploate weight" << endl;
	cout << interpolateWeights[0] << " -- " << interpolateWeights[1] << endl;
//	cout << "get first sentence " << endl;
	topSents.clear();
	vector<string> candiPhrase;
	int i, j, k;
	getCandidatePhrase(keywords, candiPhrase);
	sort(candiPhrase.begin(), candiPhrase.end(), phrlencmp);
//	cout << "candi phrase size " << candiPhrase.size() << endl;
//	for(i = 0; i < candiPhrase.size(); i ++)
//		cout << candiPhrase[i] << endl;
	neuron *newHiddenNeu = new neuron[hiddenSize];
	Stack **stacks = new Stack*[senLen + 1];
	for(i = 0; i < senLen; i ++)
		stacks[i] = new Stack(stackSize, hiddenSize);
	stacks[senLen] = new Stack(topK > stackSize ? topK : stackSize, hiddenSize);
	StackItem *sitem = NULL;
	sitem = new StackItem(hiddenSize);
	sitem->posInSent = 0;
	sitem->curTrans = sitem->word = "</s>";
	stacks[0]->push(sitem);

	// if you want to use tonal pattern constraints
	vector<SenTP> firstSenPTs;
	tp.getFirstSenTPs(senLen, firstSenPTs);

	for(i = 0; i < senLen; i ++)
	{
		Stack *nxStack = NULL;
		for(j = 0; j < stacks[i]->size(); j ++)
		{
			StackItem *curItem = stacks[i]->get(j);
			for(k = 0; k < (int)candiPhrase.size(); k ++)
			{
				string phrase = candiPhrase[k];
				vector<string> curWords;
				split(phrase, " ", curWords);
				if(i + (int)curWords.size() > senLen)
					break;

				// before doing anything, check the tonal pattern first!
				int tonalPattern = curItem->tonalPattern;
				int validPos = curItem->validPos;
				int curTPIdx = curItem->curTPIdx;
				for(int cIdx = 0; cIdx < (int)curWords.size(); cIdx ++)
				{
					string ch = curWords[cIdx];
					int tone = tr.getTone(ch);

					int base = 1 << (i+cIdx);
					if(tone == PING)
						validPos = validPos | base;
					else if(tone == ZE)
					{
						validPos = validPos | base;
						tonalPattern = tonalPattern | base;
					}
				}

				for( ; curTPIdx < (int)firstSenPTs.size(); curTPIdx ++)
				{
					SenTP senTP = firstSenPTs[curTPIdx];
					if(senTP.isValidPattern(tonalPattern, validPos))
						break;
				}
				// no suitable tonal pattern
				if(curTPIdx == (int)firstSenPTs.size())
					continue;

				StackItem *nxItem = new StackItem(hiddenSize);

				nxItem->tonalPattern = tonalPattern;
				nxItem->validPos = validPos;
				nxItem->curTPIdx = curTPIdx;


				vector<double> rnnprobs, kn3probs;
				double rnnLogProb = rnnlm->computeNetPhrase(curItem->word.c_str(), curWords,
						curItem->hiddenNeu, newHiddenNeu, rnnprobs);
				double kenLogProb = getLMLogProb(curItem->curTrans, curWords, kn3probs);


				nxItem->featVals[0] = curItem->featVals[0] + rnnLogProb;
				nxItem->featVals[1] = curItem->featVals[1] + kenLogProb;

				nxItem->interpolate(rnnprobs, kn3probs, interpolateWeights, curItem->cost);

				nxItem->renewHiddenNeu(newHiddenNeu);
				nxItem->posInSent = curItem->posInSent + curWords.size();
				nxItem->word = curWords[curWords.size() - 1];

				// .. record used phrase ..
				nxItem->words = curItem->words;
				nxItem->words.push_back(phrase);

				nxItem->curTrans = curItem->curTrans;
				for(int ii = 0; ii < (int)curWords.size(); ii ++)
					nxItem->curTrans += " " + curWords[ii];

				nxStack = stacks[i + curWords.size()];

				// it is possible to have the same string during decoding, so recombination is needed
				if(nxStack->recombine(nxItem))
					continue;

				// when the stack is full, we need to prune the items
				if(nxStack->isFull())
				{
					if(!nxStack->prune(nxItem))
					{
						delete nxItem;
						// when the current value is less than the smallest element in the stack,
						// then other hyposes should be small as they are sorted by cost
						break;
					}
				}
				else
					nxStack->push(nxItem);
			}
		}
	}

	for(i = 0; i < stacks[senLen]->size(); i ++)
	{
		StackItem *curItem = stacks[senLen]->get(i);
		vector<string> curWords;
		curWords.push_back("</s>");
		vector<double> rnnprobs;
		double rnnLogProb = rnnlm->computeNetPhrase(curItem->word.c_str(), curWords, curItem->hiddenNeu, newHiddenNeu, rnnprobs);
		vector<double> kn3probs;
		double kenLogProb = getLMLogProb(curItem->curTrans, curWords, kn3probs);
		curItem->featVals[0] = curItem->featVals[0] + rnnLogProb;
		curItem->featVals[1] = curItem->featVals[1] + kenLogProb;
		curItem->interpolate(rnnprobs, kn3probs, interpolateWeights, curItem->cost);
		curItem->curTrans = curItem->curTrans + " </s>";
		curItem->renewHiddenNeu(newHiddenNeu);
	}
	stacks[senLen]->sortByCost();

	topSents.clear();
	char strBuf[1024];
	int cnt = 0;
	for(i = 0; i < stacks[senLen]->size(); i ++)
	{
		StackItem *sitem = stacks[senLen]->get(i);

//		cout << "before ";
//		printsvec(sitem->words);
		if( this->constraints.isSegmentOK(sitem->words, senLen) )
		{
//			cout << "after ";
//			printsvec(sitem->words);

			string pureSent = removeS(sitem->curTrans);
			string featValStr;
			// sitem->getFeatValString(featValStr);
			snprintf(strBuf, sizeof(strBuf), " ||| %s |||%s", pureSent.c_str(), sitem->toFirstSentString(senLen, firstSenPTs).c_str());
			topSents.push_back(strBuf);

			cnt ++;
			if(cnt >= topK)
				break;
		}
	}

	delete []newHiddenNeu;
	for(i = 0; i <= senLen; i ++)
		delete stacks[i];
	delete []stacks;
}

double FirstSentenceGenerator::getLMLogProb(string curTrans, vector<string> &curWords, vector<double> &probs)
{
	int ngram = 3;
	vector<string> transWords;
	split(curTrans, " ", transWords);
	vector<string> contexts;
	int i = transWords.size() - ngram + 1;
	i = i >= 0 ? i : 0;
	for( ; i < (int)transWords.size(); i ++)
		contexts.push_back(transWords[i]);
	int pos = contexts.size();
	for(i = 0; i < (int)curWords.size(); i ++)
		contexts.push_back(curWords[i]);

	return kenlm->getProbs(contexts, pos, probs);
}

void FirstSentenceGenerator::loadPingShuiYun(const char* infile)
{
	tr.loadAll(infile);
}
