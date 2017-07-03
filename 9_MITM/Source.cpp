#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdint.h>
#include <vector>
#include <ctime>
#include <math.h>
#include <map>
#include "..\shared\MyFile.h"
#include "..\shared\DES.h"

#pragma comment(lib,"..\\third-party\\lib\\my.lib")

std::vector<mybyte> GetNextKey(std::vector<mybyte> inKey)
{
	std::vector<mybyte> outKey = inKey;
	bool flag = false;
	int i = 15;

	while (!flag)
	{
		if ((int)outKey.at(i) == 255)
		{
			outKey.at(i) = (mybyte)0;
			i--;
		}
		else
		{
			outKey.at(i) = (mybyte)((int)outKey.at(i) + 1);
			flag = true;
		}
	}
	return outKey;
}

std::vector<mybyte> AddKey(std::vector<mybyte> Key, int n)
{
	std::vector<mybyte> res = Key;
	for (int i = 0; i < n; i++)
	{
		res = GetNextKey(res);
	}
	return res;
}

void Search(std::map<std::vector<mybyte>, std::vector<mybyte>> &d, std::map<std::vector<mybyte>, std::vector<mybyte>> &e,
	std::vector<mybyte> initialKey, std::vector<mybyte> cipher, std::vector<mybyte> plain)
{
	std::vector<mybyte> keyL;
	std::vector<mybyte> keyH;

	int n = pow(2, 13);

	for (int i = 0; i < n; i++)
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
		initialKey = GetNextKey(initialKey);
	}
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

	std::vector<mybyte> k1;
	std::vector<mybyte> k2;
	std::vector<mybyte> k3;
	std::vector<mybyte> k4;
	std::vector<mybyte> k5;
	std::vector<mybyte> k6;
	std::vector<mybyte> k7;
	std::vector<mybyte> k8;

	std::map<std::vector<mybyte>, std::vector<mybyte>> d;
	std::map<std::vector<mybyte>, std::vector<mybyte>> e;

	plaintext.Open("..\\resources\\1.txt");
	plain = plaintext.GetData();

	byte tmp;
	for (int i = 0; i < 16; i++)
	{
		tmp = tmp = rand() % 256;
		initialKey.push_back(tmp);
	}

	int keyNum = rand() % count;

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

	int n = pow(2, 13);

	std::cout << "Creating tables...\n";
	start_time = clock();

	k1 = initialKey;
	std::thread t1(Search, std::ref(d), std::ref(e), k1, cipher, plain);

	k2 = AddKey(k1, n);
	std::thread t2(Search, std::ref(d), std::ref(e), k2, cipher, plain);

	k3 = AddKey(k2, n);
	std::thread t3(Search, std::ref(d), std::ref(e), k3, cipher, plain);

	k4 = AddKey(k3, n);
	std::thread t4(Search, std::ref(d), std::ref(e), k4, cipher, plain);

	k5 = AddKey(k4, n);
	std::thread t5(Search, std::ref(d), std::ref(e), k5, cipher, plain);

	k6 = AddKey(k5, n);
	std::thread t6(Search, std::ref(d), std::ref(e), k6, cipher, plain);

	k7 = AddKey(k6, n);
	std::thread t7(Search, std::ref(d), std::ref(e), k7, cipher, plain);

	k8 = AddKey(k7, n);
	std::thread t8(Search, std::ref(d), std::ref(e), k8, cipher, plain);
	t8.join();

	end_time = clock();
	std::cout << "Done: " << (end_time - start_time) / (double)1000 << "sec." << std::endl;

	std::vector<mybyte> newKeyL;
	std::vector<mybyte> newKeyH;
	bool find = false;

	for (int i = 0; i < e.size(); i++)
	{
		newKeyL = (*e.begin()).second;
		newKeyH = (*d.find((*e.begin()).first)).second;
	}

	std::vector<mybyte> newPlain;
	newPlain = Decrypt(Decrypt(cipher, newKeyH), newKeyL);
	for (int i = 0; i < plain.size(); i++)
	{
		std::cout << newPlain.at(i);
	}
	std::cout << std::endl;

	system("pause");
}