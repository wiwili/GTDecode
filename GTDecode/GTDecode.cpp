// GTDecode.cpp
// Quick project to decode Gemtrader's files
// Decode method pulled with IDA
#include "pch.h"
//#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
//#include <crtdbg.h>
#include <iostream>
#include <fstream>
#include<windows.h>
#include <string>
typedef unsigned char BYTE;
using namespace std;

#define __PAIR__(high, low) (((unsigned long)(high)<<sizeof(high)*8) | low)

char *__fastcall decodeXOR(unsigned __int8 * a2, int a3);
int __fastcall char2uint(unsigned __int8 *a2, int a3);

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		cout << "Usage: GTDecode.exe <inputs> \nOutputs will be created in the output folder \n";
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{
			// Get path and file name for directory creation
			char drive[_MAX_DRIVE];
			char dir[_MAX_DIR];
			char fname[_MAX_FNAME];
			char ext[_MAX_EXT];


			_splitpath_s(argv[i], drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
			string outputPath = string(drive) + string(dir) + "output\\";

			wstring stemp = wstring(outputPath.begin(), outputPath.end());

			CreateDirectoryW(stemp.c_str(), NULL);

			// Get file input
			char* memBlock = NULL;
			int blockSize = 0;

			ifstream file(argv[i], ios::in | ios::binary | ios::ate);
			if (file.is_open()) {
				blockSize = file.tellg();
				memBlock = new char[blockSize];
				file.seekg(0, ios::beg);
				file.read((char*)memBlock, blockSize);
				file.close();
			}

			// Decode file
			char *result = decodeXOR((unsigned char*)memBlock, blockSize);

			// Write output
			string output = "output\\" + string(fname) + string(ext);
			
			if (result != 0)
			{
				ofstream fout;
				fout.open(output, ios::binary | ios::out | ios::app);
				fout.write(reinterpret_cast<char*>(result), blockSize-20);
				fout.close();
				cout << "Created: " + output << endl;
			}

			// Free memory
			free(result);
			delete[] memBlock;
		}
	}
	//_CrtDumpMemoryLeaks();
	system("pause");
}

// cocos2d::FileUtils::decodeXOR from libcocos2dlua.so
// Decodes the input buffer
char *__fastcall decodeXOR(unsigned __int8 * a2, int a3)
{
	int v4 = a3;
	unsigned char* v7 = a2;
	if (a3 <= 19)
	{
		cout << "Invalid file input" << endl;
		return 0;
	}
	unsigned int v8 = memcmp("gemtrade", a2, 8u);
	if (v8)
	{
		cout << "Invalid file input (not a gemtrader file)" << endl;
		return 0;
	}
	int v9 = char2uint(v7, 8);
	int v10 = char2uint(v7, 12);
	int v11 = char2uint(v7, 16);
	unsigned int v12 = (unsigned int)(v11 + v10) >> 1;
	unsigned int v13 = (unsigned int)(v10 - v11) >> 1;
	size_t v14 = v4 - 20;
	int v15 = v9 - v12 * v13;
	char* v16 = (char *)malloc(v14);
	memcpy(v16, v7 + 20, v14);
	while (1)
	{
		int v17 = char2uint((unsigned __int8 *)v16, v8);
		short v18 = HIWORD(v17) ^ HIWORD(v15);
		v16[v8] = v17 ^ v15;
		int v19 = (int)&v16[v8];
		v8 += 4;
		unsigned int v20 = (v17 ^ (unsigned int)v15) >> 8;
		unsigned int v21 = v14 - v8;
		--v15;
		*(unsigned char*)(v19 + 1) = v20;
		*(unsigned short*)(v19 + 2) = __PAIR__(HIBYTE(v18), (unsigned __int8)v18);
		if (v14 <= v8 || v21 <= 3) {
			break;
		}
		if (v12 <= v8 && v12 < v21)
		{
			v8 += v13;
			if (v14 <v8 || v12 > v14 - v8)
				v8 = v14 - v12;
		}
	}
	return v16;
}

// cocos2d::FileUtils::char2uint
int __fastcall char2uint(unsigned __int8 *a2, int a3)
{
	return (a2[a3 + 2] << 16) | (a2[a3 + 3] << 24) | a2[a3] | (a2[a3 + 1] << 8);
}
