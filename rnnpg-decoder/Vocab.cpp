/*
 * Vocab.cpp
 *
 *  Created on: 27 Dec 2013
 *      Author: s1270921
 */

#include "Vocab.h"

int word_cmp(const void *a, const void *b)
{
	return ((Word*)b)->freq - ((Word*)a)->freq;
}

int class_cmp(const void *a, const void *b)
{
	Word *wa = (Word*)a;
	Word *wb = (Word*)b;
	if(wa->classIndex != wb->classIndex)
		return wa->classIndex - wb->classIndex;
	else
		return wb->freq - wa->freq;
}
