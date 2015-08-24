/*
 * WordEmbedding.cpp
 *
 *  Created on: 24 Dec 2013
 *      Author: s1270921
 */

#include "WordEmbedding.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
using namespace std;
#include "xutil.h"

int wdind_cmp(const void *a, const void *b)
{
	return strcmp(((WdInd*)a)->word, ((WdInd*)b)->word);
}

WordEmbedding::WordEmbedding() {

	vocab = NULL;
	emMatrix = NULL;
	wordSize = 0;
	embeddingSize = 0;
	wdInds = NULL;
}

int WordEmbedding::load(const char *infile)
{
	FILE *fin = xfopen(infile, "rb", "load word embedding");
	fscanf(fin, "%lld", &wordSize);
	fscanf(fin, "%lld", &embeddingSize);
	vocab = (char*)xmalloc(wordSize * WE_WDLEN * sizeof(char));
	emMatrix = (float*)xmalloc(wordSize * embeddingSize * sizeof(float));
	char ch = 0;
	int i, j;
	float sum = 0;
	for( i = 0; i < wordSize; i ++)
	{
		fscanf(fin, "%s%c", &vocab[i * WE_WDLEN], &ch);
		sum = 0;
		for( j = 0; j < embeddingSize; j ++ )
		{
			fread(&emMatrix[j + i * embeddingSize], sizeof(float), 1, fin);
			sum += emMatrix[j + i * embeddingSize] * emMatrix[j + i * embeddingSize];
		}
		sum = sqrt(sum);
		for( j = 0; j < embeddingSize; j ++ )
			emMatrix[j + i * embeddingSize] /= sum;
	}
	fclose(fin);

	sortWords();

	return 1;
}

void WordEmbedding::sortWords()
{
	wdInds = (WdInd*)xmalloc(wordSize * sizeof(WdInd));
	for(int i = 0; i < wordSize; i ++)
	{
		snprintf(wdInds[i].word, sizeof(wdInds[i].word), "%s", &vocab[i * WE_WDLEN]);
		wdInds[i].index = i;
	}
	qsort(wdInds, wordSize, sizeof(WdInd), wdind_cmp);

//	for(int i = 0; i < wordSize; i ++)
//		cout << wdInds[i].word << " " << wdInds[i].index << endl;
}

int WordEmbedding::binarySearch(WdInd *wdInds, int size, const char *key)
{
	int begin = 0, end = size - 1, mid, x;
	while(begin <= end)
	{
		mid = (begin + end) >> 1;
		x = strcmp(key, wdInds[mid].word);
		if(x < 0)
			end = mid - 1;
		else if(x > 0)
			begin = mid + 1;
		else
			return mid;
	}
	return -1;
}

int WordEmbedding::getWordEmbedding(const char *wd, double *embedding)
{
	int ind = binarySearch(wdInds, wordSize, wd);
	if(ind == -1) return 0;
	int offset = wdInds[ind].index * embeddingSize;
	for(int i = 0; i < embeddingSize; i ++)
		embedding[i] = emMatrix[i + offset];
	return embeddingSize;
}

struct Nd
{
	char word[WE_WDLEN];
	double score;
};

int nd_cmp(const void *a, const void *b)
{
	if(((Nd*)a)->score > ((Nd*)b)->score)
		return -1;
	else if(((Nd*)a)->score < ((Nd*)b)->score)
		return 1;
	else
		return 0;
}

void WordEmbedding::getNearestWords(const char *wd)
{
	int ind = binarySearch(wdInds, wordSize, wd);
	if(ind == -1) return;
	ind = wdInds[ind].index;
	Nd *nList;
	nList = new Nd[wordSize];
	int i, j, k;
	for(i = 0; i < wordSize; i ++)
	{
		strcpy(nList[i].word, &vocab[i*WE_WDLEN]);
		double sim = 0;
		double sum1 = 0, sum2 = 0, dot = 0;
		for(j = 0; j < embeddingSize; j ++)
		{
			double a = emMatrix[j + ind * embeddingSize];
			double b = emMatrix[j + i * embeddingSize];
			dot += a * b;
			sum1 += a * a;
			sum2 += b * b;
		}
		nList[i].score = dot / sqrt(sum1) / sqrt(sum2);
	}

	qsort(nList, wordSize, sizeof(Nd), nd_cmp);
	int N = 50;
	for(i = 0; i < N; i ++)
		cout << nList[i].word << " " << nList[i].score << endl;

	delete []nList;
}

WordEmbedding::~WordEmbedding() {

	if(vocab) free(vocab);
	if(emMatrix) free(emMatrix);
	if(wdInds) free(wdInds);
}

