/*
 * RNNPG.cpp
 *
 *  Created on: 27 Dec 2013
 *      Author: s1270921
 */

#include <iostream>
#include <cstdio>
#include <cassert>
using namespace std;

#include "RNNPG.h"
#include "xutil.h"

//#ifndef M_LN10
//#define M_LN10 2.30258509299404568402  /* log_e 10 */
//#endif
//
//double exp10 (double arg) { return exp(M_LN10 * arg); }

RNNPG::RNNPG()
{
	hiddenSize = 200;
	classSize = 100;
	trainFile[0] = 0;
	validFile[0] = 0;
	testFile[0] = 0;
	vocabClassF[0] = 0;
	randomSeed = 1;
	srand(randomSeed);
	wordEmbeddingFile[0] = 0;

	int i;
	for(i = 0; i < MAX_CON_N; i ++)
	{
		conSyn[i] = NULL;
		conSynOffset[i] = NULL;
	}

	firstTimeInit = true;
	maxIter = 15;
	for(i = 0; i < SEN5_HIGHT; i ++)
		sen5Neu[i] = NULL;
	for(i = 0; i < SEN7_HIGHT; i ++)
		sen7Neu[i] = NULL;

	compressSyn = NULL;
	hisNeu = NULL;
	cmbNeu = NULL;

	for(i = 0; i < 8; i ++)
		map7Syn[i] = NULL;
	for(i = 0; i < 6; i ++)
		map5Syn[i] = NULL;
	conditionNeu = NULL;

	inNeu = NULL;
	hiddenNeu = NULL;
	outNeu = NULL;
	hiddenInSyn = NULL;
	outHiddenSyn = NULL;

	voc_arr = NULL;
	classStart = NULL;
	classEnd = NULL;

	alpha=0.1;
	beta=0.0000001;

	// for temp information
	wordCounter = 0;

	bpttHistory = NULL;
	bpttHiddenNeu = NULL;
	bpttInHiddenNeu = NULL;
	bpttHiddenInSyn = NULL;
	bpttConditionNeu = NULL;

	fixSentenceModelFirstLayer = false;
	logp = 0;
	totalPoemCount = 0;

	senweSyn = NULL;

	randomlyInitSenModelEmbedding = false;
	alphaDivide = 0;
	minImprovement = 1.0001; // 1.0001

	for(i = 0; i < MAX_CON_N; i ++)
		conSyn_backup[i] = NULL;
	for(i = 0; i < MAX_CON_N; i ++)
		conSynOffset_backup[i] = NULL;
	for(i = 0; i < SEN7_HIGHT; i ++)
		sen7Neu_backup[i] = NULL;
	for(i = 0; i < SEN5_HIGHT; i ++)
		sen5Neu_backup[i] = NULL;
	compressSyn_backup = NULL;
	hisNeu_backup = NULL;
	cmbNeu_backup = NULL;
	for(i = 0; i < 8; i ++)
		map7Syn_backup[i] = NULL;
	for(i = 0; i < 6; i ++)
		map5Syn_backup[i] = NULL;
	conditionNeu_backup = NULL;
	senweSyn_backup = NULL;
	inNeu_backup = NULL;
	hiddenNeu_backup = NULL;
	outNeu_backup = NULL;
	hiddenInSyn_backup = NULL;
	outHiddenSyn_backup = NULL;

	modelFile[0] = 0;
	saveModel = 0;

	stableAC = 0.1;
	historyStableAC = 0;

	flushOption = 1;

	consynMin = -10;
	consynMax = 10;
	consynOffset = 5;

	mode = UNDEFINED_MODE;

	outConditionDSyn = NULL;
	bufOutConditionNeu = NULL;
	outConditionDSyn_backup = NULL;
	// bufOutConditionNeu_backup = NULL;

	directError = false;
	conbptt = false;

	isLastSentOfPoem = false;

	conBPTTHis = NULL;
	conBPTTCmbHis = NULL;
	conBPTTCmbSent = NULL;

	contextBPTTSentNum = -1;

	bpttHisCmbSyn = NULL;

	perSentUpdate = false;

	adaGrad = false;

	adaGradEps = 1e-3;
}

RNNPG::~RNNPG()
{
	int i;
	for(i = 0; i < MAX_CON_N; i ++)
	{
		free(conSyn[i]);
		free(conSynOffset[i]);
	}
	for(i = 0; i < SEN5_HIGHT; i ++)
			free(sen5Neu[i]);
	for(i = 0; i < SEN7_HIGHT; i ++)
		free(sen7Neu[i]);
	free(compressSyn);
	free(hisNeu);
	free(cmbNeu);
	for(i = 0; i < 8; i ++)
		free(map7Syn[i]);
	for(i = 0; i < 6; i ++)
		free(map5Syn[i]);
	free(conditionNeu);
	free(inNeu);
	free(hiddenNeu);
	free(outNeu);
	free(hiddenInSyn);
	free(outHiddenSyn);

	free(classStart);
	free(classEnd);

	free(bpttHistory);
	free(bpttHiddenNeu);
	free(bpttInHiddenNeu);
	free(bpttHiddenInSyn);
	free(bpttConditionNeu);

	free(senweSyn);

	for(i = 0; i < MAX_CON_N; i ++)
		free(conSyn_backup[i]);
	for(i = 0; i < MAX_CON_N; i ++)
		free(conSynOffset_backup[i]);
	for(i = 0; i < SEN7_HIGHT; i ++)
		free(sen7Neu_backup[i]);
	for(i = 0; i < SEN5_HIGHT; i ++)
		free(sen5Neu_backup[i]);
	free(compressSyn_backup);
	free(hisNeu_backup);
	free(cmbNeu_backup);
	for(i = 0; i < 8; i ++)
		free(map7Syn_backup[i]);
	for(i = 0; i < 6; i ++)
		free(map5Syn_backup[i]);
	free(conditionNeu_backup);
	free(senweSyn_backup);
	free(inNeu_backup);
	free(hiddenNeu_backup);
	free(outNeu_backup);
	free(hiddenInSyn_backup);
	free(outHiddenSyn_backup);

	free(outConditionDSyn);
	free(bufOutConditionNeu);
	free(outConditionDSyn_backup);
	// free(bufOutConditionNeu_backup);

	free(conBPTTHis);
	free(conBPTTCmbHis);
	free(conBPTTCmbSent);

	free(bpttHisCmbSyn);

	if(adaGrad)
		sumGradSquare.releaseMemory();
}

void RNNPG::initNet()
{
	if(vocab.getVocabSize() == 0)
	{
		cout << "in initNet, vocabulary size = 0!!!" << endl;
		assert(vocab.getVocabSize() != 0);
	}

	int i, j, N;

	// =========== this is all for the sentence model ==============
	// allocate memory and initilize conSyn (the convolution matrix)
	for(i = 0; i < MAX_CON_N; i ++)
	{
		conSyn[i] = (synapse*)xmalloc(hiddenSize * conSeq[i] * sizeof(synapse), "in initNet (con syn)");
		conSynOffset[i] = (synapse*)xmalloc(hiddenSize * conSeq[i] * sizeof(synapse), "in initNet (con syn offset)");
		N = hiddenSize * conSeq[i];
		for(j = 0; j < N; j ++)
		{
			// conSyn[i][j].weight = random(-0.1, 0.1)+random(-0.1, 0.1)+random(-0.1, 0.1);
			// it is important to initilize the matrix with large values
			// conSyn[i][j].weight = random(-10.0, 10.0);   // works much better than random(-0.1, 0.1)+random(-0.1, 0.1)+random(-0.1, 0.1);
			// now try this one
			// I am not sure if this is a good idea, but I will keep it first
			if(consynMin == -0.1 && consynMax == 0.1)
			{
				conSyn[i][j].weight = random(-0.1, 0.1)+random(-0.1, 0.1)+random(-0.1, 0.1);
			}
			else
			{
				double rnd = random(consynMin, consynMax);
				if(rnd < 0)
					rnd -= consynOffset;
				else
					rnd += consynOffset;
				conSyn[i][j].weight = rnd;
			}
		}
	}
	// load word embedding
	if(mode == TRAIN_MODE)
	{
		wdEmbed.load(wordEmbeddingFile);
		cout << "load word embedding done!" << endl;
	}
	// allocate memory and initilize sen5/7Neu (the sentence model)
	int unitNum = 5;
	for(i = 0; i < SEN5_HIGHT; i ++)
	{
		sen5Neu[i] = (neuron*)xmalloc(hiddenSize * unitNum * sizeof(neuron), "in initNet (sen5neu)");
		N = hiddenSize * unitNum;
		for(j = 0; j < N; j ++)
			sen5Neu[i][j].set();
		if(unitNum > 1)	unitNum -= conSeq[i] - 1;
	}
	unitNum = 7;
	for(i = 0; i < SEN7_HIGHT; i ++)
	{
		sen7Neu[i] = (neuron*)xmalloc(hiddenSize * unitNum * sizeof(neuron), "in initNet (sen7neu)");
		N = hiddenSize * unitNum;
		for(j = 0; j < N; j ++)
			sen7Neu[i][j].set();
		if(unitNum > 1)	unitNum -= conSeq[i] - 1;
	}
	// allocate memory and initilization for compression matrix
	compressSyn = (synapse*)xmalloc(2 * hiddenSize * hiddenSize * sizeof(synapse), "in initNet (compress syn)");
	N = 2 * hiddenSize * hiddenSize;
	for(i = 0; i < N; i ++)
		compressSyn[i].weight = random(-0.1, 0.1)+random(-0.1, 0.1)+random(-0.1, 0.1);
	hisNeu = (neuron*)xmalloc(hiddenSize * sizeof(neuron));
	cmbNeu = (neuron*)xmalloc(hiddenSize * 2 * sizeof(neuron));
	// =========== end of data structures for the sentence model ==============

	// allocate memory and initilization for map matrix
	for(i = 0; i < 8; i ++)
	{
		map7Syn[i] = (synapse*)xmalloc(hiddenSize * hiddenSize * sizeof(synapse), "in initNet (map7 syn)");
		N = hiddenSize * hiddenSize;
		for(j = 0; j < N; j ++)
			map7Syn[i][j].weight = random(-0.1, 0.1)+random(-0.1, 0.1)+random(-0.1, 0.1);
	}
	for(i = 0; i < 6; i ++)
	{
		map5Syn[i] = (synapse*)xmalloc(hiddenSize * hiddenSize * sizeof(synapse), "in initNet (map5 syn)");
		N = hiddenSize * hiddenSize;
		for(j = 0; j < N; j ++)
			map5Syn[i][j].weight = random(-0.1, 0.1)+random(-0.1, 0.1)+random(-0.1, 0.1);
	}
	conditionNeu = (neuron*)xmalloc(hiddenSize * sizeof(neuron), "in initNet (condition neu)");

	// allocate memory and initilization for recurrent neural network: input layer, hidden layer and output layer
	inNeu = (neuron*)xmalloc((vocab.getVocabSize() + hiddenSize + hiddenSize) * sizeof(neuron), "in initNet (in neu)");
	N = vocab.getVocabSize() + hiddenSize + hiddenSize;
	for(i = 0; i < N; i ++)
		inNeu[i].set();
	hiddenNeu = (neuron*)xmalloc(hiddenSize * sizeof(neuron), "in initNet (hidden neu)");
	for(i = 0; i < hiddenSize; i ++)
		hiddenNeu[i].set();
	outNeu = (neuron*)xmalloc((vocab.getVocabSize() + classSize)*sizeof(neuron), "in initNet (out neu)");
	N = vocab.getVocabSize() + classSize;
	for(i = 0; i < N; i ++)
		outNeu[i].set();

	hiddenInSyn = (synapse*)xmalloc(hiddenSize * (vocab.getVocabSize() + hiddenSize + hiddenSize) * sizeof(synapse), "in initNet (hidden syn)");
	N = hiddenSize * (vocab.getVocabSize() + hiddenSize + hiddenSize);
	for(i = 0; i < N; i ++)
		hiddenInSyn[i].weight = random(-0.1, 0.1)+random(-0.1, 0.1)+random(-0.1, 0.1);
	outHiddenSyn = (synapse*)xmalloc((vocab.getVocabSize() + classSize) * hiddenSize * sizeof(synapse), "in initNet (hidden syn)");
	N = (vocab.getVocabSize() + classSize) * hiddenSize;
	for(i = 0; i < N; i ++)
		outHiddenSyn[i].weight = random(-0.1, 0.1)+random(-0.1, 0.1)+random(-0.1, 0.1);

	if(vocabClassF[0] != 0)
	{
		vocab.loadVocabClass(vocabClassF);
		classStart = (int*)xmalloc(classSize * sizeof(int));
		classEnd = (int*)xmalloc(classSize * sizeof(int));
		vocab.getClassStartEnd(classStart, classEnd, classSize);
	}
	else
		assignClassLabel();

 	bpttHistory = (int*)xmalloc((SEN7_LENGTH + 1) * sizeof(int), "in initNet (bptt history)");
 	bpttHiddenNeu = (neuron*)xmalloc((SEN7_LENGTH + 1) * hiddenSize * sizeof(neuron), "in initNet (bptt hidden neuron)");
 	bpttInHiddenNeu = (neuron*)xmalloc((SEN7_LENGTH + 1) * hiddenSize * sizeof(neuron), "in initNet (bptt input hidden neuron)");
 	bpttHiddenInSyn = (synapse*)xmalloc(hiddenSize * (vocab.getVocabSize() + hiddenSize + hiddenSize) * sizeof(synapse), "in initNet (bptt hidden in sysnapse)");
 	N = hiddenSize * (vocab.getVocabSize() + hiddenSize + hiddenSize);
 	for(i = 0; i < N; i ++)
 		bpttHiddenInSyn[i].weight = 0;
 	bpttConditionNeu = (neuron*)xmalloc((SEN7_LENGTH + 1) * hiddenSize * sizeof(neuron), "in initNet (bptt condition neuron)");

 	senweSyn = (synapse*)xmalloc(vocab.getVocabSize() * hiddenSize * sizeof(synapse), "in initNet (sentence model word embedding sysnapse)");
 	if(mode == TRAIN_MODE)
 	{
		if(randomlyInitSenModelEmbedding)
		{
			N = vocab.getVocabSize() * hiddenSize;
			for(i = 0; i < N; i ++)
				senweSyn[i].weight = random(-0.1, 0.1)+random(-0.1, 0.1)+random(-0.1, 0.1);
		}
		else
		{
			if(voc_arr == NULL) voc_arr = vocab.getVocab();
			int V = vocab.getVocabSize();
			double *embed = new double[hiddenSize];
			for(i = 0; i < V; i ++)
			{
				int size = wdEmbed.getWordEmbedding(voc_arr[i].wd, embed);
				if(size != hiddenSize)
					cout << "word embedding size = " << size << ", hidden size = " << hiddenSize << endl;
				assert(size == hiddenSize);

				for(j = 0; j < hiddenSize; j ++)
					senweSyn[j*V + i].weight = embed[j];
			}
			delete []embed;
		}
 	}

 	initBackup();

 	// for directErr propagate to input layer
	outConditionDSyn = (synapse*)xmalloc((vocab.getVocabSize() + classSize) * hiddenSize * sizeof(synapse), "in initNet (outConditionDSyn)");
	N = (vocab.getVocabSize() + classSize) * hiddenSize;
	for(i = 0; i < N; i ++) outConditionDSyn[i].weight = random(-0.1, 0.1)+random(-0.1, 0.1)+random(-0.1, 0.1);
	bufOutConditionNeu = (neuron*)xmalloc(hiddenSize * (SEN7_LENGTH + 1) * sizeof(neuron), "in initNet (bufOutConditionNeu)");

	// for recurrent context model bptt training
	const int MAX_SEN_POEM = 15;	// this number should be enough: lvshi is 8, quatrain is 4
	conBPTTHis = (neuron*)xmalloc(MAX_SEN_POEM * hiddenSize * sizeof(neuron), "in initNet (con BPTT History)");
	conBPTTCmbHis = (neuron*)xmalloc(MAX_SEN_POEM * hiddenSize * sizeof(neuron), "in initNet (con BPTT combine history)");
	conBPTTCmbSent = (neuron*)xmalloc(MAX_SEN_POEM * hiddenSize * sizeof(neuron), "in initNet (con BPTT combine sentence)");
	bpttHisCmbSyn = (synapse*)xmalloc(hiddenSize * 2 * hiddenSize * sizeof(synapse), "in initNet (bptt History Combine synapse)");
	N = hiddenSize * 2 * hiddenSize;
	for(i = 0; i < N; i ++)
		bpttHisCmbSyn[i].weight = 0;

	if(adaGrad)
		sumGradSquare.init(this);
}

void RNNPG::loadVocab(const char *trainFile)
{
	char buf[1024];
	char word[WDLEN];
	vocab.add2Vocab("</s>");
	FILE *fin = xfopen(trainFile, "r");
	totalPoemCount = 0;
	totalSentenceCount = 0;
	while( fgets(buf,sizeof(buf),fin) )
	{
		int i = 0;
		int senCnt = 1;
		for(i = 0; buf[i] != '\0'; i ++)
			if(buf[i] == '\t') senCnt ++;
		totalSentenceCount += senCnt;
		i = 0;
		while(buf[i] != '\0')
		{
			while(buf[i] != '\0' && isSep(buf[i])) i ++;
			int j = 0;
			while(buf[i] != '\0' && !isSep(buf[i]))
			{
				if(j < WDLEN - 1)
					word[j++] = buf[i];
				i ++;
			}
			word[j] = 0;
			if(j > 0)
				vocab.add2Vocab(word);
		}
		totalPoemCount ++;
		// every poem has four lines, so 4 end of line!
		vocab.add2Vocab("</s>");
		vocab.add2Vocab("</s>");
		vocab.add2Vocab("</s>");
		vocab.add2Vocab("</s>");
	}
	fclose(fin);
	vocab.reHashVocab(1);
	// vocab.print();
	cout << "load vocabulary done!" << endl;
}

neuron* RNNPG::sen2vec(const vector<string> &words, neuron **senNeu, int SEN_HIGHT)
{
//	double *embedding = new double[hiddenSize];
	int unitNum = words.size(), i, j;

//	// fill first layer with word embedding...
//	for(i = 0; i < (int)words.size(); i ++)
//	{
//		int weSize = wdEmbed.getWordEmbedding(words[i].c_str(), embedding);
//		if(weSize != hiddenSize)
//		{
//			cout << words[i] << ", weSize = " << weSize << ", hiddenSize = " << hiddenSize << endl;
//			delete []embedding;
//			return NULL;
//		}
//		for(j = 0; j < hiddenSize; j ++)
//			senNeu[0][j*unitNum + i].ac = embedding[j];
//	}
//	delete []embedding;
	// fill first layer with word embedding...
	int V = vocab.getVocabSize();
	for(i = 0; i < (int)words.size(); i ++)
	{
		int curWord = vocab.getVocabID(words[i].c_str());
		if(curWord == -1) curWord = vocab.getVocabID("<R>");
		for(j = 0; j < hiddenSize; j ++)
			senNeu[0][j*unitNum + i].ac = senweSyn[V*j + curWord].weight;
	}

	// convolution
	int a, b;
	int unitNumNx = unitNum = words.size();
	for(i = 0; i < SEN_HIGHT - 1; i ++)
	{
		unitNumNx = unitNum - (conSeq[i] - 1);
//		cout << "unit size = " << unitNumNx << endl;
		for(a = 0; a < hiddenSize; a ++)
		{
			int offset = a * unitNum;
			int offsetNx = a * unitNumNx;
			int offsetCon = a * conSeq[i];
			for(b = 0; b < unitNumNx; b ++)
			{
				senNeu[i+1][offsetNx + b].ac = 0;
				for(j = 0; j < conSeq[i]; j ++)
					senNeu[i+1][offsetNx + b].ac += senNeu[i][offset + b + j].ac * conSyn[i][offsetCon + j].weight;
//				cout << senNeu[i+1][offsetNx + b].ac << ",";
				senNeu[i+1][offsetNx + b].fun_ac();
//				cout << senNeu[i+1][offsetNx + b].ac << " ";
			}
//			cout << endl;
		}
//		cout << endl;
		unitNum = unitNumNx;
	}
	assert(unitNumNx == 1);

	return senNeu[SEN_HIGHT-1];
}

void RNNPG::initSent(int senLen)
{
	int unitNum = senLen, i, j, N;
	int SEN_HIGHT = senLen == 5 ? SEN5_HIGHT : SEN7_HIGHT;
	neuron** senNeu = senLen == 5 ? sen5Neu : sen7Neu;
	for(i = 0; i < SEN_HIGHT; i ++)
	{
		N = hiddenSize * unitNum;
		for(j = 0; j < N; j ++)
			senNeu[i][j].set();
		if(unitNum > 1)	unitNum -= conSeq[i] - 1;
	}
//	clearNeurons(hisNeu, hiddenSize, 3);
//	clearNeurons(cmbNeu, hiddenSize * 2, 3);
//	clearNeurons(conditionNeu, hiddenSize, 3);
//	int V = vocab.getVocabSize();
//	clearNeurons(inNeu, V + hiddenSize + hiddenSize, 3);
//	clearNeurons(hiddenNeu, hiddenSize, 3);
//	clearNeurons(outNeu, V + classSize, 3);
//
//	memset(bpttHistory, 0xff, sizeof(int)*(SEN7_LENGTH+1));
//	// clearNeurons(bpttHiddenNeu, hiddenSize * (SEN7_LENGTH+1), 3);
}

void RNNPG::flushNet()
{
	////////////////////////////////////////////////////////////////////////////////////
	int V = vocab.getVocabSize();
	// clearNeurons(inNeu, V + hiddenSize + hiddenSize, 3);
	clearNeurons(inNeu + V, hiddenSize, 3);
	int layer1size = V + hiddenSize + hiddenSize;
	for(int i = V + hiddenSize; i < layer1size; i ++)
	{
		inNeu[i].ac = stableAC;			// like in rnnlm, last hidden layer is initialized to vector of 0.1 values to prevent unstability
		inNeu[i].er = 0;
	}
	clearNeurons(hiddenNeu, hiddenSize, 3);
	// clearNeurons(outNeu, V + classSize, 3);

	memset(bpttHistory, 0xff, sizeof(int)*(SEN7_LENGTH+1));
	// clearNeurons(bpttHiddenNeu, hiddenSize * (SEN7_LENGTH+1), 3);
}

void RNNPG::assignClassLabel()
{
	classStart = (int*)xmalloc(classSize * sizeof(int));
	classEnd = (int*)xmalloc(classSize * sizeof(int));
	memset(classStart, 0, classSize * sizeof(int));
	memset(classEnd, 0, classSize * sizeof(int));
	voc_arr = vocab.getVocab();
	int V = vocab.getVocabSize(), i;
	int tot_freq = 0;
	for(i = 0; i < V; i ++) tot_freq += voc_arr[i].freq;
	double prob = 0; int classIndex = 0;
	classStart[0] = 0;
	for(i = 0; i < V; i ++)
	{
		prob += voc_arr[i].freq / (double)tot_freq;
		if(prob > 1) prob = 1;
		voc_arr[i].classIndex = classIndex;
		if(prob > (double)(classIndex + 1)/classSize)
			if(classIndex < classSize - 1)
			{
				classEnd[classIndex] = i + 1;
				classIndex ++;
				classStart[classIndex] = i + 1;
			}
	}
	classEnd[classIndex] = V;
	// just for test
//	for(i = 0; i < classSize; i ++)
//		cout << "class " << i << ", " << classStart[i] << ", " << classEnd[i] << endl;

	// print information
	// vocab.print();
}

void RNNPG::computeNet(int lastWord, int curWord, int wordPos, synapse **mapSyn)
{
	clearNeurons(conditionNeu, hiddenSize, 1);
	matrixXvector(conditionNeu, hisNeu, mapSyn[wordPos], hiddenSize, 0, hiddenSize, 0, hiddenSize, 0);
	funACNeurons(conditionNeu, hiddenSize);
	int V = vocab.getVocabSize();
	memcpy(inNeu + V, conditionNeu, hiddenSize * sizeof(neuron));
	// go back later...

	// input layer to hidden layer
	clearNeurons(hiddenNeu, hiddenSize, 1);
	matrixXvector(hiddenNeu, inNeu, hiddenInSyn, V + hiddenSize + hiddenSize, 0, hiddenSize, V, V + hiddenSize + hiddenSize, 0);
	int i, N = V + hiddenSize + hiddenSize;
	for(i = 0; i < hiddenSize; i ++)
	{
		hiddenNeu[i].ac += hiddenInSyn[i*N + lastWord].weight;
		hiddenNeu[i].fun_ac();
	}

	// hidden layer to output layer
	// 1. hidden layer to class
	clearNeurons(outNeu + V, classSize, 1);            // clearNeurons(outNeu, classSize, 1); I used outNeu by mistake (it should be outNeu + V)
	matrixXvector(outNeu, hiddenNeu, outHiddenSyn, hiddenSize, V, V + classSize, 0, hiddenSize, 0);

	if(directError)
		// 1. condition layer (in the input layer) to the output layer -- for classes
		matrixXvector(outNeu, inNeu + V, outConditionDSyn, hiddenSize, V, V + classSize, 0, hiddenSize, 0);

	// compute softmax probability
	double sum = 0;
	for(i = 0; i < classSize; i ++)
	{
		if(outNeu[V+i].ac < -50) outNeu[V+i].ac = -50;
		if(outNeu[V+i].ac > 50) outNeu[V+i].ac = 50;
		outNeu[V+i].ac = FAST_EXP(outNeu[V+i].ac);
		sum += outNeu[V + i].ac;
	}
	for(i = 0; i < classSize; i ++)
		outNeu[V + i].ac /= sum;

	// 2. hidden layer to words
	int curClassIndex = voc_arr[curWord].classIndex;
	clearNeurons(outNeu + classStart[curClassIndex], classEnd[curClassIndex] - classStart[curClassIndex], 1);
	matrixXvector(outNeu, hiddenNeu, outHiddenSyn, hiddenSize, classStart[curClassIndex], classEnd[curClassIndex], 0, hiddenSize, 0);

	if(directError)
		// 1. condition layer (in the input layer) to the output layer -- for words
		matrixXvector(outNeu, inNeu + V, outConditionDSyn, hiddenSize, classStart[curClassIndex], classEnd[curClassIndex], 0, hiddenSize, 0);

	sum = 0;
	for(i = classStart[curClassIndex]; i < classEnd[curClassIndex]; i ++)
	{
		if(outNeu[i].ac < -50) outNeu[i].ac = -50;
		if(outNeu[i].ac > 50) outNeu[i].ac = 50;
		outNeu[i].ac = FAST_EXP(outNeu[i].ac);
		sum += outNeu[i].ac;
	}
	for(i = classStart[curClassIndex]; i < classEnd[curClassIndex]; i ++)
		outNeu[i].ac /= sum;
}

void RNNPG::learnSent(int senLen)
{
	double beta2 = alpha * beta;
	int i, j, N = hiddenSize + hiddenSize;
	for(i = 0; i < hiddenSize; i ++)
		hisNeu[i].er *= hisNeu[i].ac * (1 - hisNeu[i].ac);
	clearNeurons(cmbNeu + hiddenSize, hiddenSize, 2);
	// back propagate error from the history representation to sentence top layer (the final representation of the sentence)
	matrixXvector(cmbNeu, hisNeu, compressSyn, hiddenSize + hiddenSize, 0, hiddenSize, hiddenSize, hiddenSize + hiddenSize, 1);
	// update compress matrix
//	if(wordCounter % 10 == 0)
//	{
		for(i = 0; i < hiddenSize; i ++)
			for(j = 0; j < N; j ++)
				compressSyn[i * N + j].weight += alpha * hisNeu[i].er * cmbNeu[j].ac - compressSyn[i * N + j].weight * beta2;
//	}
//	else
//	{
//		for(i = 0; i < hiddenSize; i ++)
//			for(j = 0; j < N; j ++)
//				compressSyn[i * N + j].weight += alpha * hisNeu[i].er * cmbNeu[j].ac;
//	}

	// error propagate in sentence model
	neuron **senNeu = senLen == 5 ? sen5Neu : sen7Neu;
	int SEN_HIGHT = senLen == 5 ? SEN5_HIGHT : SEN7_HIGHT;
	int unitNum = 1, unitNumNx = 1, a = -1, b = -1;
	for(i = 0; i < hiddenSize; i ++)
		senNeu[SEN_HIGHT - 1][i].er = cmbNeu[hiddenSize + i].er;
	for(i = SEN_HIGHT - 2; i >= 0; i --)
	{
		unitNumNx = unitNum + (conSeq[i] - 1);
		int offset = 0, offsetNx = 0, offsetCon = 0;
		// for readability, I just compute the deviation seperately
		for(a = 0; a < hiddenSize; a ++) for(b = 0; b < unitNum; b ++)
		{
			offset = a * unitNum;
			senNeu[i + 1][offset + b].er *= senNeu[i + 1][offset + b].ac * (1 - senNeu[i + 1][offset + b].ac);
		}

		// compute the back propagate error
		if(i != 0 || !fixSentenceModelFirstLayer)
		{
			for(a = 0; a < hiddenSize; a ++)
			{
				offset = a * unitNum;
				offsetNx = a * unitNumNx;
				offsetCon = a * conSeq[i];
				for(b = 0; b < unitNum; b ++)
				{
					for(j = 0; j < conSeq[i]; j ++)
						senNeu[i][offsetNx + b + j].er += senNeu[i + 1][offset + b].er * conSyn[i][offsetCon + j].weight;
				}
			}
			for(a = 0; a < hiddenSize; a ++)
			{
				offsetNx = a * unitNumNx;
				for(b = 0; b < unitNumNx; b ++)
				{
					//assert(senNeu[i][offsetNx + b].er >= -15 && senNeu[i][offsetNx + b].er <= 15);
					if(senNeu[i][offsetNx + b].er < -15) senNeu[i][offsetNx + b].er = -15;
					if(senNeu[i][offsetNx + b].er > 15) senNeu[i][offsetNx + b].er = 15;
				}
			}
		}

		// update the matrix, at this point we do NOT consider the L2 normalization term
		for(a = 0; a < hiddenSize; a ++)
		{
			offset = a * unitNum;
			offsetNx = a * unitNumNx;
			offsetCon = a * conSeq[i];
			for(j = 0; j < conSeq[i]; j ++)
				conSynOffset[i][offsetCon + j].weight = 0;
			for(b = 0; b < unitNum; b ++)
			{
				for(j = 0; j < conSeq[i]; j ++)
					conSynOffset[i][offsetCon + j].weight += alpha * senNeu[i + 1][offset + b].er * senNeu[i][offsetNx + b + j].ac;
			}
		}
		// update the matrix with L2 normalization term
		for(a = 0; a < hiddenSize; a ++)
		{
			offsetCon = a * conSeq[i];
			for(j = 0; j < conSeq[i]; j ++)
				conSyn[i][offsetCon + j].weight += conSynOffset[i][offsetCon + j].weight - beta2 * conSyn[i][offsetCon + j].weight;
		}

		unitNum = unitNumNx;
	}
	// cout << unitNumNx << endl;
	assert(unitNumNx == senLen);

	if(fixSentenceModelFirstLayer)
		return;
	int V = vocab.getVocabSize();
	int newWordCounter = wordCounter - senLen - 1;
	for(i = 0; i < unitNumNx; i ++)
	{
		int word = bpttHistory[i + 1];	// because bpttHistory recorded lastWord, not curWord
		newWordCounter ++;
		if(newWordCounter % 10 == 0)
		{
			for(j = 0; j < hiddenSize; j ++)
			{
				senNeu[0][j*unitNumNx + i].er *= senNeu[0][j*unitNumNx + i].ac * (1 - senNeu[0][j*unitNumNx + i].ac);
				senweSyn[V*j + word].weight += alpha * senNeu[0][j*unitNumNx + i].er - beta2 * senweSyn[V*j + word].weight;
			}
		}
		else
		{
			for(j = 0; j < hiddenSize; j ++)
			{
				senNeu[0][j*unitNumNx + i].er *= senNeu[0][j*unitNumNx + i].ac * (1 - senNeu[0][j*unitNumNx + i].ac);
				senweSyn[V*j + word].weight += alpha * senNeu[0][j*unitNumNx + i].er;
			}
		}
	}
}

void RNNPG::learnSentBPTT(int senLen)
{
	/*
	 neuron* conBPTTHis;
	neuron* conBPTTCmbHis;
	neuron* conBPTTCmbSent;
	 */
	// contextBPTTSentNum
	copyNeurons(conBPTTHis + hiddenSize * contextBPTTSentNum, hisNeu, hiddenSize, 3);
	copyNeurons(conBPTTCmbHis + hiddenSize * contextBPTTSentNum, cmbNeu, hiddenSize, 3);
	copyNeurons(conBPTTCmbSent + hiddenSize * contextBPTTSentNum, cmbNeu + hiddenSize, hiddenSize, 3);

	if(!isLastSentOfPoem) return;

	double beta2 = alpha * beta;
	int i, j, N = hiddenSize + hiddenSize;
	for(int step = contextBPTTSentNum; step > 0; step --)
	{
		for(i = 0; i < hiddenSize; i ++)
			hisNeu[i].er *= hisNeu[i].ac * (1 - hisNeu[i].ac);
		clearNeurons(cmbNeu + hiddenSize, hiddenSize, 2);
		// back propagate error from the history representation to sentence top layer (the final representation of the sentence)
		matrixXvector(cmbNeu, hisNeu, compressSyn, hiddenSize + hiddenSize, 0, hiddenSize, hiddenSize, hiddenSize + hiddenSize, 1);
		// update compress matrix
	//	if(wordCounter % 10 == 0)
	//	{
			for(i = 0; i < hiddenSize; i ++)
				for(j = 0; j < N; j ++)
				{
					// compressSyn[i * N + j].weight += alpha * hisNeu[i].er * cmbNeu[j].ac - compressSyn[i * N + j].weight * beta2;
					// now it is bptt, so compressSyn matrix will not be updated, util all the bptt step is done. The gradient will be store
					bpttHisCmbSyn[i * N + j].weight += alpha * hisNeu[i].er * cmbNeu[j].ac;
				}
	//	}
	//	else
	//	{
	//		for(i = 0; i < hiddenSize; i ++)
	//			for(j = 0; j < N; j ++)
	//				compressSyn[i * N + j].weight += alpha * hisNeu[i].er * cmbNeu[j].ac;
	//	}

		// error propagate in sentence model
		neuron **senNeu = senLen == 5 ? sen5Neu : sen7Neu;
		int SEN_HIGHT = senLen == 5 ? SEN5_HIGHT : SEN7_HIGHT;
		int unitNum = 1, unitNumNx = 1, a = -1, b = -1;
		for(i = 0; i < hiddenSize; i ++)
			senNeu[SEN_HIGHT - 1][i].er = cmbNeu[hiddenSize + i].er;
		for(i = SEN_HIGHT - 2; i >= 0; i --)
		{
			unitNumNx = unitNum + (conSeq[i] - 1);
			int offset = 0, offsetNx = 0, offsetCon = 0;
			// for readability, I just compute the deviation seperately
			for(a = 0; a < hiddenSize; a ++) for(b = 0; b < unitNum; b ++)
			{
				offset = a * unitNum;
				senNeu[i + 1][offset + b].er *= senNeu[i + 1][offset + b].ac * (1 - senNeu[i + 1][offset + b].ac);
			}

			// compute the back propagate error
			if(i != 0 || !fixSentenceModelFirstLayer)
			{
				for(a = 0; a < hiddenSize; a ++)
				{
					offset = a * unitNum;
					offsetNx = a * unitNumNx;
					offsetCon = a * conSeq[i];
					for(b = 0; b < unitNum; b ++)
					{
						for(j = 0; j < conSeq[i]; j ++)
							senNeu[i][offsetNx + b + j].er += senNeu[i + 1][offset + b].er * conSyn[i][offsetCon + j].weight;
					}
				}
				for(a = 0; a < hiddenSize; a ++)
				{
					offsetNx = a * unitNumNx;
					for(b = 0; b < unitNumNx; b ++)
					{
						//assert(senNeu[i][offsetNx + b].er >= -15 && senNeu[i][offsetNx + b].er <= 15);
						if(senNeu[i][offsetNx + b].er < -15) senNeu[i][offsetNx + b].er = -15;
						if(senNeu[i][offsetNx + b].er > 15) senNeu[i][offsetNx + b].er = 15;
					}
				}
			}

			// update the matrix, at this point we do NOT consider the L2 normalization term
			for(a = 0; a < hiddenSize; a ++)
			{
				offset = a * unitNum;
				offsetNx = a * unitNumNx;
				offsetCon = a * conSeq[i];
				for(j = 0; j < conSeq[i]; j ++)
					conSynOffset[i][offsetCon + j].weight = 0;
				for(b = 0; b < unitNum; b ++)
				{
					for(j = 0; j < conSeq[i]; j ++)
						conSynOffset[i][offsetCon + j].weight += alpha * senNeu[i + 1][offset + b].er * senNeu[i][offsetNx + b + j].ac;
				}
			}
			// update the matrix with L2 normalization term
			for(a = 0; a < hiddenSize; a ++)
			{
				offsetCon = a * conSeq[i];
				for(j = 0; j < conSeq[i]; j ++)
					conSyn[i][offsetCon + j].weight += conSynOffset[i][offsetCon + j].weight - beta2 * conSyn[i][offsetCon + j].weight;
			}

			unitNum = unitNumNx;
		}
		// cout << unitNumNx << endl;
		assert(unitNumNx == senLen);

		if(fixSentenceModelFirstLayer)
			return;
		int V = vocab.getVocabSize();
		int newWordCounter = wordCounter - senLen - 1;
		for(i = 0; i < unitNumNx; i ++)
		{
			int word = bpttHistory[i + 1];	// because bpttHistory recorded lastWord, not curWord
			newWordCounter ++;
			if(newWordCounter % 10 == 0)
			{
				for(j = 0; j < hiddenSize; j ++)
				{
					senNeu[0][j*unitNumNx + i].er *= senNeu[0][j*unitNumNx + i].ac * (1 - senNeu[0][j*unitNumNx + i].ac);
					senweSyn[V*j + word].weight += alpha * senNeu[0][j*unitNumNx + i].er - beta2 * senweSyn[V*j + word].weight;
				}
			}
			else
			{
				for(j = 0; j < hiddenSize; j ++)
				{
					senNeu[0][j*unitNumNx + i].er *= senNeu[0][j*unitNumNx + i].ac * (1 - senNeu[0][j*unitNumNx + i].ac);
					senweSyn[V*j + word].weight += alpha * senNeu[0][j*unitNumNx + i].er;
				}
			}
		}

		// now this is the time for bptt -- hisNeu
		clearNeurons(cmbNeu, hiddenSize, 2);
		// back propagate error from the history representation to sentence top layer (the final representation of the sentence)
		matrixXvector(cmbNeu, hisNeu, compressSyn, hiddenSize + hiddenSize, 0, hiddenSize, 0, hiddenSize, 1);
		// update compress matrix, already done at the beginning
		if(step > 1)
		{
			for(i = 0; i < hiddenSize; i ++)
			{
				hisNeu[i].er = cmbNeu[i].er + conBPTTHis[(step - 1) * hiddenSize + i].er;
				hisNeu[i].ac = conBPTTHis[(step - 1) * hiddenSize + i].ac;
				cmbNeu[i].ac = conBPTTCmbHis[(step - 1) * hiddenSize + i].ac;
				cmbNeu[hiddenSize + i].ac = conBPTTCmbSent[(step - 1) * hiddenSize + i].ac;
			}
		}
	}

	// update compressSyn matrix: this is a large step
	N = hiddenSize + hiddenSize;
	for(i = 0; i < hiddenSize; i ++)
		for(j = 0; j < N; j ++)
		{
			compressSyn[i * N + j].weight += bpttHisCmbSyn[i * N + j].weight - compressSyn[i * N + j].weight * beta2;
			bpttHisCmbSyn[i * N + j].weight = 0;
		}
}

void RNNPG::learnNet(int lastWord, int curWord, int wordPos, int senLen)
{
	double beta2 = alpha * beta;
	int curClassIndex = voc_arr[curWord].classIndex, i = 0, j = 0, V = vocab.getVocabSize(), N = 0, offset = 0;
	// error at output layer. 1. error on words
	for(i = classStart[curClassIndex]; i < classEnd[curClassIndex]; i ++)
		outNeu[i].er = 0 - outNeu[i].ac;
	outNeu[curWord].er = 1 - outNeu[curWord].ac;
	// error at output layer. 2. error on classes
	N = V + classSize;
	for(i = V; i < N; i ++)
		outNeu[i].er = 0 - outNeu[i].ac;
	outNeu[V + curClassIndex].er = 1 - outNeu[V + curClassIndex].ac;

	clearNeurons(hiddenNeu, hiddenSize, 2);

	// error backpropagation to hidden layer
	// 1. error from words to hidden
	matrixXvector(hiddenNeu, outNeu, outHiddenSyn, hiddenSize, classStart[curClassIndex], classEnd[curClassIndex], 0, hiddenSize, 1);
	// 2. error from classes to hidden
	matrixXvector(hiddenNeu, outNeu, outHiddenSyn, hiddenSize, V, V + classSize, 0, hiddenSize, 1);

	if(directError)
	{
		// bufOutConditionNeu
		clearNeurons(bufOutConditionNeu + (wordPos * hiddenSize), hiddenSize, 2);
		// error back propagate to conditionNeu
		// 1. error from words to conditionNeu
		matrixXvector(bufOutConditionNeu + (wordPos * hiddenSize), outNeu, outConditionDSyn, hiddenSize, classStart[curClassIndex], classEnd[curClassIndex], 0, hiddenSize, 1);
		// 2. error from classes to conditionNeu
		matrixXvector(bufOutConditionNeu + (wordPos * hiddenSize), outNeu, outConditionDSyn, hiddenSize, V, V + classSize, 0, hiddenSize, 1);
	}

	// updating the matrix outHiddenSyn, since we already have the error at output layer and the activation in the hidden layer
	// we update the weight per word rather than per sentence for faster increase in likelihood. Perhaps it will be modified to per sentence update later
	// update submatrix of words to hidden layer
	offset = classStart[curClassIndex] * hiddenSize;
	for(i = classStart[curClassIndex]; i < classEnd[curClassIndex]; i ++)
	{
		if(wordCounter % 10 == 0)
			for(j = 0; j < hiddenSize; j ++) outHiddenSyn[offset + j].weight += alpha * outNeu[i].er * hiddenNeu[j].ac - beta2*outHiddenSyn[offset + j].weight;
		else
			for(j = 0; j < hiddenSize; j ++) outHiddenSyn[offset + j].weight += alpha * outNeu[i].er * hiddenNeu[j].ac;
		offset += hiddenSize;
	}
	// update submatrix of classes to hidden layer
	N = V + classSize;
	offset = V * hiddenSize;
	for(i = V; i < N; i ++)
	{
		if(wordCounter % 10 == 0)
			for(j = 0; j < hiddenSize; j ++) outHiddenSyn[offset + j].weight += alpha * outNeu[i].er * hiddenNeu[j].ac - beta2*outHiddenSyn[offset + j].weight;
		else
			for(j = 0; j < hiddenSize; j ++) outHiddenSyn[offset + j].weight += alpha * outNeu[i].er * hiddenNeu[j].ac;
		offset += hiddenSize;
	}

	if(directError)
	{
		// update the matrix outConditionDSyn
		// update submatrix of words to condition layer
		offset = classStart[curClassIndex] * hiddenSize;
		for(i = classStart[curClassIndex]; i < classEnd[curClassIndex]; i ++)
		{
			// how can I make such stupid bug !!! hiddenNeu[V + j].ac is obviously incorrect !!! inNeu[V + j].ac
			if(wordCounter % 10 == 0)
				for(j = 0; j < hiddenSize; j ++) outConditionDSyn[offset + j].weight += alpha * outNeu[i].er * inNeu[V + j].ac - beta2*outConditionDSyn[offset + j].weight;
			else
				for(j = 0; j < hiddenSize; j ++) outConditionDSyn[offset + j].weight += alpha * outNeu[i].er * inNeu[V + j].ac;
			offset += hiddenSize;
		}

		// update submatrix of classes to condition layer
		N = V + classSize;
		offset = V * hiddenSize;
		for(i = V; i < N; i ++)
		{
			// how can I make such stupid bug !!! hiddenNeu[V + j].ac is obviously incorrect !!! inNeu[V + j].ac
			if(wordCounter % 10 == 0)
				for(j = 0; j < hiddenSize; j ++) outConditionDSyn[offset + j].weight += alpha * outNeu[i].er * inNeu[V + j].ac - beta2*outConditionDSyn[offset + j].weight;
			else
				for(j = 0; j < hiddenSize; j ++) outConditionDSyn[offset + j].weight += alpha * outNeu[i].er * inNeu[V + j].ac;
			offset += hiddenSize;
		}
	}

	// this is for back propagation through time
	bpttHistory[wordPos] = lastWord;	// store the last word
	memcpy(bpttHiddenNeu + (wordPos * hiddenSize), hiddenNeu, sizeof(neuron)*hiddenSize);	// store the hidden layer
	memcpy(bpttInHiddenNeu + (wordPos * hiddenSize), inNeu + (V + hiddenSize), sizeof(neuron)*hiddenSize);	// store the hidden units in input layer (previous hidden layer)
	memcpy(bpttConditionNeu + (wordPos * hiddenSize), inNeu + V, sizeof(neuron)*hiddenSize);	// store the condition units in input layer
	if(curWord != 0)
		return;
	// if this is the end of sentence, then let's do it
	int lword = -1, layer1Size = V + hiddenSize + hiddenSize;
	synapse **mapSyn = NULL;
	mapSyn = senLen == 5 ? map5Syn : map7Syn;
	for(int step = wordPos; step >= 0; step --)
	{
		// take care of vocabulary and recurrent part in input layer first
		// bpttHiddenInSyn[]
		for(i = 0; i < hiddenSize; i ++)
			hiddenNeu[i].er *= hiddenNeu[i].ac * (1 - hiddenNeu[i].ac);
		lword = bpttHistory[step];
		// accumulate deviations for input matrix, X
		for(i = 0; i < hiddenSize; i ++)
			bpttHiddenInSyn[i * layer1Size + lword].weight += alpha * hiddenNeu[i].er;

		clearNeurons(inNeu + (V+hiddenSize), hiddenSize, 2);
		matrixXvector(inNeu, hiddenNeu, hiddenInSyn, layer1Size, 0, hiddenSize, V + hiddenSize, layer1Size, 1);
		// accumulate deviations for hidden matrix, R
		for(i = 0; i < hiddenSize; i ++)
			for(j = V + hiddenSize; j < layer1Size; j ++)
				bpttHiddenInSyn[i*layer1Size + j].weight += alpha * hiddenNeu[i].er * inNeu[j].ac;

		// now we take care the condition part in the input layer
		// back propagate the error to condition part
		clearNeurons(inNeu + V, hiddenSize, 2);
		matrixXvector(inNeu, hiddenNeu, hiddenInSyn, layer1Size, 0, hiddenSize, V, V + hiddenSize, 1);
		// accumulate deviations for condition matrix, H
		N = V + hiddenSize;
		for(i = 0; i < hiddenSize; i ++)
			for(j = V; j < N; j ++)
				bpttHiddenInSyn[i*layer1Size + j].weight += alpha * hiddenNeu[i].er * inNeu[j].ac;

		if(directError)
		{
			for(i = 0; i < hiddenSize; i ++)
				inNeu[V + i].er += bufOutConditionNeu[step * hiddenSize + i].er;
		}

		for(i = V; i < N; i ++)
			inNeu[i].er *= inNeu[i].ac * (1 - inNeu[i].ac);

		if(perSentUpdate)
			clearNeurons(hisNeu, hiddenSize, 2);

		// watch that the error in hisNeu must be inilizated to zero at the beginning of dealing with each sentence
		matrixXvector(hisNeu, inNeu + V, mapSyn[step], hiddenSize, 0, hiddenSize, 0, hiddenSize, 1);

		// acumulate deviations for map matrix
		if(wordCounter % 10 == 0)
		{
			for(i = 0; i < hiddenSize; i ++)
				for(j = 0; j < hiddenSize; j ++)
					mapSyn[step][i * hiddenSize + j].weight += alpha * inNeu[V+i].er * hisNeu[j].ac - mapSyn[step][i * hiddenSize + j].weight * beta2;
		}
		else
		{
			for(i = 0; i < hiddenSize; i ++)
				for(j = 0; j < hiddenSize; j ++)
					mapSyn[step][i * hiddenSize + j].weight += alpha * inNeu[V+i].er * hisNeu[j].ac;
		}

		if(perSentUpdate)
			learnSent(senLen);

		if(step == 0) continue;
		// propagate error to previous layer
		for(i = 0; i < hiddenSize; i ++)
		{
			hiddenNeu[i].er = inNeu[V + hiddenSize + i].er + bpttHiddenNeu[(step - 1)*hiddenSize + i].er;
			hiddenNeu[i].ac = bpttHiddenNeu[(step - 1)*hiddenSize + i].ac;
		// restore the recurrent part in input layer
			inNeu[V + hiddenSize + i].ac = bpttInHiddenNeu[(step - 1)*hiddenSize + i].ac;
		// restore the condition part in input layer
			inNeu[V + i].ac = bpttConditionNeu[(step - 1)*hiddenSize + i].ac;
		}
	}

	// restore hidden layer
	memcpy(hiddenNeu, bpttHiddenNeu + (wordPos * hiddenSize), sizeof(neuron)*hiddenSize);


	// update input matrix, X, condition Matrix, H and recurrent matrix, R
	for(i = 0; i < hiddenSize; i ++)
	{
//		if(wordCounter % 10 == 0)
//		{
			for(j = 0; j <= wordPos; j ++)
			{
				lword = bpttHistory[j];
				hiddenInSyn[i*layer1Size + lword].weight += bpttHiddenInSyn[i*layer1Size + lword].weight - hiddenInSyn[i*layer1Size + lword].weight * beta2;
				bpttHiddenInSyn[i*layer1Size + lword].weight = 0;
			}
//		}
//		else
//		{
//			for(j = 0; j <= wordPos; j ++)
//			{
//				lword = bpttHistory[j];
//				hiddenInSyn[i*layer1Size + lword].weight += bpttHiddenInSyn[i * layer1Size + lword].weight;
//				bpttHiddenInSyn[i * layer1Size + lword].weight = 0;
//			}
//		}

//		if(wordCounter % 10 == 0)
//		{
			N = V + hiddenSize;
			for(j = V; j < N; j ++)
			{
				hiddenInSyn[i*layer1Size + j].weight += bpttHiddenInSyn[i*layer1Size + j].weight - hiddenInSyn[i*layer1Size + j].weight * beta2;
				bpttHiddenInSyn[i*layer1Size + j].weight = 0;
			}
//		}
//		else
//		{
//			N = V + hiddenSize;
//			for(j = V; j < N; j ++)
//			{
//				hiddenInSyn[i*layer1Size + j].weight += bpttHiddenInSyn[i*layer1Size + j].weight;
//				bpttHiddenInSyn[i*layer1Size + j].weight = 0;
//			}
//		}

//		if(wordCounter % 10 == 0)
//		{
			for(j = V + hiddenSize; j < layer1Size; j ++)
			{
				hiddenInSyn[i*layer1Size + j].weight += bpttHiddenInSyn[i*layer1Size + j].weight - hiddenInSyn[i*layer1Size + j].weight * beta2;
				bpttHiddenInSyn[i*layer1Size + j].weight = 0;
			}
//		}
//		else
//		{
//			for(j = V + hiddenSize; j < layer1Size; j ++)
//			{
//				hiddenInSyn[i*layer1Size + j].weight += bpttHiddenInSyn[i*layer1Size + j].weight;
//				bpttHiddenInSyn[i*layer1Size + j].weight = 0;
//			}
//		}
	}

	if(!perSentUpdate)
	{
		// update parameters in sentences
		if(!conbptt)
			learnSent(senLen);
		else
			learnSentBPTT(senLen);
	}
}

void RNNPG::learnSentAdaGrad(int senLen)
{
	double beta2 = alpha * beta;
	int i, j, N = hiddenSize + hiddenSize;
	for(i = 0; i < hiddenSize; i ++)
		hisNeu[i].er *= hisNeu[i].ac * (1 - hisNeu[i].ac);
	clearNeurons(cmbNeu + hiddenSize, hiddenSize, 2);
	// back propagate error from the history representation to sentence top layer (the final representation of the sentence)
	matrixXvector(cmbNeu, hisNeu, compressSyn, hiddenSize + hiddenSize, 0, hiddenSize, hiddenSize, hiddenSize + hiddenSize, 1);
	// update compress matrix
//	if(wordCounter % 10 == 0)
//	{
		for(i = 0; i < hiddenSize; i ++)
			for(j = 0; j < N; j ++)
			{
				double grad = hisNeu[i].er * cmbNeu[j].ac;
				sumGradSquare.compressSyn_[i * N + j] += grad * grad;
				double move = alpha * grad * (sqrt(sumGradSquare.compressSyn_[i * N + j]) + adaGradEps);
				compressSyn[i * N + j].weight += move - compressSyn[i * N + j].weight * beta2;
			}
//	}
//	else
//	{
//		for(i = 0; i < hiddenSize; i ++)
//			for(j = 0; j < N; j ++)
//				compressSyn[i * N + j].weight += alpha * hisNeu[i].er * cmbNeu[j].ac;
//	}

	// error propagate in sentence model
	neuron **senNeu = senLen == 5 ? sen5Neu : sen7Neu;
	int SEN_HIGHT = senLen == 5 ? SEN5_HIGHT : SEN7_HIGHT;
	int unitNum = 1, unitNumNx = 1, a = -1, b = -1;
	for(i = 0; i < hiddenSize; i ++)
		senNeu[SEN_HIGHT - 1][i].er = cmbNeu[hiddenSize + i].er;
	for(i = SEN_HIGHT - 2; i >= 0; i --)
	{
		unitNumNx = unitNum + (conSeq[i] - 1);
		int offset = 0, offsetNx = 0, offsetCon = 0;
		// for readability, I just compute the deviation seperately
		for(a = 0; a < hiddenSize; a ++) for(b = 0; b < unitNum; b ++)
		{
			offset = a * unitNum;
			senNeu[i + 1][offset + b].er *= senNeu[i + 1][offset + b].ac * (1 - senNeu[i + 1][offset + b].ac);
		}

		// compute the back propagate error
		if(i != 0 || !fixSentenceModelFirstLayer)
		{
			for(a = 0; a < hiddenSize; a ++)
			{
				offset = a * unitNum;
				offsetNx = a * unitNumNx;
				offsetCon = a * conSeq[i];
				for(b = 0; b < unitNum; b ++)
				{
					for(j = 0; j < conSeq[i]; j ++)
						senNeu[i][offsetNx + b + j].er += senNeu[i + 1][offset + b].er * conSyn[i][offsetCon + j].weight;
				}
			}
			for(a = 0; a < hiddenSize; a ++)
			{
				offsetNx = a * unitNumNx;
				for(b = 0; b < unitNumNx; b ++)
				{
					//assert(senNeu[i][offsetNx + b].er >= -15 && senNeu[i][offsetNx + b].er <= 15);
					if(senNeu[i][offsetNx + b].er < -15) senNeu[i][offsetNx + b].er = -15;
					if(senNeu[i][offsetNx + b].er > 15) senNeu[i][offsetNx + b].er = 15;
				}
			}
		}

		// update the matrix, at this point we do NOT consider the L2 normalization term
		for(a = 0; a < hiddenSize; a ++)
		{
			offset = a * unitNum;
			offsetNx = a * unitNumNx;
			offsetCon = a * conSeq[i];
			for(j = 0; j < conSeq[i]; j ++)
				conSynOffset[i][offsetCon + j].weight = 0;
			for(b = 0; b < unitNum; b ++)
			{
				for(j = 0; j < conSeq[i]; j ++)
					conSynOffset[i][offsetCon + j].weight += alpha * senNeu[i + 1][offset + b].er * senNeu[i][offsetNx + b + j].ac;
			}
		}
		// update the matrix with L2 normalization term
		for(a = 0; a < hiddenSize; a ++)
		{
			offsetCon = a * conSeq[i];
			for(j = 0; j < conSeq[i]; j ++)
			{
				double grad = conSynOffset[i][offsetCon + j].weight / alpha;
				sumGradSquare.conSyn_[i][offsetCon + j] += grad * grad;
				double move = alpha * grad / (sqrt(sumGradSquare.conSyn_[i][offsetCon + j]) + adaGradEps);
				conSyn[i][offsetCon + j].weight += move - beta2 * conSyn[i][offsetCon + j].weight;
			}
		}

		unitNum = unitNumNx;
	}
	// cout << unitNumNx << endl;
	assert(unitNumNx == senLen);

	if(fixSentenceModelFirstLayer)
		return;
	int V = vocab.getVocabSize();
	int newWordCounter = wordCounter - senLen - 1;
	for(i = 0; i < unitNumNx; i ++)
	{
		int word = bpttHistory[i + 1];	// because bpttHistory recorded lastWord, not curWord
		newWordCounter ++;
		for(j = 0; j < hiddenSize; j ++)
		{
			senNeu[0][j*unitNumNx + i].er *= senNeu[0][j*unitNumNx + i].ac * (1 - senNeu[0][j*unitNumNx + i].ac);
			double grad = senNeu[0][j*unitNumNx + i].er;
			sumGradSquare.senweSyn_[V*j + word] += grad * grad;
			double move = alpha * grad / (sqrt(sumGradSquare.senweSyn_[V*j + word]) + adaGradEps);
			if(newWordCounter % 10 == 0)
				senweSyn[V*j + word].weight += move - beta2 * senweSyn[V*j + word].weight;
			else
				senweSyn[V*j + word].weight += move;
		}

//		if(newWordCounter % 10 == 0)
//		{
//			for(j = 0; j < hiddenSize; j ++)
//			{
//				senNeu[0][j*unitNumNx + i].er *= senNeu[0][j*unitNumNx + i].ac * (1 - senNeu[0][j*unitNumNx + i].ac);
//				senweSyn[V*j + word].weight += alpha * senNeu[0][j*unitNumNx + i].er - beta2 * senweSyn[V*j + word].weight;
//			}
//		}
//		else
//		{
//			for(j = 0; j < hiddenSize; j ++)
//			{
//				senNeu[0][j*unitNumNx + i].er *= senNeu[0][j*unitNumNx + i].ac * (1 - senNeu[0][j*unitNumNx + i].ac);
//				senweSyn[V*j + word].weight += alpha * senNeu[0][j*unitNumNx + i].er;
//			}
//		}
	}
}

void RNNPG::learnNetAdaGrad(int lastWord, int curWord, int wordPos, int senLen)
{
	double beta2 = alpha * beta;
	int curClassIndex = voc_arr[curWord].classIndex, i = 0, j = 0, V = vocab.getVocabSize(), N = 0, offset = 0;
	// error at output layer. 1. error on words
	for(i = classStart[curClassIndex]; i < classEnd[curClassIndex]; i ++)
		outNeu[i].er = 0 - outNeu[i].ac;
	outNeu[curWord].er = 1 - outNeu[curWord].ac;
	// error at output layer. 2. error on classes
	N = V + classSize;
	for(i = V; i < N; i ++)
		outNeu[i].er = 0 - outNeu[i].ac;
	outNeu[V + curClassIndex].er = 1 - outNeu[V + curClassIndex].ac;

	clearNeurons(hiddenNeu, hiddenSize, 2);

	// error backpropagation to hidden layer
	// 1. error from words to hidden
	matrixXvector(hiddenNeu, outNeu, outHiddenSyn, hiddenSize, classStart[curClassIndex], classEnd[curClassIndex], 0, hiddenSize, 1);
	// 2. error from classes to hidden
	matrixXvector(hiddenNeu, outNeu, outHiddenSyn, hiddenSize, V, V + classSize, 0, hiddenSize, 1);

	if(directError)
	{
		// bufOutConditionNeu
		clearNeurons(bufOutConditionNeu + (wordPos * hiddenSize), hiddenSize, 2);
		// error back propagate to conditionNeu
		// 1. error from words to conditionNeu
		matrixXvector(bufOutConditionNeu + (wordPos * hiddenSize), outNeu, outConditionDSyn, hiddenSize, classStart[curClassIndex], classEnd[curClassIndex], 0, hiddenSize, 1);
		// 2. error from classes to conditionNeu
		matrixXvector(bufOutConditionNeu + (wordPos * hiddenSize), outNeu, outConditionDSyn, hiddenSize, V, V + classSize, 0, hiddenSize, 1);
	}

	// updating the matrix outHiddenSyn, since we already have the error at output layer and the activation in the hidden layer
	// we update the weight per word rather than per sentence for faster increase in likelihood. Perhaps it will be modified to per sentence update later
	// update submatrix of words to hidden layer
	offset = classStart[curClassIndex] * hiddenSize;
	for(i = classStart[curClassIndex]; i < classEnd[curClassIndex]; i ++)
	{
		for(j = 0; j < hiddenSize; j ++)
		{
			double grad = outNeu[i].er * hiddenNeu[j].ac;
			sumGradSquare.outHiddenSyn_[offset + j] += grad * grad;
			double move = alpha * grad / (sqrt(sumGradSquare.outHiddenSyn_[offset + j]) + adaGradEps);

			if(wordCounter % 10 == 0)
				outHiddenSyn[offset + j].weight += move - beta2*outHiddenSyn[offset + j].weight;
			else
				outHiddenSyn[offset + j].weight += move;
		}
//		if(wordCounter % 10 == 0)
//			for(j = 0; j < hiddenSize; j ++) outHiddenSyn[offset + j].weight += alpha * outNeu[i].er * hiddenNeu[j].ac - beta2*outHiddenSyn[offset + j].weight;
//		else
//			for(j = 0; j < hiddenSize; j ++) outHiddenSyn[offset + j].weight += alpha * outNeu[i].er * hiddenNeu[j].ac;
		offset += hiddenSize;
	}
	// update submatrix of classes to hidden layer
	N = V + classSize;
	offset = V * hiddenSize;
	for(i = V; i < N; i ++)
	{
		for(j = 0; j < hiddenSize; j ++)
		{
			double grad = outNeu[i].er * hiddenNeu[j].ac;
			sumGradSquare.outHiddenSyn_[offset + j] += grad * grad;
			double move = alpha * grad / (sqrt(sumGradSquare.outHiddenSyn_[offset + j]) + adaGradEps);

			if(wordCounter % 10 == 0)
				outHiddenSyn[offset + j].weight += move - beta2*outHiddenSyn[offset + j].weight;
			else
				outHiddenSyn[offset + j].weight += move;
		}
//		if(wordCounter % 10 == 0)
//			for(j = 0; j < hiddenSize; j ++) outHiddenSyn[offset + j].weight += alpha * outNeu[i].er * hiddenNeu[j].ac - beta2*outHiddenSyn[offset + j].weight;
//		else
//			for(j = 0; j < hiddenSize; j ++) outHiddenSyn[offset + j].weight += alpha * outNeu[i].er * hiddenNeu[j].ac;
		offset += hiddenSize;
	}

	if(directError)
	{
		// update the matrix outConditionDSyn
		// update submatrix of words to condition layer
		offset = classStart[curClassIndex] * hiddenSize;
		for(i = classStart[curClassIndex]; i < classEnd[curClassIndex]; i ++)
		{
			// how can I make such stupid bug !!! hiddenNeu[V + j].ac is obviously incorrect !!! inNeu[V + j].ac
			if(wordCounter % 10 == 0)
				for(j = 0; j < hiddenSize; j ++) outConditionDSyn[offset + j].weight += alpha * outNeu[i].er * inNeu[V + j].ac - beta2*outConditionDSyn[offset + j].weight;
			else
				for(j = 0; j < hiddenSize; j ++) outConditionDSyn[offset + j].weight += alpha * outNeu[i].er * inNeu[V + j].ac;
			offset += hiddenSize;
		}

		// update submatrix of classes to condition layer
		N = V + classSize;
		offset = V * hiddenSize;
		for(i = V; i < N; i ++)
		{
			// how can I make such stupid bug !!! hiddenNeu[V + j].ac is obviously incorrect !!! inNeu[V + j].ac
			if(wordCounter % 10 == 0)
				for(j = 0; j < hiddenSize; j ++) outConditionDSyn[offset + j].weight += alpha * outNeu[i].er * inNeu[V + j].ac - beta2*outConditionDSyn[offset + j].weight;
			else
				for(j = 0; j < hiddenSize; j ++) outConditionDSyn[offset + j].weight += alpha * outNeu[i].er * inNeu[V + j].ac;
			offset += hiddenSize;
		}
	}

	// this is for back propagation through time
	bpttHistory[wordPos] = lastWord;	// store the last word
	memcpy(bpttHiddenNeu + (wordPos * hiddenSize), hiddenNeu, sizeof(neuron)*hiddenSize);	// store the hidden layer
	memcpy(bpttInHiddenNeu + (wordPos * hiddenSize), inNeu + (V + hiddenSize), sizeof(neuron)*hiddenSize);	// store the hidden units in input layer (previous hidden layer)
	memcpy(bpttConditionNeu + (wordPos * hiddenSize), inNeu + V, sizeof(neuron)*hiddenSize);	// store the condition units in input layer
	if(curWord != 0)
		return;
	// if this is the end of sentence, then let's do it
	int lword = -1, layer1Size = V + hiddenSize + hiddenSize;
	synapse **mapSyn = NULL;
	mapSyn = senLen == 5 ? map5Syn : map7Syn;
	double **mapSyn_ = senLen == 5 ? sumGradSquare.map5Syn_ : sumGradSquare.map7Syn_;
	for(int step = wordPos; step >= 0; step --)
	{
		// take care of vocabulary and recurrent part in input layer first
		// bpttHiddenInSyn[]
		for(i = 0; i < hiddenSize; i ++)
			hiddenNeu[i].er *= hiddenNeu[i].ac * (1 - hiddenNeu[i].ac);
		lword = bpttHistory[step];
		// accumulate deviations for input matrix, X
		for(i = 0; i < hiddenSize; i ++)
			bpttHiddenInSyn[i * layer1Size + lword].weight += alpha * hiddenNeu[i].er;

		clearNeurons(inNeu + (V+hiddenSize), hiddenSize, 2);
		matrixXvector(inNeu, hiddenNeu, hiddenInSyn, layer1Size, 0, hiddenSize, V + hiddenSize, layer1Size, 1);
		// accumulate deviations for hidden matrix, R
		for(i = 0; i < hiddenSize; i ++)
			for(j = V + hiddenSize; j < layer1Size; j ++)
				bpttHiddenInSyn[i*layer1Size + j].weight += alpha * hiddenNeu[i].er * inNeu[j].ac;

		// now we take care the condition part in the input layer
		// back propagate the error to condition part
		clearNeurons(inNeu + V, hiddenSize, 2);
		matrixXvector(inNeu, hiddenNeu, hiddenInSyn, layer1Size, 0, hiddenSize, V, V + hiddenSize, 1);
		// accumulate deviations for condition matrix, H
		N = V + hiddenSize;
		for(i = 0; i < hiddenSize; i ++)
			for(j = V; j < N; j ++)
				bpttHiddenInSyn[i*layer1Size + j].weight += alpha * hiddenNeu[i].er * inNeu[j].ac;

		if(directError)
		{
			for(i = 0; i < hiddenSize; i ++)
				inNeu[V + i].er += bufOutConditionNeu[step * hiddenSize + i].er;
		}

		for(i = V; i < N; i ++)
			inNeu[i].er *= inNeu[i].ac * (1 - inNeu[i].ac);

		if(perSentUpdate)
			clearNeurons(hisNeu, hiddenSize, 2);

		// watch that the error in hisNeu must be inilizated to zero at the beginning of dealing with each sentence
		matrixXvector(hisNeu, inNeu + V, mapSyn[step], hiddenSize, 0, hiddenSize, 0, hiddenSize, 1);

		// acumulate deviations for map matrix
		for(i = 0; i < hiddenSize; i ++)
			for(j = 0; j < hiddenSize; j ++)
			{
				double grad = inNeu[V+i].er * hisNeu[j].ac;
				mapSyn_[step][i*hiddenSize + j] += grad * grad;
				double move = alpha * grad / (sqrt(mapSyn_[step][i*hiddenSize + j]) + adaGradEps);
				if(wordCounter % 10 == 0)
					mapSyn[step][i * hiddenSize + j].weight += move - mapSyn[step][i * hiddenSize + j].weight * beta2;
				else
					mapSyn[step][i * hiddenSize + j].weight += move;
			}
//		if(wordCounter % 10 == 0)
//		{
//			for(i = 0; i < hiddenSize; i ++)
//				for(j = 0; j < hiddenSize; j ++)
//					mapSyn[step][i * hiddenSize + j].weight += alpha * inNeu[V+i].er * hisNeu[j].ac - mapSyn[step][i * hiddenSize + j].weight * beta2;
//		}
//		else
//		{
//			for(i = 0; i < hiddenSize; i ++)
//				for(j = 0; j < hiddenSize; j ++)
//					mapSyn[step][i * hiddenSize + j].weight += alpha * inNeu[V+i].er * hisNeu[j].ac;
//		}

		if(perSentUpdate)
			learnSent(senLen);

		if(step == 0) continue;
		// propagate error to previous layer
		for(i = 0; i < hiddenSize; i ++)
		{
			hiddenNeu[i].er = inNeu[V + hiddenSize + i].er + bpttHiddenNeu[(step - 1)*hiddenSize + i].er;
			hiddenNeu[i].ac = bpttHiddenNeu[(step - 1)*hiddenSize + i].ac;
		// restore the recurrent part in input layer
			inNeu[V + hiddenSize + i].ac = bpttInHiddenNeu[(step - 1)*hiddenSize + i].ac;
		// restore the condition part in input layer
			inNeu[V + i].ac = bpttConditionNeu[(step - 1)*hiddenSize + i].ac;
		}
	}

	// restore hidden layer
	memcpy(hiddenNeu, bpttHiddenNeu + (wordPos * hiddenSize), sizeof(neuron)*hiddenSize);


	// update input matrix, X, condition Matrix, H and recurrent matrix, R
	for(i = 0; i < hiddenSize; i ++)
	{
//		if(wordCounter % 10 == 0)
//		{
			for(j = 0; j <= wordPos; j ++)
			{
				lword = bpttHistory[j];
				double grad = bpttHiddenInSyn[i*layer1Size + lword].weight / alpha;
				sumGradSquare.hiddenInSyn_[i*layer1Size + lword] += grad * grad;
				double move = alpha * grad / (sqrt(sumGradSquare.hiddenInSyn_[i*layer1Size + lword]) + adaGradEps);
				hiddenInSyn[i*layer1Size + lword].weight += move - hiddenInSyn[i*layer1Size + lword].weight * beta2;
				bpttHiddenInSyn[i*layer1Size + lword].weight = 0;
			}
//		}
//		else
//		{
//			for(j = 0; j <= wordPos; j ++)
//			{
//				lword = bpttHistory[j];
//				hiddenInSyn[i*layer1Size + lword].weight += bpttHiddenInSyn[i * layer1Size + lword].weight;
//				bpttHiddenInSyn[i * layer1Size + lword].weight = 0;
//			}
//		}

//		if(wordCounter % 10 == 0)
//		{
			N = V + hiddenSize;
			for(j = V; j < N; j ++)
			{
				double grad = bpttHiddenInSyn[i*layer1Size + j].weight / alpha;
				sumGradSquare.hiddenInSyn_[i*layer1Size + j] += grad * grad;
				double move = alpha * grad / (sqrt(sumGradSquare.hiddenInSyn_[i*layer1Size + j]) + adaGradEps);
				hiddenInSyn[i*layer1Size + j].weight += move - hiddenInSyn[i*layer1Size + j].weight * beta2;
				bpttHiddenInSyn[i*layer1Size + j].weight = 0;
			}
//		}
//		else
//		{
//			N = V + hiddenSize;
//			for(j = V; j < N; j ++)
//			{
//				hiddenInSyn[i*layer1Size + j].weight += bpttHiddenInSyn[i*layer1Size + j].weight;
//				bpttHiddenInSyn[i*layer1Size + j].weight = 0;
//			}
//		}

//		if(wordCounter % 10 == 0)
//		{
			for(j = V + hiddenSize; j < layer1Size; j ++)
			{
				double grad = bpttHiddenInSyn[i*layer1Size + j].weight / alpha;
				sumGradSquare.hiddenInSyn_[i*layer1Size + j] += grad * grad;
				double move = alpha * grad / (sqrt(sumGradSquare.hiddenInSyn_[i*layer1Size + j]) + adaGradEps);
				hiddenInSyn[i*layer1Size + j].weight += move - hiddenInSyn[i*layer1Size + j].weight * beta2;
				bpttHiddenInSyn[i*layer1Size + j].weight = 0;
			}
//		}
//		else
//		{
//			for(j = V + hiddenSize; j < layer1Size; j ++)
//			{
//				hiddenInSyn[i*layer1Size + j].weight += bpttHiddenInSyn[i*layer1Size + j].weight;
//				bpttHiddenInSyn[i*layer1Size + j].weight = 0;
//			}
//		}
	}

//	if(!perSentUpdate)
//	{
//		// update parameters in sentences
//		if(!conbptt)
//			learnSent(senLen);
//		else
//			learnSentBPTT(senLen);
//	}
	learnSentAdaGrad(senLen);
}

void RNNPG::copyHiddenLayerToInput()
{
	int offset = vocab.getVocabSize() + hiddenSize;
	for(int i = 0; i < hiddenSize; i ++)
		inNeu[offset + i].ac = hiddenNeu[i].ac;
}

void RNNPG::trainPoem(const vector<string> &sentences)
{
	int SEN_NUM = sentences.size();
	vector<string> words;
	int i, SEN_HIGHT = -1;
	neuron **senNeu = NULL;
	words.clear();
	split(sentences[0].c_str(), " ", words);
	SEN_HIGHT = words.size() == 5 ? SEN5_HIGHT : SEN7_HIGHT;
	senNeu = words.size() == 5 ? sen5Neu : sen7Neu;

	// this is for the first sentence
	initSent(words.size());
	neuron *sen_repr = sen2vec(words, senNeu, SEN_HIGHT);		// this is the pointer for the top layer sentence model, DO NOT modify it

	// for first sentence, we can just give the representation to the generation model, or
	clearNeurons(cmbNeu, hiddenSize * 2, 3);		// this is probably a bug!!! change 1 to 3, also flush the error

	// init activation in recurrent context model
	for(i = 0; i < hiddenSize; i ++)
		cmbNeu[i].ac = historyStableAC;
	memcpy(cmbNeu + hiddenSize, sen_repr, sizeof(neuron)*hiddenSize);
	clearNeurons(hisNeu, hiddenSize, 3);
	matrixXvector(hisNeu, cmbNeu, compressSyn, hiddenSize * 2, 0, hiddenSize, 0, hiddenSize * 2, 0);
	funACNeurons(hisNeu, hiddenSize);

	// alternivate
	// memcpy(hisNeu, sen_repr, sizeof(neuron)*hiddenSize);

	synapse **mapSyn = words.size() == 5 ? map5Syn : map7Syn;
	// this is for the subsequence sentences (generation and compress the representation)
	for(i = 1; i < SEN_NUM; i ++)
	{
		isLastSentOfPoem = i == SEN_NUM - 1;
		contextBPTTSentNum = i;
		words.clear();
		split(sentences[i].c_str(), " ", words);
		// just for test...
		// printNeurons(hisNeu, hiddenSize);

		// generation ...
		// initSent(words.size());
		if(flushOption == EVERY_SENTENCE)
			flushNet();		// clear input hidden and output layer
		else if(flushOption == EVERY_POEM)
		{
			if(i == 1)
				flushNet();
		}

		words.push_back("</s>");	// during generation, we DO care about the End-of-Sentence
		int lastWord = 0, curWord = -1, wdPos;
		for(wdPos = 0; wdPos < (int)words.size(); wdPos ++)
		{
			wordCounter ++;
			curWord = vocab.getVocabID(words[wdPos].c_str());
			if(curWord == -1)
				cout << "unseen word " << "'" << words[wdPos] << "'" << endl;
			assert(curWord != -1);		// this is impossible, or there is a bug!
			inNeu[lastWord].ac = 1;
			computeNet(lastWord, curWord, wdPos, mapSyn);
			// perhaps I also need to caculate the log-likelihood
			logp+=log10(outNeu[voc_arr[curWord].classIndex+vocab.getVocabSize()].ac * outNeu[curWord].ac);
			// learnNet, tomorrow come back to the sentence model
			if(!adaGrad)
				learnNet(lastWord, curWord, wdPos, words.size() - 1);
			else
				learnNetAdaGrad(lastWord, curWord, wdPos, words.size() - 1);
			inNeu[lastWord].ac = 0;
			copyHiddenLayerToInput();
			lastWord = curWord;
		}
		words.pop_back();	// generation done and delete </s>

		// compress representation
		if(i == SEN_NUM - 1)
			break;
		initSent(words.size());
		sen_repr = sen2vec(words, senNeu, SEN_HIGHT);
		memcpy(cmbNeu, hisNeu, sizeof(neuron)*hiddenSize);
		memcpy(cmbNeu + hiddenSize, sen_repr, sizeof(neuron)*hiddenSize);
		clearNeurons(hisNeu, hiddenSize, 3);
		matrixXvector(hisNeu, cmbNeu, compressSyn, hiddenSize * 2, 0, hiddenSize, 0, hiddenSize * 2, 0);
		funACNeurons(hisNeu, hiddenSize);
	}
}

void RNNPG::testPoem(const vector<string> &sentences)
{
	const int SEN_NUM = 4;
	vector<string> words;
	int i, SEN_HIGHT = -1;
	neuron **senNeu = NULL;
	words.clear();
	split(sentences[0].c_str(), " ", words);
	SEN_HIGHT = words.size() == 5 ? SEN5_HIGHT : SEN7_HIGHT;
	senNeu = words.size() == 5 ? sen5Neu : sen7Neu;

	// this is for the first sentence
	initSent(words.size());
	neuron *sen_repr = sen2vec(words, senNeu, SEN_HIGHT);		// this is the pointer for the top layer sentence model, DO NOT modify it

	// for first sentence, we can just give the representation to the generation model, or
	clearNeurons(cmbNeu, hiddenSize * 2, 3);		// this is probably a bug!!! change 1 to 3, also flush the error
	memcpy(cmbNeu + hiddenSize, sen_repr, sizeof(neuron)*hiddenSize);
	clearNeurons(hisNeu, hiddenSize, 3);
	matrixXvector(hisNeu, cmbNeu, compressSyn, hiddenSize * 2, 0, hiddenSize, 0, hiddenSize * 2, 0);
	funACNeurons(hisNeu, hiddenSize);

	// alternivate
	// memcpy(hisNeu, sen_repr, sizeof(neuron)*hiddenSize);

	synapse **mapSyn = words.size() == 5 ? map5Syn : map7Syn;
	// this is for the subsequence sentences (generation and compress the representation)
	for(i = 1; i < SEN_NUM; i ++)
	{
		words.clear();
		split(sentences[i].c_str(), " ", words);
		// just for test...
		// printNeurons(hisNeu, hiddenSize);

		// generation ...
		// initSent(words.size());
		// flushNet();		// clear input hidden and output layer
		if(flushOption == EVERY_SENTENCE)
			flushNet();		// clear input hidden and output layer
		else if(flushOption == EVERY_POEM)
		{
			if(i == 1)
				flushNet();
		}
		words.push_back("</s>");	// during generation, we DO care about the End-of-Sentence
		int lastWord = 0, curWord = -1, wdPos;
		for(wdPos = 0; wdPos < (int)words.size(); wdPos ++)
		{
			wordCounter ++;
			curWord = vocab.getVocabID(words[wdPos].c_str());
			bool isRare = false;
			if(curWord == -1)
			{
				// when the word cannot be found, we use <R> instead
				curWord = vocab.getVocabID("<R>");
				isRare = true;
			}
			assert(curWord != -1);		// this is impossible, or there is a bug!
			inNeu[lastWord].ac = 1;
			computeNet(lastWord, curWord, wdPos, mapSyn);
			// perhaps I also need to caculate the log-likelihood
			if(!isRare)
				logp += log10(outNeu[voc_arr[curWord].classIndex+vocab.getVocabSize()].ac * outNeu[curWord].ac);
			else
				logp += log10(outNeu[voc_arr[curWord].classIndex+vocab.getVocabSize()].ac * outNeu[curWord].ac / voc_arr[curWord].freq);
			// learnNet, tomorrow come back to the sentence model
			// learnNet(lastWord, curWord, wdPos, words.size() - 1);
			inNeu[lastWord].ac = 0;
			copyHiddenLayerToInput();
			lastWord = curWord;
		}
		words.pop_back();	// generation done and delete </s>

		// compress representation
		if(i == SEN_NUM - 1)
			break;
		initSent(words.size());
		sen_repr = sen2vec(words, senNeu, SEN_HIGHT);
		memcpy(cmbNeu, hisNeu, sizeof(neuron)*hiddenSize);
		memcpy(cmbNeu + hiddenSize, sen_repr, sizeof(neuron)*hiddenSize);
		clearNeurons(hisNeu, hiddenSize, 3);
		matrixXvector(hisNeu, cmbNeu, compressSyn, hiddenSize * 2, 0, hiddenSize, 0, hiddenSize * 2, 0);
		funACNeurons(hisNeu, hiddenSize);
	}
}

void RNNPG::initBackup()
{
	int i = -1, M = -1, N = -1, unitNum;
	M = MAX_CON_N;
	for(i = 0; i < M; i ++)
	{
		N = hiddenSize * conSeq[i];
		conSyn_backup[i] = (synapse*)xmalloc(N*sizeof(synapse));
	}
	for(i = 0; i < M; i ++)
	{
		N = hiddenSize * conSeq[i];
		conSynOffset_backup[i] = (synapse*)xmalloc(N*sizeof(synapse));
	}
	M = SEN7_HIGHT;
	unitNum = 7;
	for(i = 0; i < M; i ++)
	{
		N = hiddenSize * unitNum;
		sen7Neu_backup[i] = (neuron*)xmalloc(N * sizeof(neuron));
		if(unitNum > 1)	unitNum -= conSeq[i] - 1;
	}
	M = SEN5_HIGHT;
	unitNum = 5;
	for(i = 0; i < M; i ++)
	{
		N = hiddenSize * unitNum;
		sen5Neu_backup[i] = (neuron*)xmalloc(N * sizeof(neuron));
		if(unitNum > 1)	unitNum -= conSeq[i] - 1;
	}
	M = hiddenSize * hiddenSize * 2;
	compressSyn_backup = (synapse*)xmalloc(M * sizeof(synapse));
	M = hiddenSize;
	hisNeu_backup = (neuron*)xmalloc(M * sizeof(neuron));
	M = hiddenSize * 2;
	cmbNeu_backup = (neuron*)xmalloc(M * sizeof(neuron));
	M = 8; N = hiddenSize * hiddenSize;
	for(i = 0; i < M; i ++)
	{
		map7Syn_backup[i] = (synapse*)xmalloc(N * sizeof(synapse));
		// map7Syn_backup[i][j].weight = map7Syn[i][j].weight;
	}
	M = 6; N = hiddenSize * hiddenSize;
	for(i = 0; i < M; i ++)
	{
		map5Syn_backup[i] = (synapse*)xmalloc(N * sizeof(synapse));
		// map5Syn_backup[i][j].weight = map5Syn[i][j].weight;
	}
	M = hiddenSize;
	conditionNeu_backup = (neuron*)xmalloc(M * sizeof(neuron));
	M = vocab.getVocabSize() * hiddenSize;
	senweSyn_backup = (synapse*)xmalloc(M * sizeof(synapse));
	M = vocab.getVocabSize() + hiddenSize + hiddenSize;
	inNeu_backup = (neuron*)xmalloc(M * sizeof(neuron));
	M = hiddenSize;
	hiddenNeu_backup = (neuron*)xmalloc(M * sizeof(neuron));
	M = vocab.getVocabSize() + classSize;
	outNeu_backup = (neuron*)xmalloc(M * sizeof(neuron));
	M = hiddenSize * (vocab.getVocabSize() + hiddenSize + hiddenSize);
	hiddenInSyn_backup = (synapse*)xmalloc(M * sizeof(synapse));
	M = (vocab.getVocabSize() + classSize) * hiddenSize;
	outHiddenSyn_backup = (synapse*)xmalloc(M * sizeof(synapse));

	M = ((vocab.getVocabSize() + classSize) * hiddenSize);
	outConditionDSyn_backup = (synapse*)xmalloc(M * sizeof(synapse));
}

void RNNPG::saveWeights()
{
	int i = -1, j = -1, M = -1, N = -1, unitNum;
	M = MAX_CON_N;
	for(i = 0; i < M; i ++)
	{
		N = hiddenSize * conSeq[i];
		for(j = 0; j < N; j ++)
			conSyn_backup[i][j].weight = conSyn[i][j].weight;
	}
	for(i = 0; i < M; i ++)
	{
		N = hiddenSize * conSeq[i];
		for(j = 0; j < N; j ++)
		{
			conSynOffset_backup[i][j].weight = conSynOffset[i][j].weight;
		}
	}
	M = SEN7_HIGHT;
	unitNum = 7;
	for(i = 0; i < M; i ++)
	{
		N = hiddenSize * unitNum;
		for(j = 0; j < N; j ++)
		{
			sen7Neu_backup[i][j].ac = sen7Neu[i][j].ac;
			sen7Neu_backup[i][j].er = sen7Neu[i][j].er;
		}
		if(unitNum > 1)	unitNum -= conSeq[i] - 1;
	}
	M = SEN5_HIGHT;
	unitNum = 5;
	for(i = 0; i < M; i ++)
	{
		N = hiddenSize * unitNum;
		for(j = 0; j < N; j ++)
		{
			sen5Neu_backup[i][j].ac = sen5Neu[i][j].ac;
			sen5Neu_backup[i][j].er = sen5Neu[i][j].er;
		}
		if(unitNum > 1)	unitNum -= conSeq[i] - 1;
	}
	M = hiddenSize * hiddenSize * 2;
	for(i = 0; i < M; i ++)
	{
		compressSyn_backup[i].weight = compressSyn[i].weight;
	}
	M = hiddenSize;
	for(i = 0; i < M; i ++)
	{
		hisNeu_backup[i].ac = hisNeu[i].ac;
		hisNeu_backup[i].er = hisNeu[i].er;
	}
	M = hiddenSize * 2;
	for(i = 0; i < M; i ++)
	{
		cmbNeu_backup[i].ac = cmbNeu[i].ac;
		cmbNeu_backup[i].er = cmbNeu[i].er;
	}
	M = 8; N = hiddenSize * hiddenSize;
	for(i = 0; i < M; i ++) for(j = 0; j < N; j ++)
	{
		map7Syn_backup[i][j].weight = map7Syn[i][j].weight;
	}
	M = 6; N = hiddenSize * hiddenSize;
	for(i = 0; i < M; i ++) for(j = 0; j < N; j ++)
	{
		map5Syn_backup[i][j].weight = map5Syn[i][j].weight;
	}
	M = hiddenSize;
	for(i = 0; i < M; i ++)
	{
		conditionNeu_backup[i].ac = conditionNeu[i].ac;
		conditionNeu_backup[i].er = conditionNeu[i].er;
	}
	M = vocab.getVocabSize() * hiddenSize;
	for(i = 0; i < M; i ++)
	{
		senweSyn_backup[i].weight = senweSyn[i].weight;
	}
	M = vocab.getVocabSize() + hiddenSize + hiddenSize;
	for(i = 0; i < M; i ++)
	{
		inNeu_backup[i].ac = inNeu[i].ac;
		inNeu_backup[i].er = inNeu[i].er;
	}
	M = hiddenSize;
	for(i = 0; i < M; i ++)
	{
		hiddenNeu_backup[i].ac = hiddenNeu[i].ac;
		hiddenNeu_backup[i].er = hiddenNeu[i].er;
	}
	M = vocab.getVocabSize() + classSize;
	for(i = 0; i < M; i ++)
	{
		outNeu_backup[i].ac = outNeu[i].ac;
		outNeu_backup[i].er = outNeu[i].er;
	}
	M = hiddenSize * (vocab.getVocabSize() + hiddenSize + hiddenSize);
	for(i = 0; i < M; i ++)
	{
		hiddenInSyn_backup[i].weight = hiddenInSyn[i].weight;
	}
	M = (vocab.getVocabSize() + classSize) * hiddenSize;
	for(i = 0; i < M; i ++)
	{
		outHiddenSyn_backup[i].weight = outHiddenSyn[i].weight;
	}
	M = (vocab.getVocabSize() + classSize) * hiddenSize;
	for(i = 0; i < M; i ++)
		outConditionDSyn_backup[i].weight = outConditionDSyn[i].weight;
}

void RNNPG::restoreWeights()
{
	int i = -1, j = -1, M = -1, N = -1, unitNum;
	M = MAX_CON_N;
	for(i = 0; i < M; i ++)
	{
		N = hiddenSize * conSeq[i];
		for(j = 0; j < N; j ++)
			conSyn[i][j].weight = conSyn_backup[i][j].weight;
	}
	for(i = 0; i < M; i ++)
	{
		N = hiddenSize * conSeq[i];
		for(j = 0; j < N; j ++)
		{
			conSynOffset[i][j].weight = conSynOffset_backup[i][j].weight;
		}
	}
	M = SEN7_HIGHT;
	unitNum = 7;
	for(i = 0; i < M; i ++)
	{
		N = hiddenSize * unitNum;
		for(j = 0; j < N; j ++)
		{
			sen7Neu[i][j].ac = sen7Neu_backup[i][j].ac;
			sen7Neu[i][j].er = sen7Neu_backup[i][j].er;
		}
		if(unitNum > 1)	unitNum -= conSeq[i] - 1;
	}
	M = SEN5_HIGHT;
	unitNum = 5;
	for(i = 0; i < M; i ++)
	{
		N = hiddenSize * unitNum;
		for(j = 0; j < N; j ++)
		{
			sen5Neu[i][j].ac = sen5Neu_backup[i][j].ac;
			sen5Neu[i][j].er = sen5Neu_backup[i][j].er;
		}
		if(unitNum > 1)	unitNum -= conSeq[i] - 1;
	}
	M = hiddenSize * hiddenSize * 2;
	for(i = 0; i < M; i ++)
	{
		compressSyn[i].weight = compressSyn_backup[i].weight;
	}
	M = hiddenSize;
	for(i = 0; i < M; i ++)
	{
		hisNeu[i].ac = hisNeu_backup[i].ac;
		hisNeu[i].er = hisNeu_backup[i].er;
	}
	M = hiddenSize * 2;
	for(i = 0; i < M; i ++)
	{
		cmbNeu[i].ac = cmbNeu_backup[i].ac;
		cmbNeu[i].er = cmbNeu_backup[i].er;
	}
	M = 8; N = hiddenSize * hiddenSize;
	for(i = 0; i < M; i ++) for(j = 0; j < N; j ++)
	{
		map7Syn[i][j].weight = map7Syn_backup[i][j].weight;
	}
	M = 6; N = hiddenSize * hiddenSize;
	for(i = 0; i < M; i ++) for(j = 0; j < N; j ++)
	{
		map5Syn[i][j].weight = map5Syn_backup[i][j].weight;
	}
	M = hiddenSize;
	for(i = 0; i < M; i ++)
	{
		conditionNeu[i].ac = conditionNeu_backup[i].ac;
		conditionNeu[i].er = conditionNeu_backup[i].er;
	}
	M = vocab.getVocabSize() * hiddenSize;
	for(i = 0; i < M; i ++)
	{
		senweSyn[i].weight = senweSyn_backup[i].weight;
	}
	M = vocab.getVocabSize() + hiddenSize + hiddenSize;
	for(i = 0; i < M; i ++)
	{
		inNeu[i].ac = inNeu_backup[i].ac;
		inNeu[i].er = inNeu_backup[i].er;
	}
	M = hiddenSize;
	for(i = 0; i < M; i ++)
	{
		hiddenNeu[i].ac = hiddenNeu_backup[i].ac;
		hiddenNeu[i].er = hiddenNeu_backup[i].er;
	}
	M = vocab.getVocabSize() + classSize;
	for(i = 0; i < M; i ++)
	{
		outNeu[i].ac = outNeu_backup[i].ac;
		outNeu[i].er = outNeu_backup[i].er;
	}
	M = hiddenSize * (vocab.getVocabSize() + hiddenSize + hiddenSize);
	for(i = 0; i < M; i ++)
	{
		hiddenInSyn[i].weight = hiddenInSyn_backup[i].weight;
	}
	M = (vocab.getVocabSize() + classSize) * hiddenSize;
	for(i = 0; i < M; i ++)
	{
		outHiddenSyn[i].weight = outHiddenSyn_backup[i].weight;
	}
	M = (vocab.getVocabSize() + classSize) * hiddenSize;
	for(i = 0; i < M; i ++)
		outConditionDSyn[i].weight = outConditionDSyn_backup[i].weight;
}

void RNNPG::testNetFile(const char *testF)
{
	FILE *fin = xfopen(testF, "r", "computeNet -- open valid/testFile");
	char buf[1024];
	vector<string> sentences;
	int SEN_NUM = 4;
	flushNet();
	while(fgets(buf, sizeof(buf), fin))
	{
		sentences.clear();
		split(buf, "\t\r\n", sentences);
		if((int)sentences.size() != SEN_NUM) // here is just for quatrain
		{
			fprintf(stderr, "This is NOT a quatrain!!!\n");
			continue;
		}
		testPoem(sentences);
	}
	fclose(fin);
}

void RNNPG::trainNet()
{
	mode = TRAIN_MODE;

	loadVocab(trainFile);
	initNet();
	showParameters();

	double oriAlpha = alpha;

	char buf[1024];		// for poems this is enough
	vector<string> sentences;
	const int QUATRAIN_SEN_NUM = 4, LVSHI_SEN_NUM = 8;
	double lastLogp = -1e18;
	int iter;
	for(iter = 0; iter < maxIter; iter ++)
	{
		if(adaGrad)
			sumGradSquare.reset(this);
		logp = 0;
		wordCounter = 0;
		FILE *fin = xfopen(trainFile, "r", "computeNet -- open trainFile");
		int poem_cnt = 0;
		int sen_cnt = 0;
		flushNet();		// for each interation, flush the net first
		while(fgets(buf,sizeof(buf),fin))	// one line is one poem, and one line will NOT exceed 1023 chars
		{
			sentences.clear();
			split(buf, "\t\r\n", sentences);
			if((int)sentences.size() != QUATRAIN_SEN_NUM
					&& (int)sentences.size() != LVSHI_SEN_NUM) // here is just for quatrain
			{
				fprintf(stderr, "This is NOT a quatrain or lvshi!!!\n");
				continue;
			}
			trainPoem(sentences);
			poem_cnt ++;
			sen_cnt += sentences.size();
			if(poem_cnt % 100 == 0)
			{
				printf("%cIter: %3d\tAlpha: %f\t   TRAIN entropy: %.4f (%.4f)   Progress: %.2f%%", 13, iter, alpha, -logp/log10(2)/wordCounter,
						exp10(-logp/(double)wordCounter), sen_cnt/(double)totalSentenceCount*100);
				fflush(stdout);
			}

			// just for observation
			if(saveModel == 1)
			{
				if(poem_cnt % 10000 == 1)
				{
					char modelName[1024];
					sprintf(modelName, "%s_iter%03d_%05d", modelFile, iter, poem_cnt);
					saveNet(modelName);
				}
			}
		}
		fclose(fin);
		printf("%cIter: %3d\tAlpha: %f\t   TRAIN entropy: %.4f (%.4f)    ", 13, iter, alpha, -logp/log10(2)/wordCounter, exp10(-logp/(double)wordCounter));
		fflush(stdout);

		// this is for the validation data
		logp = 0;
		wordCounter = 0;
		flushNet();
		testNetFile(validFile);
		printf("VALID entropy: %.4f (%.4f)    ", -logp/log10(2)/wordCounter, exp10(-logp/(double)wordCounter));

		double validationLogp = logp;

		// this is for the test data
		logp = 0;
		wordCounter = 0;
		flushNet();
		testNetFile(testFile);
		printf("TEST entropy: %.4f (%.4f)\n", -logp/log10(2)/wordCounter, exp10(-logp/(double)wordCounter));

		logp = validationLogp;

		if (logp*minImprovement < lastLogp)
			restoreWeights();
		else
			saveWeights();

		if (logp*minImprovement < lastLogp)
		{   //***maybe put some variable here to define what is minimal improvement??
			if (alphaDivide == 0)
				alphaDivide = 1;
			else
				break;
		}

		if (alphaDivide) alpha/=2;

		lastLogp = logp;
	}

	// final entropy and perplexity on test data
	// this is for the test data
	logp = 0;
	wordCounter = 0;
	flushNet();
	testNetFile(testFile);
	printf("final TEST entropy: %.4f (%.4f)\n", -logp/log10(2)/wordCounter, exp10(-logp/(double)wordCounter));

	//////////////////////////////////////////////////////////////////////////////////////////////
	if(saveModel > 0)
	{
		cout << "saving final model!" << endl;
		char modelName[1024];
		sprintf(modelName, "%s_%f.model", modelFile, oriAlpha);
		saveNet(modelName);
		cout << "saving final model done!" << endl;
	}
}

// outConditionDSyn still missing
void RNNPG::saveSynapse(FILE *fout)
{
	int i, j, N;
	for(i = 0; i < MAX_CON_N; i ++)
	{
		fprintf(fout, "convolutional matrix %d:\n", i);
		N = hiddenSize * conSeq[i];
		for(j = 0; j < N; j ++)
			fprintf(fout, "%.16g\n", conSyn[i][j].weight);
		fprintf(fout, "\n");
	}
	fprintf(fout, "\n\n");

	fprintf(fout, "word embedding matrix:\n");
	N = vocab.getVocabSize() * hiddenSize;
	for(i = 0; i < N; i ++)
		fprintf(fout, "%.16g\n", senweSyn[i].weight);
	fprintf(fout, "\n\n");

	N = hiddenSize * 2 * hiddenSize;
	fprintf(fout, "compress matrix:\n");
	for(i = 0; i < N; i ++)
		fprintf(fout, "%.16g\n", compressSyn[i].weight);
	fprintf(fout, "\n\n");

	for(i = 0; i < 8; i ++)
	{
		fprintf(fout, "7 character map matrix %d:\n", i);
		N = hiddenSize * hiddenSize;
		for(j = 0; j < N; j ++)
			fprintf(fout, "%.16g\n", map7Syn[i][j].weight);
		fprintf(fout, "\n");
	}
	for(i = 0; i < 6; i ++)
	{
		fprintf(fout, "5 character map matrix %d:\n", i);
		N = hiddenSize * hiddenSize;
		for(j = 0; j < N; j ++)
			fprintf(fout, "%.16g\n", map5Syn[i][j].weight);
		fprintf(fout, "\n");
	}
	fprintf(fout, "\n\n");

	fprintf(fout, "weight matrix from hidden layer to input layer:\n");
	N = hiddenSize * (hiddenSize + hiddenSize + vocab.getVocabSize());
	for(i = 0; i < N; i ++)
		fprintf(fout, "%.16g\n", hiddenInSyn[i].weight);
	fprintf(fout, "\n\n");

	fprintf(fout, "weight matrix from output layer to hidden layer:\n");
	N = (vocab.getVocabSize() + classSize) * hiddenSize;
	for(i = 0; i < N; i ++)
		fprintf(fout, "%.16g\n", outHiddenSyn[i].weight);
	fprintf(fout, "\n\n");

	fprintf(fout, "direct error matrix from input layer to output layer:\n");
	N = (vocab.getVocabSize()+classSize) * hiddenSize;
	for(i = 0; i < N; i ++)
		fprintf(fout, "%.16g\n", outConditionDSyn[i].weight);
	fprintf(fout, "\n\n");
}

// outConditionDSyn still missing
void RNNPG::loadSynapse(FILE *fin)
{
	int i, j, N;
	for(i = 0; i < MAX_CON_N; i ++)
	{
		skiputil(':', fin);
		N = hiddenSize * conSeq[i];
		for(j = 0; j < N; j ++)
			fscanf(fin, "%lf\n", &conSyn[i][j].weight);
	}

	skiputil(':', fin);
	N = vocab.getVocabSize() * hiddenSize;
	for(i = 0; i < N; i ++)
		fscanf(fin, "%lf", &senweSyn[i].weight);

	skiputil(':', fin);
	N = hiddenSize * 2 * hiddenSize;
	for(i = 0; i < N; i ++)
		fscanf(fin, "%lf", &compressSyn[i].weight);

	for(i = 0; i < 8; i ++)
	{
		skiputil(':', fin);
		N = hiddenSize * hiddenSize;
		for(j = 0; j < N; j ++)
			fscanf(fin, "%lf", &map7Syn[i][j].weight);
	}
	for(i = 0; i < 6; i ++)
	{
		skiputil(':', fin);
		N = hiddenSize * hiddenSize;
		for(j = 0; j < N; j ++)
			fscanf(fin, "%lf", &map5Syn[i][j].weight);
	}
	skiputil(':', fin);
	N = hiddenSize * (vocab.getVocabSize() + hiddenSize + hiddenSize);
	for(i = 0; i < N; i ++)
		fscanf(fin, "%lf", &hiddenInSyn[i].weight);

	skiputil(':', fin);
	N = (vocab.getVocabSize() + classSize) * hiddenSize;
	for(i = 0; i < N; i ++)
		fscanf(fin, "%lf", &outHiddenSyn[i].weight);

	skiputil(':', fin);
	N = (vocab.getVocabSize()+classSize) * hiddenSize;
	for(i = 0; i < N; i ++)
		fscanf(fin, "%lf", &outConditionDSyn[i].weight);
}

void RNNPG::loadNet(const char *infile)
{
	FILE *fin = xfopen(infile, "rb");
	loadBasicSetting(fin);
	vocab.load(fin);
	if(inNeu == NULL)
		initNet();
	loadSynapse(fin);
	loadNeuron(fin);
	fclose(fin);
}

void RNNPG::saveNet(const char *outfile)
{
	FILE *fout = xfopen(outfile, "wb");
	saveBasicSetting(fout);
	vocab.save(fout);
	saveSynapse(fout);
	saveNeuron(fout);
	fclose(fout);
}

void RNNPG::saveNeuron(FILE *fout)
{
	int i, j, N, unitNum;
	unitNum = 7;
	for(i = 0; i < SEN7_HIGHT; i ++)
	{
		N = unitNum * hiddenSize;
		fprintf(fout, "neuron in 7 char sentence model, layer %d:\n", i);
		for(j = 0; j < N; j ++)
			fprintf(fout, "%.16g %.16g\n", sen7Neu[i][j].ac, sen7Neu[i][j].er);
		fprintf(fout, "\n");
		if(unitNum > 1)	unitNum -= conSeq[i] - 1;
	}
	fprintf(fout, "\n\n");

	unitNum = 5;
	for(i = 0; i < SEN5_HIGHT; i ++)
	{
		N = unitNum * hiddenSize;
		fprintf(fout, "neuron in 5 char sentence model, layer %d:\n", i);
		for(j = 0; j < N; j ++)
			fprintf(fout, "%.16g %.16g\n", sen5Neu[i][j].ac, sen5Neu[i][j].er);
		fprintf(fout, "\n");
		if(unitNum > 1)	unitNum -= conSeq[i] - 1;
	}
	fprintf(fout, "\n\n");

	fprintf(fout, "history neuron:\n");
	N = hiddenSize;
	for(i = 0; i < N; i ++)
		fprintf(fout, "%.16g %.16g\n", hisNeu[i].ac, hisNeu[i].er);
	fprintf(fout, "\n\n");

	fprintf(fout, "combine neuron:\n");
	N = hiddenSize * 2;
	for(i = 0; i < N; i ++)
		fprintf(fout, "%.16g %.16g\n", cmbNeu[i].ac, cmbNeu[i].er);
	fprintf(fout, "\n\n");

	fprintf(fout, "condition neuron:\n");
	N = hiddenSize;
	for(i = 0; i < N; i ++)
		fprintf(fout, "%.16g %.16g\n", conditionNeu[i].ac, conditionNeu[i].er);
	fprintf(fout, "\n\n");

	fprintf(fout, "input neuron:\n");
	N = vocab.getVocabSize() + hiddenSize + hiddenSize;
	for(i = 0; i < N; i ++)
		fprintf(fout, "%.16g %.16g\n", inNeu[i].ac, inNeu[i].er);
	fprintf(fout, "\n\n");

	fprintf(fout, "hidden neuron:\n");
	N = hiddenSize;
	for(i = 0; i < N; i ++)
		fprintf(fout, "%.16g %.16g\n", hiddenNeu[i].ac, hiddenNeu[i].er);
	fprintf(fout, "\n\n");

	fprintf(fout, "output neuron:\n");
	N = vocab.getVocabSize() + classSize;
	for(i = 0; i < N; i ++)
		fprintf(fout, "%.16g %.16g\n", outNeu[i].ac, outNeu[i].er);
	fprintf(fout, "\n\n");
}

void RNNPG::loadNeuron(FILE *fin)
{
	int i, j, N, unitNum;
	unitNum = 7;
	for(i = 0; i < SEN7_HIGHT; i ++)
	{
		N = unitNum * hiddenSize;
		skiputil(':', fin);
		for(j = 0; j < N; j ++)
			fscanf(fin, "%lf %lf", &sen7Neu[i][j].ac, &sen7Neu[i][j].er);
		if(unitNum > 1)	unitNum -= conSeq[i] - 1;
	}

	unitNum = 5;
	for(i = 0; i < SEN5_HIGHT; i ++)
	{
		N = unitNum * hiddenSize;
		skiputil(':', fin);
		for(j = 0; j < N; j ++)
			fscanf(fin, "%lf %lf", &sen5Neu[i][j].ac, &sen5Neu[i][j].er);
		if(unitNum > 1)	unitNum -= conSeq[i] - 1;
	}

	skiputil(':', fin);
	N = hiddenSize;
	for(i = 0; i < N; i ++)
		fscanf(fin, "%lf %lf", &hisNeu[i].ac, &hisNeu[i].er);

	skiputil(':', fin);
	N = hiddenSize * 2;
	for(i = 0; i < N; i ++)
		fscanf(fin, "%lf %lf", &cmbNeu[i].ac, &cmbNeu[i].er);

	skiputil(':', fin);
	N = hiddenSize;
	for(i = 0; i < N; i ++)
		fscanf(fin, "%lf %lf", &conditionNeu[i].ac, &conditionNeu[i].er);

	skiputil(':', fin);
	N = vocab.getVocabSize() + hiddenSize + hiddenSize;
	for(i = 0; i < N; i ++)
		fscanf(fin, "%lf %lf", &inNeu[i].ac, &inNeu[i].er);

	skiputil(':', fin);
	N = hiddenSize;
	for(i = 0; i < N; i ++)
		fscanf(fin, "%lf %lf", &hiddenNeu[i].ac, &hiddenNeu[i].er);

	skiputil(':', fin);
	N = vocab.getVocabSize() + classSize;
	for(i = 0; i < N; i ++)
		fscanf(fin, "%lf %lf", &outNeu[i].ac, &outNeu[i].er);
}

void RNNPG::saveBasicSetting(FILE *fout)
{
	fprintf(fout, "alpha:%.16g\n", alpha);
	fprintf(fout, "beta:%.16g\n", beta);
	fprintf(fout, "hiddenSize:%d\n", hiddenSize);
	fprintf(fout, "classSize:%d\n", classSize);
	fprintf(fout, "max iter:%d\n", maxIter);
	fprintf(fout, "fix first:%d\n", fixSentenceModelFirstLayer);
	fprintf(fout, "rand init:%d\n", randomlyInitSenModelEmbedding);
	fprintf(fout, "train file:%s\n", trainFile);
	fprintf(fout, "valid file:%s\n", validFile);
	fprintf(fout, "test file:%s\n", testFile);
	fprintf(fout, "word embeding file:%s\n", wordEmbeddingFile);
	fprintf(fout, "random seed:%d\n", randomSeed);
	fprintf(fout, "min Improvement:%.16g\n", minImprovement);
	// fprintf(fout, "\n\n");

	fprintf(fout, "word counter:%d\n", wordCounter);
	fprintf(fout, "total poem count:%d\n", totalPoemCount);
	fprintf(fout, "log prob:%.16g\n", logp);
	fprintf(fout, "alpha divide:%d\n", alphaDivide);
	fprintf(fout, "stableAC:%.16g\n", stableAC);
	fprintf(fout, "flush option:%d\n", flushOption);
	fprintf(fout, "minimum value for convolutional matrix:%.16g\n", consynMin);
	fprintf(fout, "maximum value for convolutional matrix:%.16g\n", consynMax);
	fprintf(fout, "offset of the value for convolutional matrix:%.16g\n", consynOffset);
	fprintf(fout, "the direct error from output layer to condition layer:%d\n", directError);
	fprintf(fout, "BPTT training for the recurrent context model (during learning the sentence model):%d\n", conbptt);
	fprintf(fout, "\n\n");
}

void RNNPG::loadBasicSetting(FILE *fin)
{
	skiputil(':', fin);	fscanf(fin, "%lf", &alpha);
	skiputil(':', fin);	fscanf(fin, "%lf", &beta);
	skiputil(':', fin);	fscanf(fin, "%d", &hiddenSize);
	skiputil(':', fin);	fscanf(fin, "%d", &classSize);
	skiputil(':', fin);	fscanf(fin, "%d", &maxIter);
	int bval;
	skiputil(':', fin);	fscanf(fin, "%d", &bval);
	fixSentenceModelFirstLayer = bval;
	skiputil(':', fin);	fscanf(fin, "%d", &bval);
	randomlyInitSenModelEmbedding = bval;
	skiputil(':', fin);	fscanf(fin, "%s", trainFile);
	skiputil(':', fin);	fscanf(fin, "%s", validFile);
	skiputil(':', fin);	fscanf(fin, "%s", testFile);
	skiputil(':', fin);	fscanf(fin, "%s", wordEmbeddingFile);
	skiputil(':', fin);	fscanf(fin, "%d", &randomSeed);
	skiputil(':', fin);	fscanf(fin, "%lf", &minImprovement);
	skiputil(':', fin);	fscanf(fin, "%d", &wordCounter);
	skiputil(':', fin);	fscanf(fin, "%d", &totalPoemCount);
	skiputil(':', fin);	fscanf(fin, "%lf", &logp);
	skiputil(':', fin);	fscanf(fin, "%d", &alphaDivide);
	skiputil(':', fin);	fscanf(fin, "%lf", &stableAC);
	skiputil(':', fin);	fscanf(fin, "%d", &flushOption);
	skiputil(':', fin);	fscanf(fin, "%lf", &consynMin);
	skiputil(':', fin);	fscanf(fin, "%lf", &consynMax);
	skiputil(':', fin);	fscanf(fin, "%lf", &consynOffset);
	skiputil(':', fin);	fscanf(fin, "%d", &bval);
	directError = bval;
	skiputil(':', fin);	fscanf(fin, "%d", &bval);
	conbptt = bval;
}

void RNNPG::showParameters()
{
	// printf("-conf <conf file>  -- configuration file, the program first read the configuration file options and then use the command line options. That is to say command line options have higher priorities\n");
	printf("-alpha %f\n", alpha);
	printf("-beta  %g\n", beta);
	printf("-hidden %d      -- hidden layer size\n", hiddenSize);
	printf("-class  %d      -- class size\n", classSize);
	printf("-iter   %d      -- max iteration\n", maxIter);
	printf("-fixFirst %s   -- true means fix the first layer of the sentence model\n", fixSentenceModelFirstLayer ? "true" : "false");
	printf("-randInit %s   -- true means randomly initialize sentence model word embedding; false means using word2vec to initialize the layer\n",
			randomlyInitSenModelEmbedding ? "true" : "false");
	printf("-trainF    %s\n", trainFile);
	printf("-validF    %s\n", validFile);
	printf("-testF     %s\n", testFile);
	printf("-embedF    %s\n", wordEmbeddingFile);
	printf("-saveModel %d  -- 0 do not save model; 1 save every 10000 poems; 2 save at the end\n", saveModel);
	printf("-modelF    %s\n", modelFile);
	printf("-randSeed  %d   -- random seed for all the random initialization\n", randomSeed);
	printf("-minImp  %f   -- minmum improvment for convergence, default 1.0001\n", minImprovement);
	printf("-stableAC %f  -- everytime when flushing the network, activation in last time hidden layer is set to 'stableAC', default 0.1\n", stableAC);
	printf("-flushOption %d  -- 1, flush the network every sentence; 2, flush the network every poem; 3, never flush (only flush at the beginning of training)\n", flushOption);
	printf("-consynMin %f  -- minimum value for convolutional matrix\n", consynMin);
	printf("-consynMax %f  -- maximum value for convolutional matrix\n", consynMax);
	printf("-consynOffset %f -- offset of the value for convolutional matrix\n", consynOffset);
	printf("-direct %s -- true means the direct error from output layer to condition layer will be used\n",
			directError ? "true" : "false");
	printf("-conbptt %s -- true means use BPTT training for the recurrent context model (during learning the sentence model)\n",
				conbptt ? "true" : "false");
	printf("-vocabClassF %s\n", vocabClassF);
	printf("-perSentUpdate %s\n", perSentUpdate ? "true" : "false");
	printf("-historyStableAC %f -- stableAC in recurrent context model\n", historyStableAC);
	printf("-adaGrad %s -- true means using AdaGrad training algorithm; false means using SDG\n", adaGrad ? "true" : "false");
}

// this function is from Tomas Mikolov's toolkit, rnnlm-0.2b
void RNNPG::matrixXvector(struct neuron *dest, struct neuron *srcvec, struct synapse *srcmatrix, int matrix_width, int from, int to, int from2, int to2, int type)
{
    int a, b;
    double val1, val2, val3, val4;
    double val5, val6, val7, val8;

    if (type==0) {		//ac mod
	for (b=0; b<(to-from)/8; b++) {
	    val1=0;
	    val2=0;
	    val3=0;
	    val4=0;

	    val5=0;
	    val6=0;
	    val7=0;
	    val8=0;

	    for (a=from2; a<to2; a++) {
    		val1 += srcvec[a].ac * srcmatrix[a+(b*8+from+0)*matrix_width].weight;
    		val2 += srcvec[a].ac * srcmatrix[a+(b*8+from+1)*matrix_width].weight;
    		val3 += srcvec[a].ac * srcmatrix[a+(b*8+from+2)*matrix_width].weight;
    		val4 += srcvec[a].ac * srcmatrix[a+(b*8+from+3)*matrix_width].weight;

    		val5 += srcvec[a].ac * srcmatrix[a+(b*8+from+4)*matrix_width].weight;
    		val6 += srcvec[a].ac * srcmatrix[a+(b*8+from+5)*matrix_width].weight;
    		val7 += srcvec[a].ac * srcmatrix[a+(b*8+from+6)*matrix_width].weight;
    		val8 += srcvec[a].ac * srcmatrix[a+(b*8+from+7)*matrix_width].weight;
    	    }
    	    dest[b*8+from+0].ac += val1;
    	    dest[b*8+from+1].ac += val2;
    	    dest[b*8+from+2].ac += val3;
    	    dest[b*8+from+3].ac += val4;

    	    dest[b*8+from+4].ac += val5;
    	    dest[b*8+from+5].ac += val6;
    	    dest[b*8+from+6].ac += val7;
    	    dest[b*8+from+7].ac += val8;
	}

	for (b=b*8; b<to-from; b++) {
	    for (a=from2; a<to2; a++) {
    		dest[b+from].ac += srcvec[a].ac * srcmatrix[a+(b+from)*matrix_width].weight;
    	    }
    	}
    }
    else {		//er mod
    	for (a=0; a<(to2-from2)/8; a++) {
	    val1=0;
	    val2=0;
	    val3=0;
	    val4=0;

	    val5=0;
	    val6=0;
	    val7=0;
	    val8=0;

	    for (b=from; b<to; b++) {
    	        val1 += srcvec[b].er * srcmatrix[a*8+from2+0+b*matrix_width].weight;
    	        val2 += srcvec[b].er * srcmatrix[a*8+from2+1+b*matrix_width].weight;
    	        val3 += srcvec[b].er * srcmatrix[a*8+from2+2+b*matrix_width].weight;
    	        val4 += srcvec[b].er * srcmatrix[a*8+from2+3+b*matrix_width].weight;

    	        val5 += srcvec[b].er * srcmatrix[a*8+from2+4+b*matrix_width].weight;
    	        val6 += srcvec[b].er * srcmatrix[a*8+from2+5+b*matrix_width].weight;
    	        val7 += srcvec[b].er * srcmatrix[a*8+from2+6+b*matrix_width].weight;
    	        val8 += srcvec[b].er * srcmatrix[a*8+from2+7+b*matrix_width].weight;
    	    }
    	    dest[a*8+from2+0].er += val1;
    	    dest[a*8+from2+1].er += val2;
    	    dest[a*8+from2+2].er += val3;
    	    dest[a*8+from2+3].er += val4;

    	    dest[a*8+from2+4].er += val5;
    	    dest[a*8+from2+5].er += val6;
    	    dest[a*8+from2+6].er += val7;
    	    dest[a*8+from2+7].er += val8;
	}

	for (a=a*8; a<to2-from2; a++) {
	    for (b=from; b<to; b++) {
    		dest[a+from2].er += srcvec[b].er * srcmatrix[a+from2+b*matrix_width].weight;
    	    }
    	}

    	for (a=from2; a<to2; a++) {
    	    if (dest[a].er>15) dest[a].er=15;
    	    if (dest[a].er<-15) dest[a].er=-15;
    	}
    }
}

/**
 * Since given all the previous sentences, the resulting activation caused by these sentences in hiddenNeu is constant,
 * we pre-compute all the activations here
 */
void RNNPG::getContextHiddenNeu(vector<string> &prevSents, neuron **contextHiddenNeu)
{
	int i, SEN_HIGHT = -1;
	neuron **senNeu = NULL;
	synapse **mapSyn = NULL;
	vector<string> words;
	clearNeurons(cmbNeu, hiddenSize * 2, 3);

	if(flushOption == EVERY_POEM)
	{
		flushNet();
		for(i = 0; i < hiddenSize; i ++)
		{
			hiddenNeu[i].ac = stableAC;
			hiddenNeu[i].er = 0;
		}
	}

	// obtaining hisNeu from all previous sentences
	for(i = 0; i < (int)prevSents.size(); i ++)
	{
		words.clear();
		split(prevSents[i], " ", words);
		SEN_HIGHT = words.size() == 5 ? SEN5_HIGHT : SEN7_HIGHT;
		senNeu = words.size() == 5 ? sen5Neu : sen7Neu;
		initSent(words.size());
		neuron *sen_repr = sen2vec(words, senNeu, SEN_HIGHT);		// this is the pointer for the top layer sentence model, DO NOT modify it
		memcpy(cmbNeu + hiddenSize, sen_repr, sizeof(neuron)*hiddenSize);
		clearNeurons(hisNeu, hiddenSize, 3);
		matrixXvector(hisNeu, cmbNeu, compressSyn, hiddenSize * 2, 0, hiddenSize, 0, hiddenSize * 2, 0);
		funACNeurons(hisNeu, hiddenSize);

		if(flushOption == EVERY_POEM && i + 1 < (int)prevSents.size())
		{
			if(mapSyn == NULL)
				mapSyn = words.size() == 5 ? map5Syn : map7Syn;
			words.clear();
			split(prevSents[i + 1], " ", words);
			words.push_back("</s>");	// during generation, we DO care about the End-of-Sentence
			int lastWord = 0, curWord = -1, wdPos;
			for(wdPos = 0; wdPos < (int)words.size(); wdPos ++)
			{
				curWord = vocab.getVocabID(words[wdPos].c_str());
				bool isRare = false;
				if(curWord == -1)
				{
					// when the word cannot be found, we use <R> instead
					curWord = vocab.getVocabID("<R>");
					isRare = true;
				}
				assert(curWord != -1);		// this is impossible, or there is a bug!
				inNeu[lastWord].ac = 1;
				computeNet(lastWord, curWord, wdPos, mapSyn);
				inNeu[lastWord].ac = 0;
				copyHiddenLayerToInput();
				lastWord = curWord;
			}
			words.pop_back();	// generation done and delete </s>
		}

		if(i != (int)prevSents.size() - 1)
			memcpy(cmbNeu, hisNeu, sizeof(neuron)*hiddenSize);
	}

	// after obtaining hisNeu, we will next get the conditionNeu and activation from conditionNeu to hiddenNeu
	int SEN_LEN = words.size() + 1;
	mapSyn = words.size() == 5 ? map5Syn : map7Syn;
	int V = vocab.getVocabSize();
	for(i = 0; i < SEN_LEN; i ++)
	{
		clearNeurons(inNeu + V, hiddenSize, 1);
		matrixXvector(inNeu + V, hisNeu, mapSyn[i], hiddenSize, 0, hiddenSize, 0, hiddenSize, 0);
		funACNeurons(inNeu + V, hiddenSize);
		clearNeurons(contextHiddenNeu[i], hiddenSize, 1);
		matrixXvector(contextHiddenNeu[i], inNeu, hiddenInSyn, V + hiddenSize + hiddenSize, 0, hiddenSize, V, V + hiddenSize, 0);

		// printNeurons(contextHiddenNeu[i], hiddenSize);
	}
}

bool pair_cmp(const pair<string,double> &p1, const pair<string,double> &p2)
{
	return !(p1.second < p2.second);
}

void RNNPG::computeNetContext(const char *lword, const char *cword, neuron *curHiddenNeu, neuron *contextHiddenNeu,
		neuron *newHiddenNeu, vector<pair<string,double> > &nextWordProbs)
{
	int lastWord = vocab.getVocabID(lword);
	int i, j, V = vocab.getVocabSize();

	// recurrent from last time hidden layer: put last time hidden layer in the input layer
	if(lastWord == 0)	// if lword == "</s>"
	{
		if(flushOption == EVERY_SENTENCE)
			flushNet();
		else if(flushOption == EVERY_POEM)
			copyNeurons(inNeu + V + hiddenSize, curHiddenNeu, hiddenSize, 1);
		else
		{
			fprintf(stderr, "flushOption %d not supported right now in computeNetContext (for decoding)\n", flushOption);
			exit(1);
		}
	}
	else
		copyNeurons(inNeu + V + hiddenSize, curHiddenNeu, hiddenSize, 1);

	copyNeurons(hiddenNeu, contextHiddenNeu, hiddenSize, 1); // if lword == 0, the copy is invalid, since flushNet will take care of it  BUG!!!!!!! Now fixed by change the sequence the lines of code above


	int N = V + hiddenSize + hiddenSize;
	matrixXvector(hiddenNeu, inNeu, hiddenInSyn, N, 0, hiddenSize, V + hiddenSize, V + hiddenSize + hiddenSize, 0);
	for(i = 0; i < hiddenSize; i ++)
		hiddenNeu[i].ac += hiddenInSyn[i*N + lastWord].weight;
	funACNeurons(hiddenNeu, hiddenSize);

	copyNeurons(newHiddenNeu, hiddenNeu, hiddenSize, 1);

	// compute probabilities on classes
	clearNeurons(outNeu + V, classSize, 1);
	matrixXvector(outNeu, hiddenNeu, outHiddenSyn, hiddenSize, V, V + classSize, 0, hiddenSize, 0);
	double sum = 0;
	N = V + classSize;
	for(i = V; i < N; i ++)
	{
		if(outNeu[i].ac < -50) outNeu[i].ac = -50;
		if(outNeu[i].ac > 50) outNeu[i].ac = 50;
		outNeu[i].ac = FAST_EXP(outNeu[i].ac);
		sum += outNeu[i].ac;
	}
	for(i = V; i < N; i ++)
		outNeu[i].ac /= sum;

	// vector<pair<string,double>>
	nextWordProbs.clear();
	if(cword == NULL)
	{
		// oh my god, the computational cost is great!!!
		matrixXvector(outNeu, hiddenNeu, outHiddenSyn, hiddenSize, 0, V, 0, hiddenSize, 0);
		for(i = 0; i < classSize; i ++)
		{
			sum = 0;
			// cout << "class start " << classStart[i] << ", class end " << classEnd[i] << endl;
			for(j = classStart[i]; j < classEnd[i]; j ++)
			{
				if(outNeu[j].ac < -50) outNeu[j].ac = -50;
				if(outNeu[j].ac > 50) outNeu[j].ac = 50;
				outNeu[j].ac = FAST_EXP(outNeu[j].ac);
				sum += outNeu[j].ac;
			}
			for(j = classStart[i]; j < classEnd[i]; j ++)
			{
				outNeu[j].ac /= sum;
				nextWordProbs.push_back(make_pair(this->voc_arr[j].wd, outNeu[V+i].ac * outNeu[j].ac));
			}
		}
		sort(nextWordProbs.begin(), nextWordProbs.end(), pair_cmp);
	}
	else
	{
		int curWord = vocab.getVocabID(cword);
		int classIndex = voc_arr[curWord].classIndex;
		matrixXvector(outNeu, hiddenNeu, outHiddenSyn, hiddenSize, classStart[classIndex], classEnd[classIndex], 0, hiddenSize, 0);
		sum = 0;
		for(j = classStart[classIndex]; j < classEnd[classIndex]; j ++)
		{
			if(outNeu[j].ac < -50) outNeu[j].ac = -50;
			if(outNeu[j].ac > 50) outNeu[j].ac = 50;
			outNeu[j].ac = FAST_EXP(outNeu[j].ac);
			sum += outNeu[j].ac;
		}
		for(j = classStart[classIndex]; j < classEnd[classIndex]; j ++)
			outNeu[j].ac /= sum;
		nextWordProbs.push_back(make_pair(this->voc_arr[curWord].wd, outNeu[V+classIndex].ac * outNeu[curWord].ac));
	}
}

double RNNPG::computeNetContext(const char *lword, int startPos, const vector<string> &words, neuron *curHiddenNeu, neuron **contextHiddenNeus,
			neuron *newHiddenNeu)
{
	double phraseLogProb = 0;
	int lastWord = vocab.getVocabID(lword);
	if(lastWord == -1) lastWord = vocab.getVocabID("<R>");
	int i, j, V = vocab.getVocabSize();

	// recurrent from last time hidden layer: put last time hidden layer in the input layer
	if(lastWord == 0)	// if lword == "</s>"
	{
		if(flushOption == EVERY_SENTENCE)
			flushNet();
		else if(flushOption == EVERY_POEM)
			copyNeurons(inNeu + V + hiddenSize, curHiddenNeu, hiddenSize, 1);
		else
		{
			fprintf(stderr, "flushOption %d not supported right now in computeNetContext (for decoding)\n", flushOption);
			exit(1);
		}
	}
	else
		copyNeurons(inNeu + V + hiddenSize, curHiddenNeu, hiddenSize, 1);

	// cout << "copy activation done!" << endl;

	int curPos = startPos, N = V + hiddenSize + hiddenSize;
	for(size_t idx = 0; idx < words.size(); idx ++)
	{
		// cout << "word = " << words[idx] << endl;

		copyNeurons(hiddenNeu, contextHiddenNeus[curPos], hiddenSize, 1);
		matrixXvector(hiddenNeu, inNeu, hiddenInSyn, N, 0, hiddenSize, V + hiddenSize, V + hiddenSize + hiddenSize, 0);
		for(i = 0; i < hiddenSize; i ++)
			hiddenNeu[i].ac += hiddenInSyn[i*N + lastWord].weight;
		funACNeurons(hiddenNeu, hiddenSize);

		// cout << "compute activation done" << endl;

		// compute probs on classes
		clearNeurons(outNeu + V, classSize, 1);
		matrixXvector(outNeu, hiddenNeu, outHiddenSyn, hiddenSize, V, V + classSize, 0, hiddenSize, 0);
		double sum = 0;
		N = V + classSize;
		for(i = V; i < N; i ++)
		{
			if(outNeu[i].ac < -50) outNeu[i].ac = -50;
			if(outNeu[i].ac > 50) outNeu[i].ac = 50;
			outNeu[i].ac = FAST_EXP(outNeu[i].ac);
			sum += outNeu[i].ac;
		}
		for(i = V; i < N; i ++)
			outNeu[i].ac /= sum;

		// cout << "on classes done" << endl;

		// compute probs on words in the current class
		int curWord = vocab.getVocabID(words[idx].c_str());

		// cout << "curWord = " << curWord << endl;

		if(curWord == -1)
			curWord = vocab.getVocabID("<R>");

		int classIndex = voc_arr[curWord].classIndex;
		matrixXvector(outNeu, hiddenNeu, outHiddenSyn, hiddenSize, classStart[classIndex], classEnd[classIndex], 0, hiddenSize, 0);
		sum = 0;
		for(j = classStart[classIndex]; j < classEnd[classIndex]; j ++)
		{
			if(outNeu[j].ac < -50) outNeu[j].ac = -50;
			if(outNeu[j].ac > 50) outNeu[j].ac = 50;
			outNeu[j].ac = FAST_EXP(outNeu[j].ac);
			sum += outNeu[j].ac;
		}
		for(j = classStart[classIndex]; j < classEnd[classIndex]; j ++)
			outNeu[j].ac /= sum;

		phraseLogProb += log(outNeu[V+classIndex].ac * outNeu[curWord].ac);
		lastWord = curWord;

		curPos ++;
		if(idx != words.size() - 1)
			copyHiddenLayerToInput();
		else
			copyNeurons(newHiddenNeu, hiddenNeu, hiddenSize, 1);
	}

	// cout << "all the computation done " << endl;

	return phraseLogProb;
}

void RNNPG::testNet()
{
	mode = TEST_MODE;

	cout << "test net" << endl;
	cout << "modelF = " << modelFile << endl;
	cout << "testF = " << testFile << endl;

	showParameters();

	loadNet(modelFile);
	logp = 0;
	wordCounter = 0;
	flushNet();
	testNetFile(testFile);
	printf("final TEST entropy: %.4f (%.4f)\n", -logp/log10(2)/wordCounter, exp10(-logp/(double)wordCounter));
}

