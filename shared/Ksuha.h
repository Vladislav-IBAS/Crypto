#include <iostream>
#include <vector>
#include <ctime>
#include <string>
#define mybyte unsigned char


std::vector<mybyte> Ksusha_hash(std::vector<mybyte> plaintext, int hash_size = 256)
{
	std::vector<mybyte> hash;
	std::vector<mybyte> iv;
	for (int i = 0; i < hash_size; i++)
	{
		char t = (25 + i) >> 2;
		iv.push_back(plaintext.at(i%plaintext.size())*t);
	}

	for (int i = 0; i < hash_size; i++)
	{
		hash.push_back((iv.at(i)) ^ (i << 5));
	}

	if (plaintext.size() > hash_size)
	{
		for (int i = 0; i < plaintext.size(); i++)
		{
			hash[i%hash_size] ^= plaintext.at(i);
		}
	}
	else
		for (int i = 0; i < hash_size; i++)
		{
			hash[i] ^= plaintext.at(i%plaintext.size());
		}
	return hash;
}
