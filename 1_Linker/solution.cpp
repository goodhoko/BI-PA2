#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <stdint.h>
using namespace std;
#endif /* __PROGTEST__ */

struct symbol
{
	int32_t nameLen;
	int32_t dataLen;
	char *name;
	char *data;
	string nameStr;
};

inline bool operator<(const symbol &left, const symbol &right)
{
	/*cout << "porovnavam:\n" << left.name << "\n" << right.name << endl;
	int res = left.nameStr < right.nameStr;
	cout << "a vracim " << res << "\n" << endl;*/

	return left.nameStr < right.nameStr;
}

void printSymbol(const symbol a)
{
	cout << "Symbol:" << endl;
	cout << "nameLen:" << a.nameLen << " dataLen:" << a.dataLen << endl;
	cout << "name: " << a.name << "\n" << endl;
}

bool linkFiles ( const char  * srcFile1, const char  * srcFile2, const char  * dstFile )
{
	int32_t leftCount = 0, rightCount = 0,  dataPos = 4;
 	set<symbol> buffer;


 	ifstream left(srcFile1, ios::binary);
 	if (!left.is_open())
 	{
 		return false;
 	}

 	ifstream right(srcFile2, ios::binary);	
 	if (!left.is_open())
 	{
 		left.close();
 		return false;
 	}

 	left.read((char*) &leftCount, sizeof(int32_t));
 	right.read((char*) &rightCount, sizeof(int32_t));

 	if ((!leftCount && !rightCount) || !left.good() || !right.good())
 	{
 		left.close();
 		right.close();
 		return false;
 	}

 	for (int i = 0; i < leftCount; ++i)
 	{
 		symbol tmp;
 		int32_t tmpOffset, tmpPos;
 		left.read((char*) &tmp.nameLen, sizeof(int32_t)); //reading symbol numbers
 		left.read((char*) &tmp.dataLen, sizeof(int32_t));
 		left.read((char*) &tmpOffset, sizeof(int32_t));
 		if (!left.good() || tmp.nameLen < 0 || tmp.dataLen < 0 || tmpOffset < 0)
 			return false;

 		tmp.name = new char[tmp.nameLen + 1];	//allocating space for name
 		left.read((char*) tmp.name, tmp.nameLen); //reading name
 		if (!left.good())
 			return false;
 		tmp.name[tmp.nameLen] = '\0';	//apending \0 to the end of the name
 		tmp.nameStr.assign(tmp.name);

 		tmpPos = left.tellg();	//saving current position
 		left.seekg(tmpOffset);	//moving to the data block
 		if (!left.good())
 			return false;

 		tmp.data = new char[tmp.dataLen]; 	//allocating space for data
 		left.read((char*) tmp.data, tmp.dataLen);	//reading data
 		if (!left.good())
 			return false;
 		//cout << "firstfile" << endl;
 		//printSymbol(tmp);

 		auto res = buffer.insert(tmp);	//insertint into buffer
 		if (res.second == false)
 			return false;

 		left.seekg(tmpPos);	//returning to the list of symbols
 		if (!left.good())
 			return false;
 	}

 	for (int i = 0; i < rightCount; ++i)
 	{
 		symbol tmp;
 		int32_t tmpOffset, tmpPos;
 		right.read((char*) &tmp.nameLen, sizeof(int32_t)); //reading symbol numbers
 		right.read((char*) &tmp.dataLen, sizeof(int32_t));
 		right.read((char*) &tmpOffset, sizeof(int32_t));
 		if (!right.good() || tmp.nameLen < 0 || tmp.dataLen < 0 || tmpOffset < 0)
 			return false;

 		tmp.name = new char[tmp.nameLen + 1];	//allocating space for name
 		right.read((char*) tmp.name, tmp.nameLen); //reading name
 		if (!right.good())
 			return false;
 		tmp.name[tmp.nameLen] = '\0';	//apending \0 to the end of the name
 		tmp.nameStr.assign(tmp.name);

 		tmpPos = right.tellg();	//saving current position
 		right.seekg(tmpOffset);	//moving to the data block
 		if (!right.good())
 			return false;

 		tmp.data = new char[tmp.dataLen]; 	//allocating space for data
 		right.read((char*) tmp.data, tmp.dataLen);	//reading data
 		if (!right.good())
 			return false;
 		//cout << "second file" << endl;
 		//printSymbol(tmp);
 		
 		auto res = buffer.insert(tmp);	//insertint into buffer
 		if (res.second == false)
 			return false;

 		right.seekg(tmpPos);	//returning to the list of symbols
 		if (!right.good())
 			return false;
 	}

 	left.close();
 	right.close();

 	int32_t count = leftCount + rightCount;

 	ofstream file(dstFile, ios::trunc | ios::binary);
 	if (!file.is_open())
 		return false;

 	file.write((char*)&count, sizeof(int32_t));
 	if (!file.good())
 	{
 		file.close();
 		return false;
 	}

 	//getting start of the data block
 	for (set<symbol>::iterator i = buffer.begin(); i != buffer.end(); i++)
 	{
  		symbol a = *i;
  		dataPos += 3*sizeof(int32_t) + a.nameLen;
  		//printSymbol(a);
	}

	//writing
	for (set<symbol>::iterator i = buffer.begin(); i != buffer.end(); i++)
 	{
 		int32_t tmpPos;
  		symbol a = *i;

  		file.write((char*)&a.nameLen, 4);
  		file.write((char*)&a.dataLen, 4);
		file.write((char*)&dataPos, 4);
		file.write((char*)a.name, a.nameLen);
		if(file.fail())
		{
			file.close();
			return false;
		}

		tmpPos = file.tellp();
		file.seekp(dataPos);
		file.write((char*)a.data, a.dataLen);
		if(file.fail())
		{
			file.close();
			return false;
		}
		dataPos += a.dataLen;

		file.seekp(tmpPos);  		
	}

	file.close();

 	return true;

}

#ifndef __PROGTEST__
bool identicalContents ( const char * fileName1, const char * fileName2 )
{
	ifstream file1(fileName1, ios::binary | ios::ate);
	ifstream file2(fileName2, ios::binary | ios::ate);
	if (!file1.is_open() || !file2.is_open())
	{
		//cout << "Can't open one of the files!" << endl;
		return false;
		file1.close();
		file2.close();
	}

	if (file1.tellg() != file2.tellg())
	{
		//cout << "File sizes doesn't match!" << endl;
		//cout << fileName1 << " size is " << file1.tellg() << " bytes" << endl;
		//cout << fileName2 << " size is " << file2.tellg() << " bytes" << endl;
		file1.close();
		file2.close();
		return false;
	}

	size_t size = file1.tellg();
	char *buff1, *buff2;
	buff1 = new char[size];
	buff2 = new char[size];

	file1.seekg(0, ios::beg);
	file2.seekg(0, ios::beg);
	file1.read(buff1, size);
	file2.read(buff2, size);

	if (memcmp(buff1, buff2, size))
	{
		//cout << "Content is different\n// memcmp returns " << memcmp(buff1, buff2, size) << endl;
		file1.close();
		file2.close();
		return false;
	}

	file1.close();
	file2.close();

  	return true;
}

int main ( void )
{
  	//assert ( linkFiles ( "in1_00.obj", "in2_00.obj", "out_00.obj" ) && identicalContents ( "out_00.obj", "ref_00.obj" ) );

 	//assert ( linkFiles ( "in1_01.obj", "in2_01.obj", "out_01.obj" ) && identicalContents ( "out_01.obj", "ref_01.obj" ) );

 	assert ( !linkFiles ( "in1_02.obj", "in2_02.obj", "out_02.obj" ) );

  	return 0;
}

#endif /* __PROGTEST__ */
