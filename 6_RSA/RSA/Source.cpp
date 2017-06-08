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
#include "comutil.h"
#include <vector>

#pragma comment(lib,"cryptlib.lib")

class MyFile
{
private:
	std::vector<char> _data;
	FILE* _file;

public:
	MyFile()
		: _file(NULL), _data(NULL)
	{

	}

	bool Open(std::string name)
	{
		const char *_name = name.c_str();
		_file = fopen(_name, "rb");

		if (_file != NULL)
		{
			fseek(_file, 0, SEEK_END);
			int size = ftell(_file);
			rewind(_file);
			_data.resize(size);
			fread(_data.data(), 1, size, _file);
			fclose(_file);
		}
		return !_data.empty();
	}
	bool Write(std::string name)
	{
		const char *_name = name.c_str();
		_file = fopen(_name, "w+b");
		int flag = fwrite(_data.data(), 1, _data.size(), _file);
		fclose(_file);
		return !(flag == 0);
	}

	std::vector<char> &GetData() { return _data; }

};

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
		if (!_plaintext.Open("plaintext.txt"))
		{
			return;
		}
		
		std::string strpublic;
		std::string plaintext;

		for (int i = 0; i < _publicKey.GetData().size(); i++)
		{
			strpublic += _publicKey.GetData().at(i);
		}

		for (int i = 0; i < _plaintext.GetData().size(); i++)
		{
			plaintext += _plaintext.GetData().at(i);
		}

		CryptoPP::StringSource pubString(strpublic, true, new CryptoPP::Base64Decoder);
		CryptoPP::RSAES_OAEP_SHA_Encryptor Encryptor(pubString);
		CryptoPP::AutoSeededRandomPool randPool;


		std::string encryptText = "";
		CryptoPP::StringSource(plaintext, true,
			new CryptoPP::PK_EncryptorFilter(randPool, Encryptor,
				new CryptoPP::StringSink(encryptText)));

		for (int i = 0; i < encryptText.size(); i++)
		{
			_ciphertext.GetData().push_back(encryptText[i]);
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

		for (int i = 0; i < _privateKey.GetData().size(); i++)
		{
			strprivate += _privateKey.GetData().at(i);
		}

		for (int i = 0; i < _ciphertext.GetData().size(); i++)
		{
			ciphertext += _ciphertext.GetData().at(i);
		}

		CryptoPP::StringSource privString(strprivate, true, new CryptoPP::Base64Decoder);
		CryptoPP::RSAES_OAEP_SHA_Decryptor Decryptor(privString);
		CryptoPP::AutoSeededRandomPool randPool;
		std::string decryptText = "";


		CryptoPP::StringSource(ciphertext, true,
			new CryptoPP::PK_DecryptorFilter(randPool, Decryptor,
				new CryptoPP::StringSink(decryptText)));

		for (int i = 0; i < decryptText.size(); i++)
		{
			DecryptedText.GetData().push_back(decryptText[i]);
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
	rsa.GenerateKey();
	rsa.Encrypt();
	rsa.Decrypt();

	system("pause");
}