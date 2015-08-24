/*
 * TranslationTable.h
 *
 *  Created on: 2014年2月16日
 *      Author: xing
 */

#ifndef TRANSLATIONTABLE_H_
#define TRANSLATIONTABLE_H_

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstring>
#include <utility>
#include <map>
#include <fstream>
#include <cassert>
using namespace std;

#include "xutil.h"
#include "StringBuffer.h"

struct TransList
{
	int word;
	int total;
	vector<pair<int,int> > tlist;

	void sortList()
	{
		sort(tlist.begin(), tlist.end(), TransList::intstr_cmp);
	}

	static bool intstr_cmp(const pair<int,int> &a, const pair<int,int> &b)
	{
		return strcmp(StringBuffer::getRealAddr(a.first), StringBuffer::getRealAddr(b.first)) < 0;
	}

	int find(const char* second)
	{
		int begin = 0, end = tlist.size() - 1;
		while(begin <= end)
		{
			int mid = (begin + end) >> 1;
			int x = strcmp(second, StringBuffer::getRealAddr(tlist[mid].first));
			if(x < 0)
				end = mid - 1;
			else if(x > 0)
				begin = mid + 1;
			else
				return mid;
		}

		return -1;
	}

	double getProb(const char* second)
	{
		int pos = find(second);
		if(pos == -1)
			return 0;
		else
			return (double)tlist[pos].second / total;
	}

	void print()
	{
		cout << "word = " << word << endl;
		cout << "table = ";
		for(int i = 0; i < (int)tlist.size(); i ++)
			cout << StringBuffer::getRealAddr(tlist[i].first) << " ";
		cout << endl;
	}
};

class TranslationTable
{
public:
	void load(const char* infile)
	{
		ifstream fin(infile);
		if(!fin.is_open())
		{
			fprintf(stderr, "open file %s failed!\n", infile);
			exit(1);
		}
		string buf;
		fin >> MAX_NGRAM;
		int i, j;
		for(i = 0; i < MAX_NGRAM; i ++)
		{
			vector<TransList*> transTbl;
			transTbls.push_back(transTbl);
			invertedTransTbls.push_back(transTbl);
		}
		fin.ignore();
		vector<string> fields;
		vector<string> words;
		for(i = 0; i < MAX_NGRAM; i ++)
		{
			while(getline(fin, buf))
			{
				if(buf == "@@@@@@@@")
					break;
				split(buf, "\t", fields);
				if(fields.size() != 3)
				{
					fprintf(stderr, "invalid line: %s\n", buf.c_str());
					continue;
				}
				TransList *transList = new TransList;
				transList->word = add2buf(fields[0]);
				transList->total = atoi(fields[1].c_str());
				split(fields[2], "*", words);
				for(j = 0; j < (int)words.size(); j ++)
				{
					// char word[128];
					string word;
					int freq;
					// sscanf(words[j].c_str(), "%s_%d", word, &freq);
					int pos = words[j].rfind('_');
					word = words[j].substr(0, pos);
					freq = atoi(words[j].substr(pos + 1).c_str());
					transList->tlist.push_back(make_pair(add2buf(word), freq));
				}
				transTbls[i].push_back(transList);
			}
		}
		// delete str2idx;
		cout << "load trnaslation table done" << endl;

		for(i = 0; i < MAX_NGRAM; i ++)
		{
			for(j = 0; j < (int)transTbls[i].size(); j ++)
				transTbls[i][j]->sortList();

			sort(transTbls[i].begin(), transTbls[i].end(), TranslationTable::trans_cmp);
		}

		cout << "sort translation table done" << endl;

		// build inverted translation table
		map<char*,int, cmp_str> *str2idx = new map<char*,int, cmp_str>();
		map<char*,int, cmp_str>::iterator iter;
		int k;
		for(i = 0; i < MAX_NGRAM; i ++)
		{
			str2idx->clear();
			for(j = 0; j < (int)transTbls[i].size(); j ++)
			{
				TransList *transList = transTbls[i][j];
				int firstIndex = transList->word;
				char *first = NULL;
				first = StringBuffer::getRealAddr(firstIndex);
				char *second = NULL;
				for(k = 0; k < (int)transList->tlist.size(); k ++)
				{
					int secIndex = transList->tlist[k].first;
					int freq = transList->tlist[k].second;
					second = StringBuffer::getRealAddr(secIndex);
					iter = str2idx->find(second);
					if(iter == str2idx->end())
					{
						TransList *newTList = new TransList;
						newTList->total = freq;
						newTList->word = secIndex;
						newTList->tlist.push_back(make_pair(firstIndex, freq));

						// str2idx->insert(map<char*,int, cmp_str>::value_type(second, invertedTransTbls[i].size()));
						(*str2idx)[second] = invertedTransTbls[i].size();
						invertedTransTbls[i].push_back(newTList);
					}
					else
					{
						invertedTransTbls[i][iter->second]->total += freq;
						invertedTransTbls[i][iter->second]->tlist.push_back(make_pair(firstIndex, freq));
					}
				}
			}

			for(j = 0; j < (int)invertedTransTbls[i].size(); j ++)
				invertedTransTbls[i][j]->sortList();
			sort(invertedTransTbls[i].begin(), invertedTransTbls[i].end(), TranslationTable::trans_cmp);
		}
		delete str2idx;
		cout << "loading done!" << endl;
	}

	double getProb(const char* first, const char* second)
	{
		int len = getPhraseLen(first);

		assert(len <= MAX_NGRAM);
		int pos = find(transTbls[len-1], first);
		if(pos == -1)
			return 0;
		return transTbls[len-1][pos]->getProb(second);
	}

	double getProbInverted(const char* first, const char* second)
	{
		int len = getPhraseLen(first);

		assert(len <= MAX_NGRAM);
		int pos = find(invertedTransTbls[len - 1], first);
		if(pos == -1)
			return 0;
		return invertedTransTbls[len - 1][pos]->getProb(second);
	}

	void getAllTrans(const char *first, vector<pair<char*,double> > &trans)
	{
		trans.clear();
		int len = getPhraseLen(first);

		assert(len <= MAX_NGRAM);
		int pos = find(transTbls[len - 1], first);
		if(pos == -1)
			return;
		TransList *trList = transTbls[len - 1][pos];
		for(int i = 0; i < (int)trList->tlist.size(); i ++)
		{
			char *phrase = StringBuffer::getRealAddr(trList->tlist[i].first);
			double prob = (double)trList->tlist[i].second / trList->total;
			trans.push_back(make_pair(phrase, prob));
		}
	}

	int getMaxNGram() { return MAX_NGRAM; }

	void print()
	{
		cout << "translation table" << endl;
		print(transTbls);

		cout << "inverted translation table" << endl;
		print(invertedTransTbls);
	}

	~TranslationTable()
	{
		for(int i = 0; i < MAX_NGRAM; i ++)
		{
			int j;
			for(j = 0; j < (int)transTbls[i].size(); j ++)
				if(transTbls[i][j] != NULL)
					delete transTbls[i][j];

			for(j = 0; j < (int)invertedTransTbls[i].size(); j ++)
				if(invertedTransTbls[i][j] != NULL)
					delete invertedTransTbls[i][j];
		}
	}
private:
	int MAX_NGRAM;
	vector<vector<TransList*> > transTbls;
	vector<vector<TransList*> > invertedTransTbls;

	struct cmp_str
	{
	   bool operator()(char const *a, char const *b) const
	   {
	      return std::strcmp(a, b) < 0;
	   }
	};

	void print(const vector<vector<TransList*> > &_transTbls)
	{
		for(int i = 0; i < MAX_NGRAM; i ++)
		{
			for(int j = 0; j < (int)_transTbls[i].size(); j ++)
			{
				cout << tostr(_transTbls[i][j]->word) << "\t" << _transTbls[i][j]->total << "\t";
				for(int k = 0; k < (int)_transTbls[i][j]->tlist.size(); k ++)
					cout << tostr(_transTbls[i][j]->tlist[k].first) << "-" << _transTbls[i][j]->tlist[k].second << "*";
				cout << endl;
			}
			cout << "@@@@" << endl;
		}
	}


	int add2buf(string str)
	{
		return add2buf(str.c_str());
	}
	int add2buf(const char* str)
	{
		return StringBuffer::add(str);
	}
	char *tostr(int offset)
	{
		return StringBuffer::getRealAddr(offset);
	}
	static bool trans_cmp(TransList *a, TransList *b)
	{
		return strcmp(StringBuffer::getRealAddr(a->word), StringBuffer::getRealAddr(b->word)) < 0;
	}

	int find(const vector<TransList*> &transTbl, const char* first)
	{
		int begin = 0, end = transTbl.size() - 1;
		while(begin <= end)
		{
			int mid = (begin + end) >> 1;
			int x = strcmp(first, StringBuffer::getRealAddr(transTbl[mid]->word));
			if(x < 0)
				end = mid - 1;
			else if(x > 0)
				begin = mid + 1;
			else
				return mid;
		}

		return -1;
	}

	int getPhraseLen(const char*phr)
	{
		int cnt = 0;
		for(int i = 0; phr[i] != '\0'; i ++)
			if(phr[i] == ' ')
				cnt ++;
		return cnt + 1;
	}
};

#endif /* TRANSLATIONTABLE_H_ */
