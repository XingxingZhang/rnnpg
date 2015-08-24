/*
 * Decoder.cpp
 *
 *  Created on: 30 Jan 2014
 *      Author: s1270921
 */

#include "Decoder.h"

//bool stack_item_cmp(StackItem *firstItem, StackItem *secondItem)
//{
//	return firstItem->cost > secondItem->cost;
//}

/**
 * In the first step of implmentation, we do not consider the flush option,
 * although it seems that the results can be improved
 */
int Decoder::decode(vector<string> &prevSents, int stackSize, int K, vector<string> &topSents)
{
	// cout << "in decode" << endl;
	assert(prevSents.size() > 0);
	vector<string> words;
	split(prevSents[0], " ", words);
	int senLen = words.size();

	// cout << "sentence length = " << senLen << endl;

	rnnpg->getContextHiddenNeu(prevSents, contextHiddenNeu);

	// cout << "get context neuron done" << endl;

	int i, j, k;
	neuron *newHiddenNeu = new neuron[hiddenSize];
	Stack **stacks = new Stack*[senLen + 1];
	for(i = 0; i <= senLen; i ++)
		stacks[i] = new Stack(stackSize, hiddenSize);

	if(rnnpg->getFlushOption() == 2)
		rnnpg->getHiddenNeuron(newHiddenNeu);

	StackItem *sitem = NULL;
	sitem = new StackItem(hiddenSize);
	sitem->posInSent = 0;
	sitem->curTrans = sitem->word = "</s>";
	if(rnnpg->getFlushOption() == 2)
		sitem->renewHiddenNeu(newHiddenNeu);
	stacks[0]->push(sitem);

	// now try to consider the previous sentence of the generated sentence
	// try to build a translation table
	vector<string> lwords;
	split(prevSents[prevSents.size() - 1], " ", lwords);

	vector<pair<string,double> > nextWordProbs;
	for(i = 0; i < senLen; i ++)
	{
		Stack *nxStack = stacks[i+1];
		// cout << "i = " << i << ", stack size " << stacks[i]->size() << endl;
		for(j = 0; j < stacks[i]->size(); j ++)
		{
			StackItem *curItem = stacks[i]->get(j);
			// get generation options
			rnnpg->computeNetContext(curItem->word.c_str(), NULL, curItem->hiddenNeu, contextHiddenNeu[curItem->posInSent],
					newHiddenNeu, nextWordProbs);

			// cout << "next words size " << nextWordProbs.size() << endl;
//			TransList *transList = NULL;
//			if(transTable != NULL)
//			{
//				// transList = transTable->getTransList(lwords[i].c_str());
//				if(transList == NULL)
//				{
//					cerr << lwords[i] << " not found in the translation table! impossible!!!" << endl;
//					assert(transList != NULL);
//				}
//			}

			for(k = 0; k < (int)nextWordProbs.size(); k ++)
			{
				string nxWd = nextWordProbs[k].first;
				double nxProb = nextWordProbs[k].second;

//				if(transTable != NULL)
//				{
//					if(!transList->contains(nxWd.c_str()))
//						continue;
//				}

				StackItem *nxItem = new StackItem(hiddenSize);
				nxItem->cost = curItem->cost + log(nxProb);
				nxItem->renewHiddenNeu(newHiddenNeu);
				nxItem->posInSent = curItem->posInSent + 1;
				nxItem->word = nxWd;
				nxItem->curTrans = curItem->curTrans + " " + nxWd;

				// it is impossible to have the same string during decoding, so no recombination is needed
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

	// next we will recompute the cost of the senLen-th stack and it is the senLen-th word in the original sentence
	// add the probability from lastword --> </s>
	for(i = 0; i < stacks[senLen]->size(); i ++)
	{
		StackItem *curItem = stacks[senLen]->get(i);
//		cout << curItem->posInSent << endl;
		// get generation options
		rnnpg->computeNetContext(curItem->word.c_str(), "</s>", curItem->hiddenNeu, contextHiddenNeu[curItem->posInSent],
				newHiddenNeu, nextWordProbs);
		assert(nextWordProbs.size() == 1);
		string nxWd = nextWordProbs[0].first;
		double nxProb = nextWordProbs[0].second;
		curItem->cost = curItem->cost + log(nxProb);
		curItem->curTrans = curItem->curTrans + " " + nxWd;
		curItem->renewHiddenNeu(newHiddenNeu);
	}
	stacks[senLen]->sortByCost();

	topSents.clear();
	char strBuf[1024];
	for(i = 0; i < K && i < stacks[senLen]->size(); i ++)
	{
		StackItem *sitem = stacks[senLen]->get(i);
		string pureSent = removeS(sitem->curTrans);
		snprintf(strBuf, sizeof(strBuf), " ||| %s ||| %f", pureSent.c_str(), sitem->cost);
		topSents.push_back(strBuf);
	}

	delete []newHiddenNeu;
	for(i = 0; i <= senLen; i ++)
		delete stacks[i];
	delete []stacks;

	return 0;
}

int Decoder::decode(const char* infile, const char* outfile, int stackSize, int K, int startId)
{
	FILE *fin = xfopen(infile, "r", "in decoder: decode");
	FILE *fout = xfopen(outfile, "w", "in decoder: decode");

	char buf[1024];
	vector<string> sents;
	vector<string> topSents;
	int cnt = startId;
	while(fgets(buf,sizeof(buf),fin))
	{
		cout << buf;
		split(buf, "\t\r\n", sents);
		decode(sents, stackSize, K, topSents);
//		cout << "decoding done!" << endl;
//		cout << topSents.size() << endl;
		int i;
		for(i = 0; i < (int)topSents.size(); i ++)
		{
			fprintf(fout, "%d%s\n", cnt, topSents[i].c_str());
			fflush(fout);
		}
		for( ; i < K; i ++)
		{
			fprintf(fout, "%d%s\n", cnt, topSents[topSents.size() - 1].c_str());
			fflush(fout);
		}
		cnt ++;
		printf("decoder start id = %d, decoded : %d\n", startId, (cnt-startId));
	}

	fclose(fin);
	fclose(fout);

	return 0;
}

double Decoder::getInvertedLexLogProb(const char *first, const char *second)
{
	vector<string> fwords, swords;
	split(first, " ", fwords);
	split(second, " ", swords);

	double logProb = 0, prob = 0;
	for(size_t i = 0; i < fwords.size(); i ++)
	{
		prob = transTable->getProb(fwords[i].c_str(), swords[i].c_str());
		prob = prob == 0 ? 1e-15 : prob;
		logProb += log(prob);
	}

	return logProb;
}

double Decoder::getLexLogProb(const char *first, const char *second)
{
	vector<string> fwords, swords;
	split(first, " ", fwords);
	split(second, " ", swords);

	double logProb = 0, prob = 0;
	for(size_t i = 0; i < fwords.size(); i ++)
	{
		prob = transTable->getProbInverted(swords[i].c_str(), fwords[i].c_str());
		prob = prob == 0 ? 1e-15 : prob;
		logProb += log(prob);
	}

	return logProb;
}

int Decoder::decodeTransTable(vector<string> &prevSents, int stackSize, int K, vector<string> &topSents)
{
	assert(prevSents.size() > 0);
	vector<string> words;
	split(prevSents[0], " ", words);
	int senLen = words.size();

	int maxNGram = transTable->getMaxNGram();

	rnnpg->getContextHiddenNeu(prevSents, contextHiddenNeu);

	int i, j, k, t;
	neuron *newHiddenNeu = new neuron[hiddenSize];
	Stack **stacks = new Stack*[senLen + 1];
	for(i = 0; i <= senLen; i ++)
		stacks[i] = new Stack(stackSize, hiddenSize);

	if(rnnpg->getFlushOption() == 2)
		rnnpg->getHiddenNeuron(newHiddenNeu);

	StackItem *sitem = NULL;
	sitem = new StackItem(hiddenSize);
	sitem->posInSent = 0;
	sitem->curTrans = sitem->word = "</s>";
	if(rnnpg->getFlushOption() == 2)
		sitem->renewHiddenNeu(newHiddenNeu);
	stacks[0]->push(sitem);

	// now try to consider the previous sentence of the generated sentence
	// try to build a translation table
	vector<string> lwords;
	split(prevSents[prevSents.size() - 1], " ", lwords);

	vector<pair<char*,double> > trans;
	for(i = 0; i < senLen; i ++)
	{
		// Stack *nxStack = stacks[i+1];
		Stack *nxStack = NULL;
		for(j = 0; j < stacks[i]->size(); j ++)
		{
			StackItem *curItem = stacks[i]->get(j);
			string first = "";
			vector<string> curWords;
			for(k = 0; k < maxNGram && i + k < senLen; k ++)
			{
				if(k != 0) first.append(" "); first.append(lwords[i+k]);
				// curWords.push_back(lwords[i+k]);
				// cout << "first " << first << endl;
				transTable->getAllTrans(first.c_str(), trans);
				// cout << "get translation table of " << first << " done " << trans.size() << endl;
				for(t = 0; t < (int)trans.size(); t ++)
				{
					string second = trans[t].first;
					split(trans[t].first, " ", curWords);
					if(repeatGT(curItem->curTrans, second, 2))
						continue;
					// cout << "cur words" << endl;
					// printsvec(curWords);

					// cout << "prev word " << curItem->word << endl;
					// cout << "curItem posInSent " << curItem->posInSent << endl;

					// update feature weights
					double rnnLogProb = rnnpg->computeNetContext(curItem->word.c_str(), curItem->posInSent, curWords, curItem->hiddenNeu,
							contextHiddenNeu, newHiddenNeu);

					// cout << "rnnLogProb = " << rnnLogProb << endl;

//					double invertedPhraseProb = trans[t].second;
//					// double invertedPhraseLogProb = log(invertedPhraseProb == 0 ? 1e-15 : invertedPhraseProb);
//					double invertedPhraseLogProb = log(invertedPhraseProb);
//					double invertedLexLogProb = getInvertedLexLogProb(first.c_str(), trans[t].first);

					StackItem *nxItem = new StackItem(hiddenSize);
					if(disableRNN == 0)
						nxItem->featVals[0] = curItem->featVals[0] + rnnLogProb;
					if(channelOption & 1)
					{
						double invertedPhraseProb = trans[t].second;
						double invertedPhraseLogProb = log(invertedPhraseProb);
						double invertedLexLogProb = getInvertedLexLogProb(first.c_str(), trans[t].first);
						nxItem->featVals[1] = curItem->featVals[1] + invertedPhraseLogProb;
						nxItem->featVals[2] = curItem->featVals[2] + invertedLexLogProb;
					}
					if(channelOption & 2)
					{
						double phraseProb = transTable->getProbInverted(trans[t].first, first.c_str());
						double phraseLogProb = log(phraseProb);
						double lexLogProb = getLexLogProb(first.c_str(), trans[t].first);
						nxItem->featVals[3] = curItem->featVals[3] + phraseLogProb;
						nxItem->featVals[4] = curItem->featVals[4] + lexLogProb;
					}
					nxItem->updateCost(featWeights);

					nxItem->renewHiddenNeu(newHiddenNeu);
					nxItem->posInSent = curItem->posInSent + curWords.size();
					nxItem->word = curWords[curWords.size() - 1];
					nxItem->curTrans = curItem->curTrans;
					for(int ii = 0; ii < (int)curWords.size(); ii ++)
						nxItem->curTrans += " " + curWords[ii];

					nxStack = stacks[i + curWords.size()];

					// it is possible to have the same string during decoding, so no recombination is needed
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
	}

	// next we will recompute the cost of the senLen-th stack and it is the senLen-th word in the original sentence
	// add the probability from lastword --> </s>
	vector<pair<string,double> > nextWordProbs;
	for(i = 0; i < stacks[senLen]->size(); i ++)
	{
		StackItem *curItem = stacks[senLen]->get(i);
		// get generation options
		rnnpg->computeNetContext(curItem->word.c_str(), "</s>", curItem->hiddenNeu, contextHiddenNeu[curItem->posInSent],
				newHiddenNeu, nextWordProbs);
		assert(nextWordProbs.size() == 1);
		string nxWd = nextWordProbs[0].first;
		double nxProb = nextWordProbs[0].second;
		curItem->cost = curItem->cost + log(nxProb) * featWeights[0];		// BUG: feature weights should be considered, but not before
		// curItem->cost = curItem->cost + log(nxProb)  ===> curItem->cost = curItem->cost + log(nxProb) * featWeights[0];
		curItem->featVals[0] = curItem->featVals[0] + log(nxProb);
		curItem->curTrans = curItem->curTrans + " " + nxWd;
		curItem->renewHiddenNeu(newHiddenNeu);
	}
	if(rerank == 1)
		stacks[senLen]->rerankByRNNPG();
	else
		stacks[senLen]->sortByCost();

	topSents.clear();
	char strBuf[1024];
	for(i = 0; i < K && i < stacks[senLen]->size(); i ++)
	{
		StackItem *sitem = stacks[senLen]->get(i);
		string pureSent = removeS(sitem->curTrans);
		string featValStr;
		sitem->getFeatValString(featValStr);
		snprintf(strBuf, sizeof(strBuf), " ||| %s |||%s", pureSent.c_str(), featValStr.c_str());
		topSents.push_back(strBuf);
	}

	delete []newHiddenNeu;
	for(i = 0; i <= senLen; i ++)
		delete stacks[i];
	delete []stacks;

	return 0;
}

int Decoder::decodeTransTable(const char* infile, const char* outfile, int stackSize, int K, int startId)
{
	FILE *fin = xfopen(infile, "r", "in decoder: decodeTransTable");
	FILE *fout = xfopen(outfile, "w", "in decoder: decodeTransTable");

	char buf[1024];
	vector<string> sents;
	vector<string> topSents;
	int cnt = startId;
	while(fgets(buf,sizeof(buf),fin))
	{
		cout << buf;
		split(buf, "\t\r\n", sents);
		decodeTransTable(sents, stackSize, K, topSents);
		int len = strlen(buf);
		while(buf[len-1] == '\r' || buf[len-1] == '\n')
			buf[-- len] = 0;

//		cout << "decoding done!" << endl;
//		cout << topSents.size() << endl;
		int i;
		for(i = 0; i < (int)topSents.size(); i ++)
		{
			fprintf(fout, "%d%s\n", cnt, topSents[i].c_str());
			fflush(fout);
		}
		for( ; i < K; i ++)
		{
			if(topSents.size() != 0)
				fprintf(fout, "%d%s\n", cnt, topSents[topSents.size() - 1].c_str());
			else
				fprintf(fout, "%d ||| %s ||| 0.0 0.0 0.0 0.0 0.0 |||0.0\n", cnt, buf);
			fflush(fout);
		}
		cnt ++;
		printf("decoder start id = %d, decoded : %d\n", startId, (cnt-startId));
		fflush(stdout);

		// just for test
		// break;
	}

	fclose(fin);
	fclose(fout);

	return 0;
}

void Decoder::loadWeights(const char* weightF)
{
	FILE *fin = xfopen(weightF, "r");
	char buf[1024];
	int i = 0;
	while(fgets(buf,sizeof(buf),fin))
	{
		vector<string> fields;
		split(buf, " \t\r\n", fields);
		if(fields.size() == 2)
		{
			if(i >= FEATURE_SIZE)
			{
				cerr << buf;
				cerr << "more features than expected !!!" << endl;
				exit(1);
			}
			featWeights[i] = atof(fields[1].c_str());
			i ++;
		}
	}
	cout << "show loaded features weights:" << endl;
	for(i = 0; i < FEATURE_SIZE; i ++)
		cout << "feature " << i << " = " << featWeights[i] << endl;

	fclose(fin);
}

