#ifndef __XCONFIG_H__
#define __XCONFIG_H__

#include <iostream>
#include <cstdio>
#include <map>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cctype>
#include <vector>
using namespace std;

class XConfig
{
public:
	static void load(const char *infile)
	{
		FILE *fin = fopen(infile, "r");
		if(fin == NULL)
		{
			fprintf(stderr, "load config file \'%s\' failed!\n", infile);
			exit(1);
		}
		char buf[1024];
		while(fgets(buf,sizeof(buf),fin))
		{
			char *commentPtr = strchr(buf, '#');
			if(commentPtr != NULL)
				*commentPtr = 0;
			// cout << buf << endl;
			char *ptr = strchr(buf, '=');
			if(ptr == NULL) continue;
			*ptr = 0;
			// int oldSize = kv.size();
			char *key = trim(buf);
			kv[key] = trim(ptr+1);
			// if((int)kv.size() > oldSize)
			keys.push_back(key);
		}

		fclose(fin);
	}
	static int getInt(const char *key)
	{
		const char *val = getStr(key);
		if(strcmp(val, "NO_SUCH_KEY"))
			return atoi(val);
		return 0;
	}
	static double getDouble(const char* key)
	{
		const char *val = getStr(key);
		if(strcmp(val, "NO_SUCH_KEY"))
			return atof(val);
		return 0;
	}
	static bool getBool(const char *key)
	{
		const char *val = getStr(key);
		if(strcmp(val, "NO_SUCH_KEY"))
		{
			char *ptr = tolowerN(val);
			bool ret = strcmp(ptr, "true") == 0;
			delete []ptr;
			return ret;
		}
		return false;
	}
	static const char* getStr(const char *key)
	{
		map<string,string>::iterator iter = kv.find(key);
		if(iter != kv.end())
			return iter->second.c_str();
		return "NO_SUCH_KEY";
	}
	static void save(const char *outfile)
	{
		FILE *fout = fopen(outfile, "w");
		map<string,string>::iterator iter;
		for(int i = 0; i < (int)keys.size(); i ++)
			fprintf(fout, "%s = %s\n", keys[i].c_str(), kv[keys[i]].c_str());
		fclose(fout);
	}
private:
	static map<string,string> kv;
	static vector<string> keys;

	static inline bool isBlank(char ch) { return ch == '\r' || ch == '\n' || ch == ' ' || ch == '\t'; }
	static char *trim(char *str)
	{
		int len = strlen(str);
		while(len > 0 && isBlank(str[len-1]))
			str[-- len] = 0;
		char *start = str;
		while(*start != 0 && isBlank(*start))
			start ++;
		return start;
	}

	static char *tolowerN(const char *str)
	{
		char *tmp = new char[strlen(str) + 1];
		strcpy(tmp, str);
		for(char *ptr = tmp; *ptr != 0; ptr ++)
			*ptr = tolower(*ptr);

		return tmp;
	}
};

map<string,string> XConfig::kv = map<string,string>();
vector<string> XConfig::keys = vector<string>();

#endif
