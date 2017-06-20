#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdint.h>
#include <vector>
#include <ctime>
#include <math.h>
#include <map>
#include "..\\..\\Classes\MyFile.h"
#include "..\\..\\Classes\DES.h"
#define mybyte unsigned char

std::vector<byte> GetNextKey(std::vector<byte> inKey)
{
	std::vector<byte> outKey = inKey;
	bool flag = false;
	int i = 15;

	while (!flag)
	{
		if ((int)outKey.at(i) == 255)
		{
			outKey.at(i) = (byte)0;
			i--;
		}
		else
		{
			outKey.at(i) = (byte)((int)outKey.at(i) + 1);
			flag = true;
		}
	}
	return outKey;
}

void main()
{
	srand(time(0));
	int b = 16;
	int count = pow(2, b);
	MyFile plaintext;
	unsigned int start_time;
	unsigned int end_time;
	std::vector<mybyte> initialKey;
	std::vector<mybyte> myKey;
	std::vector<mybyte> keyL;
	std::vector<mybyte> keyH;
	std::vector<mybyte> cipher;
	std::vector<mybyte> plain;
	std::map<std::vector<mybyte>, std::vector<mybyte>> d;
	std::map<std::vector<mybyte>, std::vector<mybyte>> e;

	plaintext.Open("1.txt");
	plain = plaintext.GetData();

	byte tmp;
	for (int i = 0; i < 16; i++)
	{
		tmp = tmp = rand() % 256;
		initialKey.push_back(tmp);
	}

	int keyNum = rand() % 10000;

	myKey = initialKey;

	for (int i = 0; i < keyNum; i++)
	{
		myKey = GetNextKey(myKey);
	}

	for (int i = 0; i < 8; i++)
	{
		keyL.push_back(myKey.at(i));
	}

	for (int i = 8; i < 16; i++)
	{
		keyH.push_back(myKey.at(i));
	}

	cipher = Encrypt(Encrypt(plain, keyL), keyH);

	start_time = clock();
	std::vector<mybyte> oldKeyL;

	for (int i = 0; i < 10000; i++)
	{
		keyL.clear();
		keyH.clear();
		for (int j = 0; j < 8; j++)
		{
			keyL.push_back(initialKey.at(j));
		}
		for (int j = 8; j < 16; j++)
		{
			keyH.push_back(initialKey.at(j));
		}

		e.insert(std::pair<std::vector<mybyte>, std::vector<mybyte>>(Encrypt(plain, keyL), keyL));
		d.insert(std::pair<std::vector<mybyte>, std::vector<mybyte>>(Decrypt(cipher, keyH), keyH));
		oldKeyL = keyL;

		initialKey = GetNextKey(initialKey);
	}

	end_time = clock();
	std::cout << "Total time: " << (end_time - start_time) / (double)1000 << "sec." << std::endl;

	std::map<std::vector<mybyte>, std::vector<mybyte>>::iterator it_e;
	std::map<std::vector<mybyte>, std::vector<mybyte>>::iterator it_d;
	std::vector<mybyte> newKeyL;
	std::vector<mybyte> newKeyH;
	it_e = e.begin();
	bool find = false;
	for (int i = 0; i < e.size(); i++)
	{
		it_d = d.find((*e.begin()).first);
		newKeyL = (*it_e).second;
		newKeyH = (*it_d).second;
		it_e++;
		find = true;
		break;
	}

	plain = Decrypt(Decrypt(cipher, newKeyH), newKeyL);

	if (!find)
	{
		std::cout << "Not find" << std::endl;
	}
	else
	{
		for (int i = 0; i < plain.size(); i++)
		{
			std::cout << plain.at(i);
		}
		std::cout << std::endl;
	}

	system("pause");
}