#define _CRT_SECURE_NO_WARNINGS
#include "iostream"
#include <string>
#include <conio.h>
#include <string>
#include "..\..\crypto++\RSA.h"
#include "..\..\crypto++\cryptlib.h"
#include "..\..\crypto++\modes.h"
#include "..\..\crypto++\osrng.h"
#include "..\..\crypto++\filters.h"
#include "..\..\crypto++\cbcmac.h"
#include "..\..\crypto++\base64.h"
#include "..\..\crypto++\files.h"
#include "..\\..\\Classes\MyFile.h"
#include "comutil.h"
#include <vector>

#pragma comment(lib,"..\\..\\cryptlib.lib")

class RSA
{
private:
	MyFile _publicKey;
	MyFile _privateKey;
	MyFile _plaintext;
	MyFile _ciphertext;
	std::string _filename;

public:
	RSA()
	{
		CryptoPP::AutoSeededRandomPool _randPool(true);
	}
	void Encrypt()
	{
		if (!_publicKey.Open("publicKey.txt"))
		{
			return;
		}
		if (!_plaintext.Open("..\\..\\plaintext.txt"))
		{
			return;
		}
		
		std::string strpublic;
		std::string plaintext;
		std::string my_ciphertext = "";

		for (int i = 0; i < _publicKey.GetData().size(); i++)
		{
			strpublic += _publicKey.GetData().at(i);
		}

		int count = _plaintext.GetData().size() / 86;

		int k = 0;
		int st = 86;
		for (int t = 0; t < count; t++)
		{
			plaintext = "";
			for (; k < st; k++)
			{
				plaintext += _plaintext.GetData().at(k);
			}
			st += 86;

			std::string encryptText = "";
			CryptoPP::StringSource pubString(strpublic, true, new CryptoPP::Base64Decoder);
			CryptoPP::RSAES_OAEP_SHA_Encryptor Encryptor(pubString);
			CryptoPP::AutoSeededRandomPool randPool;

			CryptoPP::StringSource(plaintext, true,
				new CryptoPP::PK_EncryptorFilter(randPool, Encryptor,
					new CryptoPP::StringSink(encryptText)));
			my_ciphertext += encryptText;
		}

		plaintext = "";
		for (; k < _plaintext.GetData().size(); k++)
		{
			plaintext += _plaintext.GetData().at(k);
		}

		std::string encryptText = "";
		CryptoPP::StringSource pubString(strpublic, true, new CryptoPP::Base64Decoder);
		CryptoPP::RSAES_OAEP_SHA_Encryptor Encryptor(pubString);
		CryptoPP::AutoSeededRandomPool randPool;

		CryptoPP::StringSource(plaintext, true,
			new CryptoPP::PK_EncryptorFilter(randPool, Encryptor,
				new CryptoPP::StringSink(encryptText)));

		my_ciphertext += encryptText;
		for (int i = 0; i < my_ciphertext.size(); i++)
		{
			_ciphertext.GetData().push_back(my_ciphertext[i]);
		}
		_ciphertext.Write("ciphertext.txt");
	}

	void Decrypt()
	{
		MyFile DecryptedText;
		if (!_privateKey.Open("privateKey.txt"))
		{
			return;
		}
		if (!_ciphertext.Open("ciphertext.txt"))
		{
			return;
		}

		std::string strprivate;
		std::string ciphertext;
		std::string my_plaintext = "";

		for (int i = 0; i < _privateKey.GetData().size(); i++)
		{
			strprivate += _privateKey.GetData().at(i);
		}

		int count = _ciphertext.GetData().size() / 128;

		int k = 0;
		int st = 128;
		
		for (int t = 0; t < count; t++)
		{
			ciphertext = "";
			for (; k < st; k++)
			{
				ciphertext += _ciphertext.GetData().at(k);
			}
			st += 128;

			CryptoPP::StringSource privString(strprivate, true, new CryptoPP::Base64Decoder);
			CryptoPP::RSAES_OAEP_SHA_Decryptor Decryptor(privString);
			CryptoPP::AutoSeededRandomPool randPool;
			std::string decryptText = "";


			CryptoPP::StringSource(ciphertext, true,
				new CryptoPP::PK_DecryptorFilter(randPool, Decryptor,
					new CryptoPP::StringSink(decryptText)));
			my_plaintext += decryptText;
		}

		for (int i = 0; i < my_plaintext.size(); i++)
		{
			DecryptedText.GetData().push_back(my_plaintext[i]);
		}
		DecryptedText.Write("plaintext1.txt");
	}

	void GenerateKey()
	{
		std::string strprivkey, strpubkey;
		CryptoPP::InvertibleRSAFunction privkey;
		CryptoPP::AutoSeededRandomPool randPool;

		privkey.Initialize(randPool, 1024);

		CryptoPP::Base64Encoder privkeysink(new CryptoPP::StringSink(strprivkey), false);
		privkey.DEREncode(privkeysink);
		privkeysink.MessageEnd();

		CryptoPP::RSAFunction pubkey(privkey);

		CryptoPP::Base64Encoder pubkeysink(new CryptoPP::StringSink(strpubkey), false);
		pubkey.DEREncode(pubkeysink);
		pubkeysink.MessageEnd();

		for (int i = 0; i < strpubkey.size(); i++)
		{
			_publicKey.GetData().push_back(strpubkey[i]);
		}
		_publicKey.Write("publicKey.txt");

		for (int i = 0; i < strprivkey.size(); i++)
		{
			_privateKey.GetData().push_back(strprivkey[i]);
		}
		_privateKey.Write("privateKey.txt");
	}
};

int main()
{
	RSA rsa;
	std::cout << "1. Encrypt\n2. Decrypt\n";
	int mode;
	std::cin >> mode;
	if (mode == 1)
	{
		rsa.GenerateKey();
		rsa.Encrypt();
	}
	else
	{
		rsa.Decrypt();
	}

	system("pause");
}