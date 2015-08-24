/*
 * Vocab.h
 *
 *  Created on: Sat 21 Dec, 2013
 *      Author: Xingxing Zhang
 *
 *  The vocabulary here uses a Hashing algorithm
 *  int getStringHash(const char* wd)  hashVal = 37 * hashVal + wd[i];
 *  curPos = hashVal + j * j; (j = 1, 2, ..., n) for conflicts
 *  hash size is a prime 1000003
 */

#ifndef __VOCAB_H__
#define __VOCAB_H__

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <cassert>
#include <iostream>
using namespace std;

#include "xutil.h"

const int HASH_SIZE = 1000003;
const int WDLEN = 10;	// this is for Chinese character, so it is enough

struct Word
{
	char wd[WDLEN]; 	// this is for a Chinese character and should be enough
	int freq;
	double prob;
	int classIndex;

	void load(const char* _wd)
	{
		snprintf(wd, sizeof(wd), _wd);
		freq = 1;
		prob = 0;
		classIndex = -1;
	}

//	static bool class_cmp(const Word &w1, const Word &w2)
//	{
//		if(w1.classIndex != w2.classIndex)
//			return w1.classIndex < w2.classIndex;
//		else
//			return !(w1.freq < w2.freq);
//	}
};

int word_cmp(const void *a, const void *b);
int class_cmp(const void *a, const void *b);

class Vocab
{
public:
	Vocab(int maxVSize = 6000, int vIncSize = 1000, int hSize = HASH_SIZE)
		: vocabSize(0), maxVocabSize(maxVSize), vocabIncrement(vIncSize), hashSize(hSize)
	{
		hash = (int*)malloc(sizeof(int)*hashSize);
		memset(hash, 0xff, sizeof(int)*hashSize);	// all entries in hash table are set to -1
		vocab = (Word*)malloc(sizeof(Word)*maxVocabSize);
	}
	/**
	 * add a word to vocabulary and return the index of the word in vocabulary
	 */
	int add2Vocab(const char* wd)
	{
		int hashVal = findPos(wd);
		if(hash[hashVal] == -1)
		{
			if(vocabSize + 1 >= maxVocabSize)
			{
				maxVocabSize += vocabIncrement;
				vocab = (Word*)realloc(vocab, sizeof(Word)*maxVocabSize);
				if(vocab == NULL)
				{
					fprintf(stderr, "realloc memory for vocab failed, size = %d", maxVocabSize);
					exit(1);
				}
			}
			hash[hashVal] = vocabSize;
			vocab[vocabSize ++].load(wd);

			return vocabSize - 1;
		}

		int idx = hash[hashVal];
		vocab[idx].freq ++;

		return idx;
	}
	int getVocabSize()
	{
		return vocabSize;
	}
	/**
	 * Get the index of wd in vocabulary
	 */
	int getVocabID(const char *wd)
	{
		return hash[findPos(wd)];
	}
	/**
	 * find a place in hash table that is empty
	 */
	int findPos(const char* wd)
	{
		int curPos = getStringHash(wd);

		int i, offset;
		for(offset = 1; (i = hash[curPos]) != -1; offset += 2)
		{
			if(!strcmp(wd, vocab[i].wd))
				return curPos;
			curPos = (curPos + offset) % hashSize;
		}
		return curPos;
	}
	Word* getVocab() { return vocab; }
	void reHashVocab()
	{
		reHashVocab(0);
	}
	/**
	 * sort vocab according to their frequency
	 * rehash every entry
	 */
	void reHashVocab(int beginIndex)
	{
		qsort(vocab + beginIndex, vocabSize - beginIndex, sizeof(Word), word_cmp);
		// rehashing
		memset(hash, 0xff, sizeof(int)*hashSize);
		for(int i = 0; i < vocabSize; i ++)
			hash[findPos(vocab[i].wd)] = i;
	}
	void save(FILE *fout)
	{
		fprintf(fout, "max vocabulary size:%d\n", maxVocabSize);
		fprintf(fout, "increment size:%d\n", vocabIncrement);
		fprintf(fout, "vocabulary size:%d\n", vocabSize);
		for(int i = 0; i < vocabSize; i ++)
			fprintf(fout, "%s %d %d\n", vocab[i].wd, vocab[i].freq, vocab[i].classIndex);
		fprintf(fout, "\n\n");
	}
	void save(const char *outfile)
	{
		FILE *fout = xfopen(outfile, "wb");
		save(fout);
		fclose(fout);
	}
	void load(const char *infile)
	{
		FILE *fin = xfopen(infile, "r");
		load(fin);
		fclose(fin);
	}
	void load(FILE *fin)
	{
		skiputil(':', fin);
		int curMaxVocabSize = maxVocabSize;
		fscanf(fin, "%d", &maxVocabSize);
		skiputil(':', fin);
		fscanf(fin, "%d", &vocabIncrement);
		skiputil(':', fin);
		fscanf(fin, "%d", &vocabSize);
		if(curMaxVocabSize < maxVocabSize)
		{
			vocab = (Word*)realloc(vocab, sizeof(Word)*maxVocabSize);
			if(vocab == NULL)
			{
				fprintf(stderr, "realloc memory for vocab failed, size = %d", maxVocabSize);
				exit(1);
			}
		}
		for(int i = 0; i < vocabSize; i ++)
			fscanf(fin, "%s %d %d", vocab[i].wd, &vocab[i].freq, &vocab[i].classIndex);

		// rehashing
		memset(hash, 0xff, sizeof(int)*hashSize);
		for(int i = 0; i < vocabSize; i ++)
			hash[findPos(vocab[i].wd)] = i;
	}
	void loadVocabClass(const char *infile)
	{
		FILE *fin = xfopen(infile, "r");
		fscanf(fin, "%d", &vocabSize);
		if(vocabSize > maxVocabSize)
		{
			maxVocabSize = vocabSize;
			vocab = (Word*)realloc(vocab, sizeof(Word)*vocabSize);
			if(vocab == NULL)
			{
				fprintf(stderr, "realloc memory for vocab failed, size = %d", maxVocabSize);
				exit(1);
			}
		}
		int sClassIndex = -1;
		for(int i = 0; i < vocabSize; i ++)
		{
			fscanf(fin, "%s %d %d", vocab[i].wd, &vocab[i].freq, &vocab[i].classIndex);
			if(strcmp(vocab[i].wd, "</s>") == 0)
				sClassIndex = vocab[i].classIndex;
		}
		fclose(fin);

		assert(sClassIndex != -1);
		if(sClassIndex != 0)
		{
			for(int i = 0; i < vocabSize; i ++)
				if(vocab[i].classIndex < sClassIndex)
					vocab[i].classIndex ++;
				else if(vocab[i].classIndex == sClassIndex)
					vocab[i].classIndex = 0;
		}

		qsort(vocab, vocabSize, sizeof(Word), class_cmp);
		if(strcmp(vocab[0].wd, "</s>") != 0)
		{
			for(int i = 0; vocab[i].classIndex == 0; i ++)
				if(strcmp(vocab[i].wd, "</s>") == 0)
				{
					Word tmp = vocab[0];
					vocab[0] = vocab[i];
					vocab[i] = tmp;
					break;
				}
		}

		// rehashing
		memset(hash, 0xff, sizeof(int)*hashSize);
		for(int i = 0; i < vocabSize; i ++)
			hash[findPos(vocab[i].wd)] = i;
	}
	void getClassStartEnd(int *classStart, int *classEnd, int classSize)
	{
		int i;
		assert(vocabSize > 0);
		int minIndex = vocab[0].classIndex, maxIndex = vocab[0].classIndex;
		for(i = 1; i < vocabSize; i ++)
		{
			if(vocab[i].classIndex < minIndex)
				minIndex = vocab[i].classIndex;
			if(vocab[i].classIndex > maxIndex)
				maxIndex = vocab[i].classIndex;
		}
		assert(classSize == maxIndex - minIndex + 1);

		int lastIndex = -1;
		for(i = 0; i < vocabSize; i ++)
		{
			int index = vocab[i].classIndex;
			if(index != lastIndex)
			{
				classStart[index] = i;
				classEnd[index] = i + 1;
				lastIndex = index;
			}
			else
				classEnd[index] = i + 1;
		}
	}
	void print()
	{
		printf("this is for vocabulary size = %d\n", vocabSize);
		for(int i = 0; i < vocabSize; i ++)
			printf("%d %s %d %d\n", i, vocab[i].wd, vocab[i].freq, vocab[i].classIndex);
	}
	~Vocab()
	{
		free(hash);
		free(vocab);
	}
private:
	int hashSize;
	int *hash;
	Word* vocab;
	int vocabSize;
	int maxVocabSize;
	int vocabIncrement;



	int getStringHash(const char* wd)
	{
		unsigned int hashVal = 0;
		for(int i = 0; wd[i] != '\0'; i ++)
			hashVal = 237 * hashVal + wd[i];
		return hashVal % hashSize;
	}
};

#endif
