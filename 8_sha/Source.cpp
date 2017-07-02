#define _CRT_SECURE_NO_WARNINGS
#include "iostream"
#include <string>
#include <conio.h>
#include <string>
#include "..\third-party\src\crypto++\SHA.h"
#include "..\third-party\src\crypto++\cryptlib.h"
#include "..\third-party\src\crypto++\modes.h"
#include "..\third-party\src\crypto++\osrng.h"
#include "..\third-party\src\crypto++\filters.h"
#include "..\third-party\src\crypto++\cbcmac.h"
#include "..\third-party\src\crypto++\base64.h"
#include "..\third-party\src\crypto++\files.h"
#include "..\third-party\src\crypto++\channels.h"
#include "..\third-party\src\crypto++\hex.h"
#include "comutil.h"
#include <vector>
#include "..\shared\MyFile.h"

#pragma comment(lib,"..\\third-party\\lib\\cryptlib.lib")
#pragma comment(lib,"..\\third-party\\lib\\my.lib")

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
	plaintext.Open("..\\resources\\plaintext.txt");
	hash.GetData() = SHA(plaintext.GetData());
	hash.Write("hash_sha.txt");

	system("pause");
}