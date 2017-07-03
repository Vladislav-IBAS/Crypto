#define _CRT_SECURE_NO_WARNINGS
#define mybyte unsigned char
#include <iostream>
#include <vector>
#include <math.h>
#include "..\shared\MyFile.h"

std::vector<mybyte> My_hash(std::vector<mybyte> data, int size = 64)
{
	MyFile hash;
	int lenght;
	mybyte a = 13;
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
