/*
 * StringBuffer.h
 *
 *  Created on: 2014年2月16日
 *      Author: xing
 */

#ifndef STRINGBUFFER_H_
#define STRINGBUFFER_H_

#include <vector>
using namespace std;

class StringBuffer
{
public:
	static int add(const char* str)
	{
		int start = buffer.size();
		for(int i = 0; str[i] != '\0'; i ++)
			buffer.push_back(str[i]);
		buffer.push_back(0);

		return start;
	}
	static char *getRealAddr(int offset)
	{
		return &buffer[offset];
	}
private:
	static vector<char> buffer;
};

#endif /* STRINGBUFFER_H_ */
