/*
 * WordEmbedding.h
 *
 *  Created on: 24 Dec 2013
 *      Author: s1270921
 */

#ifndef WORDEMBEDDING_H_
#define WORDEMBEDDING_H_

#include <string.h>

const int WE_WDLEN = 50;		// this is not general purpose, and this is just for chinese characters

struct WdInd
{
	char word[WE_WDLEN];
	int index;
};

int wdind_cmp(const void *a, const void *b);

class WordEmbedding {
public:
	WordEmbedding();
	// load word embedding from file
	int load(const char *infile);
	/**
	 * wd : input word
	 * embeeding : the place to store word embedding (normalized)
	 * return 0 if wd is not in the vocabulary; return the length of the embedding, e.g. 100
	 */
	int getWordEmbedding(const char* wd, double *embedding);
	void getNearestWords(const char *wd);
	virtual ~WordEmbedding();
private:
	char *vocab;
	float *emMatrix;	// because in word2vec implementation they used float, we have to follow this
	long long wordSize;
	long long embeddingSize;

	WdInd *wdInds;

	void sortWords();
	int binarySearch(WdInd *wdInds, int size, const char *key);
};

#endif /* WORDEMBEDDING_H_ */
