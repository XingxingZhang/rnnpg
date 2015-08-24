/*
 * main.cpp
 *
 *  Created on: 27 Dec 2013
 *      Author: s1270921
 */

/* *
 * ================= change log ======================
 * 0. Today Sun 16 Feb 2014, I began to write change log in every version
 *
 * 1. model saving is problematic: sprintf(modelName, "%s_%f.model", modelFile, alpha); // The alpha is the final alpha
 * and therefore cannot be used to distinguish different models. Add oriAlpha
 *
 * 2. 23 Feb, 2014. Add new classes to vocabulary. The classes are obtained by clustering the word2vec output. word2vec trained on all poem corpus
 * and we use cluto (./vcluster -clmethod=bagglo -crfun=i2 $clinF 100) to get the resulting classes
 * 		2.1 Vocab.h add void loadVocabClass(const char *infile) and void getClassStartEnd(int *classStart, int *classEnd, int classSize) to support
 * 			new kind of vocab class
 * 		2.2 change sort(vocab, vocab + vocabSize) to qsort. It seems that use sort will course problem... But will only when classSize == 50
 *
 * 3. saving beta might be 0, in saveBasicSetting and showParameters, use %g instead of %f
 *
 * 4. 24 Feb, 2014. Add per sentence update during BPTT training
 *
 * 5. Add historyStableAC to the recurrent context model, the value 0.01 is currently the best. This value doesn't influence the results much
 *
 * 6. add adaGrad flag to control if we use the adaptive gradient descent training algorithm. Initial experiment does not show any improvement.
 * 		Now I will turn off this option
 *
 * 7. save model with more precision. %.16g
 *
 * 8. Also support training with lvshi
 *
 * 9. Add direct ngram feature between input layer to output layer
 *
 * ------------- Decoder -------------
 * 1. support flush option 2 (flush the neuron per poem rather than per sentence)
 * 2. add translation table this time P(S_i|F_i)_max = max{ P_{rnnpg}(S_i) * P(F_i|S_i)}
 * 			S_i is the second sentence and F_i is the first sentence
 * 		2.1 - Delete TransTable.h and TransTable.cpp as this is not actually a translation table
 * 		2.2 - Add a new TranslationTable.h StringBuffer.h StringBuffer.cpp utf8.h utf8.cpp (discarded)
 * 		2.3 - int decodeTransTable(vector<string> &prevSents, int stackSize, int K, vector<string> &topSents); and int decodeTransTable(const char* infile, const char* outfile, int stackSize, int K, int startId = 0);
 * 			to support translation Table
 * 		2.4 - add options (channelOption) for selecting different channel model features
 * 		2.5 - add a re-ranking strategy in the final stack with the rnnpg score
 * 		2.6 - add disableRNN to disable RNN feature and see what happened
 * 		2.7 - support MERT training, add -weightFile add feature weight file to the decoder
 * 	3. add position feature
 * 	4. add positionProb cut for 5 char quatrains and 7 char quatrains
 * 	5. add KN3 feature
 *
 * 	--------------- Poem Generator ----------------
 * 	Today I am sleepy, so write some code to have fun.
 * 	1. add enforce constraints in Decoder.h Decoder.cpp:
 * 	2. add rules to delete repeated words
 * 	3. add rules to delete repeated words (by removing some top sentences)
 * 	4. repair the bug in deleting repeated words
 *
 * 	5. this is the last repair. Add repetition constraints in SMT and RNNPG model. Just delete the top sentences
 */

#include <iostream>
#include <cstdio>
using namespace std;

#include "RNNPG.h"
#include "xutil.h"
#include "XConfig.h"
#include "Decoder.h"
#include "SubsequentSentenceGenerator.h"

int getArgPos(int argc, char **argv, const char *curArg)
{
	for(int i = 1; i < argc; i ++)
		if(strcmp(argv[i], curArg) == 0)
			return i;
	return -1;
}

/*
-conf <conf file>  -- configuration file, the program first read the configuration file options and then use the command line options. That is to say command line options have higher priorities
-alpha <double>    -- learning rate
-beta  <double>    -- regulation weight
-hidden <int>      -- hidden layer size
-class  <int>      -- class size
-iter   <int>      -- max iteration
-fixFirst <bool>   -- true means fix the first layer of the sentence model
-randInit <bool>   -- true means randomly initialize sentence model word embedding; false means using word2vec to initialize the layer
-trainF    <train file>
-validF    <valid file>
-testF     <test file>
-embedF    <word embeding file>
-saveModel <bool>  -- true means model during training
-modelF    <model file>
-randSeed  <int>    -- random seed for all the random initialization
-minImp  <double>   -- minmum improvment for convergence, default 1.0001
-stableAC <double>  -- everytime when flushing the network, activation in last time hidden layer is set to 'stableAC', default 0.1
-flushOption <int>  -- 1, flush the network every sentence; 2, flush the network every poem; 3, never flush (only flush at the beginning of training)
-consynMin <float>  -- minimum value for convolutional matrix
-consynMax <float>  -- maximum value for convolutional matrix
-consynOffset <float> -- offset of the value for convolutional matrix
*/

void printUsage()
{
	printf("-conf <conf file>  -- configuration file, the program first read the configuration file options and then use the command line options. That is to say command line options have higher priorities\n");
	printf("-alpha <double>    -- learning rate\n");
	printf("-beta  <double>    -- regulation weight\n");
	printf("-hidden <int>      -- hidden layer size\n");
	printf("-class  <int>      -- class size\n");
	printf("-iter   <int>      -- max iteration\n");
	printf("-fixFirst <bool>   -- true means fix the first layer of the sentence model\n");
	printf("-randInit <bool>   -- true means randomly initialize sentence model word embedding; false means using word2vec to initialize the layer\n");
	printf("-trainF    <train file>\n");
	printf("-validF    <valid file>\n");
	printf("-testF     <test file>\n");
	printf("-embedF    <word embeding file>\n");
	printf("-saveModel <int>  -- 0 do not save model; 1 save every 10000 poems; 2 save at the end\n");
	printf("-modelF    <model file>\n");
	printf("-randSeed  <int>   -- random seed for all the random initialization\n");
	printf("-minImp  <double>   -- minmum improvment for convergence, default 1.0001\n");
	printf("-stableAC <double>  -- everytime when flushing the network, activation in last time hidden layer is set to 'stableAC', default 0.1\n");
	printf("-flushOption <int>  -- 1, flush the network every sentence; 2, flush the network every poem; 3, never flush (only flush at the beginning of training)\n");
	printf("-consynMin <float>  -- minimum value for convolutional matrix\n");
	printf("-consynMax <float>  -- maximum value for convolutional matrix\n");
	printf("-consynOffset <float> -- offset of the value for convolutional matrix, should always be positive [consynMin - consynOffset, consynMax + consynOffset]\n");
	printf("-direct <bool> -- true means the direct error from output layer to condition layer will be used\n");
	printf("-conbptt <bool> -- true means use BPTT training for the recurrent context model (during learning the sentence model)\n");
	printf("-vocabClassF <vocab class path> -- the path of the new vocabulary class; no path mean do not use new vocabulary class\n");
	printf("-historyStableAC <float> -- stableAC in recurrent context model\n");
	printf("-adaGrad <bool> -- true means using AdaGrad training algorithm; false means using SDG\n");
	printf("-directHashSize <int> -- add direct ngram feature to output layer; size of the hash for direct connections with n-gram features in millions\n");
	printf("-directNGramOrder <int> -- the n-gram order for direct connections (max 20); default is 3\n");
}

void cmdLineConf(int argc, char **argv)
{
	cout << "RNN_Poem_Generation_12 Direct NGram Feature; Note that decoder call direct interface connection not added -- directSyn_backup memory allocated" << endl;
	if(argc < 3)
	{
		printUsage();
		exit(1);
	}
	// RNNPG rnnpg;
	int pos;
	const int PATH_LEN = 1024;
	double alpha = 0.05;
	double beta = 0.0000001;
	int hiddenSize = 100;
	int classSize = 100;
	int maxIter = 50;
	bool fixFirst = false;
	bool randInit = false;
	char trainF[PATH_LEN];
	char validF[PATH_LEN];
	char testF[PATH_LEN];
	char embedF[PATH_LEN];
	char modelF[PATH_LEN];
	char vocabClassF[PATH_LEN];
	trainF[0] = 0;
	validF[0] = 0;
	testF[0] = 0;
	embedF[0] = 0;
	modelF[0] = 0;
	vocabClassF[0] = 0;
	int saveModel = 0;
	int randSeed = 1;
	double minImp = 1.0001;
	double stableAC = 0.1;
	int flushOption = 1;		// flush every sentence
	double consynMin = -10;
	double consynMax = 10;
	double consynOffset = 0;
	bool direct = false;
	bool conbptt = false;
	double historyStableAC = 0.01;
	bool adaGrad = false;
	long long directHashSize = 1000000;
	int directNGramOrder = 3;
	if((pos = getArgPos(argc, argv, "-conf")) != -1)
	{
		XConfig::load(argv[pos + 1]);
		alpha = XConfig::getDouble( "alpha" );
		beta = XConfig::getDouble( "beta" );
		hiddenSize = XConfig::getInt( "hidden" );
		classSize = XConfig::getInt( "class" );
		maxIter = XConfig::getInt( "iter" );
		fixFirst = XConfig::getBool( "fixFirst" );
		randInit = XConfig::getBool( "randInit" );
		xstrcpy(trainF, sizeof(trainF), XConfig::getStr("trainF"));
		xstrcpy(validF, sizeof(validF), XConfig::getStr("validF"));
		xstrcpy(testF, sizeof(testF), XConfig::getStr("testF"));
		xstrcpy(embedF, sizeof(embedF), XConfig::getStr("embedF"));
		saveModel = XConfig::getInt( "saveModel" );
		xstrcpy(modelF, sizeof(modelF), XConfig::getStr("modelF"));
		randSeed = XConfig::getInt("randSeed");
		minImp = XConfig::getDouble("minImp");
		stableAC = XConfig::getDouble("stableAC");
		flushOption = XConfig::getInt("flushOption");
		consynMin = XConfig::getDouble("consynMin");
		consynMax = XConfig::getDouble("consynMax");
		consynOffset = XConfig::getDouble("consynOffset");
		direct = XConfig::getBool("direct");
		conbptt = XConfig::getBool("conbptt");
		xstrcpy(vocabClassF, sizeof(vocabClassF), XConfig::getStr("vocabClassF"));
		if(strcmp(vocabClassF, "NO_SUCH_KEY") == 0)
			vocabClassF[0] = 0;

		historyStableAC = XConfig::getDouble("historyStableAC");
		adaGrad = XConfig::getBool( "adaGrad" );

		int tmp = XConfig::getInt("directHashSize");
		tmp = tmp <= 0 ? 1 : tmp;
		directHashSize *= tmp;

		directNGramOrder = XConfig::getInt("directNGramOrder");
	}
	if((pos = getArgPos(argc, argv, "-alpha")) != -1)
		alpha = atof(argv[pos + 1]);

	if((pos = getArgPos(argc, argv, "-beta")) != -1)
		beta = atof(argv[pos + 1]);

	if((pos = getArgPos(argc, argv, "-hidden")) != -1)
		hiddenSize = atoi(argv[pos + 1]);

	if((pos = getArgPos(argc, argv, "-class")) != -1)
		classSize = atoi(argv[pos + 1]);

	if((pos = getArgPos(argc, argv, "-iter")) != -1)
		maxIter = atoi(argv[pos + 1]);

	if((pos = getArgPos(argc, argv, "-fixFirst")) != -1)
		fixFirst = atob(argv[pos + 1]);

	if((pos = getArgPos(argc, argv, "-randInit")) != -1)
		randInit = atob(argv[pos + 1]);

	if((pos = getArgPos(argc, argv, "-trainF")) != -1)
		xstrcpy(trainF, sizeof(trainF), argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-validF")) != -1)
		xstrcpy(validF, sizeof(validF), argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-testF")) != -1)
		xstrcpy(testF, sizeof(testF), argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-embedF")) != -1)
		xstrcpy(embedF, sizeof(embedF), argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-saveModel")) != -1)
		saveModel = atob(argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-modelF")) != -1)
		xstrcpy(modelF, sizeof(modelF), argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-randSeed")) != -1)
		randSeed = atoi(argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-minImp")) != -1)
			minImp = atof(argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-stableAC")) != -1)
		stableAC = atof(argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-flushOption")) != -1)
		flushOption = atoi(argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-consynMin")) != -1)
		consynMin = atof(argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-consynMax")) != -1)
		consynMax = atof(argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-consynOffset")) != -1)
		consynOffset = atof(argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-direct")) != -1)
		direct = atob(argv[pos + 1]);

	if((pos = getArgPos(argc, argv, "-conbptt")) != -1)
		conbptt = atob(argv[pos + 1]);

	if((pos = getArgPos(argc, argv, "-vocabClassF")) != -1)
		xstrcpy(validF, sizeof(validF), argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-historyStableAC")) != -1)
		historyStableAC = atof(argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-adaGrad")) != -1)
		adaGrad = atob(argv[pos+1]);

	int tmp;
	// 10000000
	if((pos = getArgPos(argc, argv, "-directHashSize")) != -1)
	{
		tmp = atoi(argv[pos+1]);
		tmp = tmp <= 0 ? 1 : tmp;
		directHashSize *= tmp;
	}

	if((pos = getArgPos(argc, argv, "-directNGramOrder")) != -1)
		directNGramOrder = atoi(argv[pos+1]);

	RNNPG rnnpg;
	rnnpg.setAlpha(alpha);
	rnnpg.setBeta(beta);
	rnnpg.setHiddenSize(hiddenSize);
	rnnpg.setClassSize(classSize);
	rnnpg.setMaxIter(maxIter);
	rnnpg.setFixSentenceModelFirstLayer(fixFirst);
	rnnpg.setTrainFile(trainF);
	rnnpg.setValidFile(validF);
	rnnpg.setTestFile(testF);
	rnnpg.setWordEmbeddingFile(embedF);
	rnnpg.setSaveModel(saveModel);
	rnnpg.setModelFile(modelF);
	rnnpg.setRandomSeed(randSeed);
	rnnpg.setRandomlyInitSenModelEmbedding(randInit);
	rnnpg.setMinImprovement(minImp);
	rnnpg.setStableAC(stableAC);
	rnnpg.setFlushOption(flushOption);
	rnnpg.setConsynMin(consynMin);
	rnnpg.setConsynMax(consynMax);
	rnnpg.setConsynOffset(consynOffset);
	rnnpg.setDirectError(direct);
	rnnpg.setConbptt(conbptt);

	rnnpg.setHistoryStableAC(historyStableAC);

	// this is for test, turned off
	rnnpg.setPerSentUpdate(false);

	// this is for test, no improvments yet. Turn off!
	rnnpg.setAdaGrad(false);

	rnnpg.setDirectNGramHashSize(directHashSize);
	rnnpg.setDirectNgramOrder(directNGramOrder);

	if(vocabClassF[0] != 0)
		rnnpg.setVocabClassFile(vocabClassF);

	if(trainF[0] != 0 && validF[0] != 0 && testF[0] != 0)
		rnnpg.trainNet();

	if(trainF[0] == 0 && validF[0] == 0 && testF[0] != 0)
		rnnpg.testNet();
}

void testDecoder()
{
	const char *modelPath = "/afs/inf.ed.ac.uk/user/s12/s1270921/Desktop/programming/c++/RNN-Related/RNN_Poem_Generation_Decoder/models/iter066_74809_final";
	RNNPG rnnpg;
	rnnpg.loadNet(modelPath);
	TranslationTable transTable;
	transTable.load("/disk/scratch/RNN_POEM/Data/quatrain/translation-table/trans_tabl_L1.txt");

	Decoder decoder(&rnnpg, NULL);
	vector<string> prevSents;
	int stackSize = 300, topK = 300;
//	prevSents.push_back("床 前 明 月 光");
//	prevSents.push_back("疑 是 地 上 霜");
//	prevSents.push_back("举 头 望 明 月");

//	prevSents.push_back("江 枫 渔 火 对 愁 眠");
//	prevSents.push_back("孤 苏 城 外 寒 山 寺");

//	prevSents.push_back("月 落 乌 啼 霜 满 天");
//	prevSents.push_back("江 枫 渔 火 对 愁 眠");
//	prevSents.push_back("孤 苏 城 外 寒 山 寺");

	cout << "start" << endl;
	clock_t start = clock();
//	prevSents.push_back("春 眠 不 觉 晓");
//	prevSents.push_back("处 处 闻 啼 鸟");
//	prevSents.push_back("夜 来 风 雨 声");
	prevSents.push_back("夜 来 风 雨 R");

	vector<string> topSents;
	decoder.decode(prevSents, stackSize, topK, topSents);
	for(int i = 0; i < (int)topSents.size(); i ++)
		cout << topSents[i] << endl;
	clock_t end = clock();
	cout << "time spend " << (double)(end - start)/CLOCKS_PER_SEC << " S" << endl;
}

void printUsageDecoder()
{
	printf("-inFile <infile>\n");
	printf("-outFile <outfile>\n");
	printf("-model <modelPath>\n");
	printf("-transTable  <translation table path>\n");
	printf("-stackSize <int>  -- stack size during decoding\n");
	printf("-topK  <int>      -- topK ranked decoded sentences will be returned\n");
	printf("-startId   <int>  -- start id for the decoder, the first column of the returned result is for the sentence id\n");
	printf("-channelOption <int> -- 1 for P(S_i | F_i); 2 for P(F_i | S_i); 3 for P(S_i | F_i) and P(F_i | S_i); 0 for only use the translation table for phrase selection\n");
	printf("-rerank <int> -- 1 for re-rank results with RNNPG values; 0 for not\n");
	printf("-disableRNN <int> -- 1 disable RNN during decoding; 0 for not\n");
	printf("-weightFile <weightFile>\n");
	printf("-quatrainPosFile <quatrainPosFile> -- this is the file to compute P(pos|ch), the probability of one char 'ch' that appears in a specific position\n");
	printf("-posProbCut5 <float>\n");
	printf("-posProbCut7 <float>\n");
	printf("-ngramLM <ngram (KN3 KenLM) model path>\n");
	printf("-interpolateWeight <float> -- the weight for ngram model interpolate\n");
	printf("-ngramFeatOn <int> -- 1 for open ngram feature, 0 for close ngram feature\n");
}

void cmdLineDecoder(int argc, char **argv)
{
	cout << "decoder 11: add char position feature; with pos prob cut; with KN3 language model; with interpolate (bug fixed)" << endl;
	if(argc < 3)
	{
		printUsageDecoder();
		exit(1);
	}

	int pos = -1;
	const int PATH_LENGTH = 1024;
	char infile[PATH_LENGTH];	infile[0] = 0;
	char outfile[PATH_LENGTH];	outfile[0] = 0;
	char modelPath[PATH_LENGTH]; modelPath[0] = 0;
	char transTablePath[PATH_LENGTH]; transTablePath[0] = 0;
	char weightPath[PATH_LENGTH]; weightPath[0] = 0;
	char quatrainPosPath[PATH_LENGTH]; quatrainPosPath[0] = 0;
	char ngramLMPath[PATH_LENGTH]; ngramLMPath[0] = 0;
	int stackSize = 300;
	int topK = 300;
	int startId = 0;
	int channelOption = 1;
	int rerank = 0;
	int disableRNN = 0;
	double posProbCut5 = 1;
	double posProbCut7 = 1;
	double interpolateWeight = 0;
	int ngramFeatOn = 0;

	if((pos = getArgPos(argc, argv, "-inFile")) != -1)
		xstrcpy(infile, sizeof(infile), argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-outFile")) != -1)
		xstrcpy(outfile, sizeof(outfile), argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-model")) != -1)
		xstrcpy(modelPath, sizeof(modelPath), argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-transTable")) != -1)
		xstrcpy(transTablePath, sizeof(transTablePath), argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-stackSize")) != -1)
		stackSize = atoi(argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-topK")) != -1)
		topK = atoi(argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-startId")) != -1)
		startId = atoi(argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-channelOption")) != -1)
		channelOption = atoi(argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-rerank")) != -1)
		rerank = atoi(argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-disableRNN")) != -1)
		disableRNN = atoi(argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-weightFile")) != -1)
		xstrcpy(weightPath, sizeof(weightPath), argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-quatrainPosFile")) != -1)
		xstrcpy(quatrainPosPath, sizeof(quatrainPosPath), argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-posProbCut5")) != -1)
		posProbCut5 = atof(argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-posProbCut7")) != -1)
		posProbCut7 = atof(argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-ngramLM")) != -1)
		xstrcpy(ngramLMPath, sizeof(ngramLMPath), argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-interpolateWeight")) != -1)
		interpolateWeight = atof(argv[pos+1]);

	if((pos = getArgPos(argc, argv, "-ngramFeatOn")) != -1)
		ngramFeatOn = atoi(argv[pos+1]);

	RNNPG rnnpg;
	rnnpg.loadNet(modelPath);
	TranslationTable *transTable = NULL;
	if(transTablePath[0] != 0)
	{
		transTable = new TranslationTable();
		transTable->load(transTablePath);
	}
	QuatrainPositionProb *posProb = NULL;
	if(quatrainPosPath[0] != 0)
	{
		posProb = new QuatrainPositionProb();
		posProb->load(quatrainPosPath);
	}
	KenLMM *kenlm = NULL;
	Model *model = NULL;
	if(ngramLMPath[0] != 0)
	{
		model = new Model(ngramLMPath);
		kenlm = new KenLMM(model);
	}
	Decoder decoder(&rnnpg, transTable, kenlm);
	if(weightPath[0] != 0)
		decoder.loadWeights(weightPath);
	decoder.setChannelOption(channelOption);
	decoder.setRerank(rerank);
	decoder.setDisableRNN(disableRNN);
	decoder.setPosProbCut5(posProbCut5);
	decoder.setPosProbCut7(posProbCut7);
	decoder.setInterpolateWeight(interpolateWeight);
	decoder.setNgramFeatureOn(ngramFeatOn);
	if(posProb != NULL)
		decoder.setQuatrainPositionProb(posProb);
	if(transTable == NULL)
		decoder.decode(infile, outfile, stackSize, topK, startId);
	else
		decoder.decodeTransTable(infile, outfile, stackSize, topK, startId);

	if(transTable != NULL) delete transTable;
	if(posProb != NULL) delete posProb;

	if(kenlm != NULL)
	{
		delete kenlm;
		delete model;
	}
}

void testPoemGenerator()
{
	XConfig::load("poem_generator.conf");
	SubsequentSentenceGenerator ssg;
	ssg.generatePoems("/afs/inf.ed.ac.uk/user/s12/s1270921/Desktop/programming/c++/RNN-Related/FirstSentenceGenerator/firstSent.txt",
			"output.txt");
}

void cmdLineGenerator(int argc, char **argv)
{
	// cout << "Poem generator version 1.4 repair the bug in deleting repeated words" << endl;
	cout << "Poem generator version 1.5 this is the last repair. Add repetition constraints in SMT and RNNPG model. Just delete the top sentences" << endl;
	if(argc !=4)
	{
		fprintf(stderr, "rnn_poem_generator <configFile> <inputFirstSentenceFile> <outputPoemFile>\n");
		return;
	}
	XConfig::load(argv[1]);
	SubsequentSentenceGenerator ssg;
	ssg.generatePoems(argv[2], argv[3]);
}

int main(int argc, char **argv)
{
	// cmdLineDecoder(argc, argv);
	// cmdLineConf(argc, argv);
	// testPoemGenerator();
	cmdLineGenerator(argc, argv);

	return 0;
}
