#define _CRT_SECURE_NO_WARNINGS
#include "iostream"
#include <string>
#include <conio.h>
#include <string>
#include "..\..\crypto++\SHA.h"
#include "..\..\crypto++\cryptlib.h"
#include "..\..\crypto++\modes.h"
#include "..\..\crypto++\osrng.h"
#include "..\..\crypto++\filters.h"
#include "..\..\crypto++\cbcmac.h"
#include "..\..\crypto++\base64.h"
#include "..\..\crypto++\files.h"
#include "..\..\crypto++\channels.h"
#include "..\..\crypto++\hex.h"
#include "comutil.h"
#include "..\..\Classes\MyFile.h"
#include <vector>

#pragma comment(lib,"..\\..\\cryptlib.lib")

std::vector<byte> SHA(std::vector<byte> data)
{
	MyFile hash;

	std::string message;
	for (int i = 0; i < data.size(); i++)
	{
		message += data.at(i);
	}

	std::string hashstr;
	CryptoPP::SHA512 sha512;
	CryptoPP::HashFilter filter(sha512, new CryptoPP::HexEncoder(new CryptoPP::StringSink(hashstr)));
	
	CryptoPP::ChannelSwitch cs;
	cs.AddDefaultRoute(filter);

	CryptoPP::StringSource ss(message, true, new CryptoPP::Redirector(cs));

	for (int i = 0; i < hashstr.size(); i += 2)
	{
		byte tmp = ((byte)hashstr[i] << 4) + (byte)hashstr[i + 1];
		hash.GetData().push_back(tmp);
	}
	return hash.GetData();
}

void main()
{
	MyFile plaintext;
	MyFile hash;
	plaintext.Open("..\\..\\plaintext.txt");
	hash.GetData() = SHA(plaintext.GetData());
	hash.Write("hash_sha.txt");

	system("pause");
}