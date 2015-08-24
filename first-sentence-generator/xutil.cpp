/*
 * xutil.cpp
 *
 *  Created on: 24 Dec 2013
 *      Author: s1270921
 */

#include "xutil.h"
#include <iostream>
using namespace std;

FILE *xfopen(const char *infile, const char *mode, const char *msg)
{
	FILE *f = fopen(infile, mode);
	if(f == NULL)
	{
		fprintf(stderr, "open file '%s' failed! (%s)\n", infile, msg);
		exit(1);
	}
	return f;
}

FILE *xfopen(const char *infile, const char *mode)
{
	return xfopen(infile, mode, "");
}

void* xmalloc (size_t size, const char *msg)
{
	void* p = malloc(size);
	if(p == NULL)
	{
		fprintf(stderr, "alloc memory %d failed (%s)\n", size, msg);
		exit(1);
	}
	return p;
}

void* xmalloc(size_t size)
{
	return xmalloc(size, "");
}

void *xrealloc(void *mem_address, unsigned int newsize, const char *msg)
{
	void *p = realloc(mem_address, newsize);
	if(p == NULL)
	{
		fprintf(stderr, "realloc memory %d failed (%s)\n", newsize, msg);
		exit(1);
	}
	return p;
}

void *xrealloc(void *mem_address, unsigned int newsize)
{
	return xrealloc(mem_address, newsize, "");
}

int split(const char *str, const char *sep, vector<string>& fields)
{
	fields.clear();
	string word;
	int i = 0;
	while(str[i] != '\0')
	{
		while(str[i] != '\0' && strchr(sep, str[i]))
			i ++;
		word.clear();
		while(str[i] != '\0' && strchr(sep, str[i]) == NULL)
		{
			word.append(1, str[i]);
			i ++;
		}
		if(word.length() > 0)
			fields.push_back(word);
	}

	return fields.size();
}

int split(string str, const char *sep, vector<string>& fields)
{
	const char *cstr = str.c_str();
	return split(cstr, sep, fields);
}

void skiputil(int mark, FILE *fin)
{
	int ch=0;

	while (ch!=mark) {
		ch=fgetc(fin);
		if (feof(fin)) {
			printf("Unexpected end of file (skiputil)\n");
			exit(1);
		}
	}
}

void xstrcpy(char *dst, int dstLen, const char *str)
{
	snprintf(dst, dstLen, "%s", str);
}

char *tolowerN(const char *str)
{
	char *tmp = new char[strlen(str) + 1];
	strcpy(tmp, str);
	for(char *ptr = tmp; *ptr != 0; ptr ++)
		*ptr = tolower(*ptr);

	return tmp;
}

bool atob(const char*str)
{
	char *ptr = tolowerN(str);
	bool ret = strcmp(ptr, "true") == 0;
	delete []ptr;
	return ret;
}

void printsvec(vector<string> &strs)
{
	for(size_t i = 0; i < strs.size(); i ++)
		cout << strs[i] << " -- ";
	cout << endl;
}

void replacein(std::string& subject, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
}

std::string replace(std::string subject, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
    return subject;
}

string join(const string &sep, const vector<string> &strs)
{
	string ret;
	if(strs.size() == 0)
		return ret;
	ret.append(strs[0]);
	for(int i = 1; i < strs.size(); i ++)
	{
		ret.append(sep);
		ret.append(strs[i]);
	}
	return ret;
}

