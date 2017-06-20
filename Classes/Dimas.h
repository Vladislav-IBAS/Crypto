#include "iostream"
#include <string>
#include <conio.h>
#include <vector>


#pragma warning(disable : 4996)
#define _CRT_NO_WARNINGS
#define mybyte unsigned char


std::vector<mybyte> Dimas_hash(std::vector<mybyte> plaintext, int block_size)
{
	std::vector<mybyte> hash_temp;
	int count_of_blocks = plaintext.size() / block_size;
	int add_bytes = plaintext.size() % block_size;
	hash_temp.resize(block_size);
	plaintext.resize(block_size*(count_of_blocks + 1));
	for (int i = count_of_blocks * block_size + add_bytes; i < (count_of_blocks + 1) * block_size; i++)
	{
		plaintext.at(i) = 0;
	}
	for (int i = 0; i < block_size; i++)
	{
		hash_temp.at(i) = plaintext.at(i);
	}

	for (int i = 1; i < count_of_blocks + 1; i++)
	{
		for (int j = 0; j < block_size; j++)
		{
			hash_temp.at(j) = hash_temp.at(j) ^ plaintext.at(i * block_size + j) ^ (hash_temp.at(j) >> (1) | hash_temp.at(j) << (7));
		}
	}

	return hash_temp;
}
