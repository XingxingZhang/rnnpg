/*
 * xutil.h
 *
 *  Created on: 24 Dec 2013
 *      Author: s1270921
 */

#ifndef XUTIL_H_
#define XUTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
using namespace std;

FILE *xfopen(const char *infile, const char *mode, const char *msg);
FILE *xfopen(const char *infile, const char *mode);

void* xmalloc (size_t size, const char *msg);
void* xmalloc (size_t size);

void *xrealloc(void *mem_address, unsigned int newsize, const char *msg);
void *xrealloc(void *mem_address, unsigned int newsize);

int split(const char *str, const char *sep, vector<string>& fields);
int split(string str, const char *sep, vector<string>& fields);

void skiputil(int ch, FILE *fin);

void xstrcpy(char *dst, int dstLen, const char *str);

char *tolowerN(const char *str);

bool atob(const char*str);

void printsvec(const vector<string> &strs);

void replacein(std::string& subject, const std::string& search,
                          const std::string& replace);

std::string replace(std::string subject, const std::string& search,
                          const std::string& replace);

string join(const string &sep, const vector<string> &strs);

void ltrim(string &str, const string &sep = " \t\r\n");

void rtrim(string &str, const string &sep = " \t\r\n");

void trim(string &str, const string &sep = " \t\r\n");

#endif /* XUTIL_H_ */
