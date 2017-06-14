#define _CRT_SECURE_NO_WARNINGS
#define byte unsigned char
#include <iostream>
#include <vector>
#include <math.h>
#include "..\..\Classes\MyFile.h"

std::vector<byte> My_hash(std::vector<byte> data, int size = 256)
{
	MyFile hash;
	int lenght;
	byte a = 13;
	for (int i = 0; i < size; i++)
	{
		hash.GetData().push_back(data.at(i % data.size()) << (a % 8));
		a ^= data.at((i + int(a ^ i)) % data.size());
	}

	if (data.size() > size)
	{
		lenght = data.size();
	}
	else
	{
		lenght = size;
	}
	a = 21;
	for (int i = 0; i < lenght; i++)
	{
		hash.GetData().at(i % size) ^= (data.at((i + a) % data.size()) ^ a);
		a ^= (hash.GetData().at((i + int(a ^ i)) % hash.GetData().size()) ^ ((a + i) % 256));
	}
	return hash.GetData();
}

void main()
{
	MyFile plaintext;
	MyFile hash;
	plaintext.Open("..\\..\\plaintext.txt");
	hash.GetData() = My_hash(plaintext.GetData(), 64);
	hash.Write("my_hash.txt");
	system("pause");
}