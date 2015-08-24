/*
 * main.cpp
 *
 *  Created on: 10 Mar 2014
 *      Author: s1270921
 */

/**
 * This will be a single program to produce the first sentence,
 * considering that there will be naming conflicts in my generation model
 * and this one, so I decide to write this as an individual program
 */

/**
 * change log
 * version 1.1: add line no to the end of each keywords
 * version 1.2: add rnnlm interpolation weight switch
 *
 * version 1.3: add segment constraints to filter some of the top K sentences
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include "rnnlmlib.h"
using namespace std;

#include "FirstSentenceGenerator.h"

void cmdLine(int argc, char **argv)
{
	if(argc < 9)
	{
		printf("firstSentenceGenerator <keyowrdsFile> <topK> <stackSize> <outFile> <rnnModelPath> <kenLMPath> <ShixuehanyingPath> <PingShuiYunPath> [<rnnlmWeight>=0.5]\n");
		return;
	}
	const int PATH_LENGTH = 1024;
	char keywordsFile[PATH_LENGTH];
	int topK, stackSize;
	char outFile[PATH_LENGTH];
	char rnnModelPath[PATH_LENGTH];
	char kenLMPath[PATH_LENGTH];
	char shixuehanyingPath[PATH_LENGTH];
	char pingShuiYunPath[PATH_LENGTH];
	strcpy(keywordsFile, argv[1]);
	topK = atoi(argv[2]);
	stackSize = atoi(argv[3]);
	strcpy(outFile, argv[4]);
	strcpy(rnnModelPath, argv[5]);
	strcpy(kenLMPath, argv[6]);
	strcpy(shixuehanyingPath, argv[7]);
	strcpy(pingShuiYunPath, argv[8]);
	double rnnWeight = 0.5;
	if(argc > 9)
		rnnWeight = atof(argv[9]);

	CRnnLM rnnlm;
	rnnlm.setRnnLMFile(rnnModelPath);
	rnnlm.restoreNet();
	KenLMM kenlm;
	Model model(kenLMPath);
	kenlm.setModel(&model);

	FirstSentenceGenerator fsg;
	fsg.setRNNLM(&rnnlm);
	fsg.setKenLM(&kenlm);
	fsg.loadShixuehanying(shixuehanyingPath);
	fsg.loadPingShuiYun(pingShuiYunPath);
	fsg.setRNNLMWeight(rnnWeight);

	char buf[1024];
	FILE *fin = xfopen(keywordsFile, "r", "open keywords file");
	FILE *fout = xfopen(outFile, "w", "open output file");
	int i;
	clock_t start = clock();
	int lineNo = 0;
	while(fgets(buf,sizeof(buf),fin))
	{
		char *pos = strchr(buf, '\t');
		if(pos == NULL) continue;

		lineNo ++;

		*pos = 0;
		vector<string> keywords;
		split(buf, "*", keywords);
		int senLen;
		senLen = atoi(pos + 1);
		string bufstr = buf;
		replacein(bufstr, "*", " * ");
		cout << bufstr << " " << senLen << endl;
		vector<string> topSents;
		fsg.getFirstSentence(keywords, topK, senLen, stackSize, topSents);

		// output topSents
		fprintf(fout, "<keywords>=");
		for(i = 0; i < (int)keywords.size(); i ++)
			fprintf(fout, "%s * ", keywords[i].c_str());
		fprintf(fout, "senLen = %d\t%d\n", senLen, lineNo);
		for(i = 0; i < (int)topSents.size(); i ++)
			fprintf(fout, "%s\n", topSents[i].c_str());
		fprintf(fout, "\n\n\n");
	}
	clock_t end = clock();
	cout << "time spend " << double(end-start)/CLOCKS_PER_SEC << "S" << endl;

	fclose(fin);
	fclose(fout);
}

int main(int argc, char **argv)
{
//	int _argc = 9;
//	char *_argv[] = {
//			"firstSentenceGenerator",
//			"keywords.in.tmp",
//			"300",
//			"300",
//			"firstSent.txt",
//			"/disk/scratch/Data/poem/language_model_data/RNNLM-Model/poem.all.model.hidden200.class100.txt",
//			"/afs/inf.ed.ac.uk/user/s12/s1270921/usr/kenlm/kenlm.poem.20140306.bin",
//			"/disk/scratch/Data/poem/shixuehanying/Shixuehanying.txt.cpp",
//			"/afs/inf.ed.ac.uk/user/s12/s1270921/Desktop/programming/Run/RNNPG/Data/psy_table/psy_table_cpp"
//	};

	// cout << "First Sentence Generator version 1.2 : add rnnlm interpolation weight switch" << endl;
	cout << "First Sentence Generator version 1.3 : add rnnlm interpolation weight switch; add segmentation check, debug done!" << endl;
	cmdLine(argc, argv);

	return 0;
}


