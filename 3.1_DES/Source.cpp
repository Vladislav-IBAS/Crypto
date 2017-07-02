#define _CRT_SECURE_NO_WARNINGS
#include "iostream"
#include <string>
#include <conio.h>
#include <string>
#include "..\third-party\src\crypto++\des.h"
#include "..\third-party\src\crypto++\cryptlib.h"
#include "..\third-party\src\crypto++\modes.h"
#include "..\third-party\src\crypto++\osrng.h"
#include "..\third-party\src\crypto++\filters.h"
#include "..\third-party\src\crypto++\cbcmac.h"
#include <vector>
#include "..\shared\MyFile.h"

#pragma comment(lib,"..\\third-party\\lib\\cryptlib.lib")
#pragma comment(lib,"..\\third-party\\lib\\my.lib")

class DES
{
private:
	MyFile _plaintext;
	MyFile _key;
	MyFile _iv;
	MyFile _ciphertext;
	std::string _filename;
public:
	DES()
	{

	}

	void Encrypt(int cipher_mode)
	{
		std::cout << "Enter File name or enter \"std\" to use plaintext.docx file: ";
		std::cin >> _filename;
		if (_filename == "std")
		{
			if (!_plaintext.Open("..\\resources\\plaintext.docx"))
			{
				std::cout << "Cannot open plaintext.docx file. Perhaps it doesn't exist." << std::endl;
				return;
			}
		}
		else
			if (!_plaintext.Open(_filename))
			{
				std::cout << "Cannot open source file. Perhaps it doesn't exist." << std::endl;
				return;
			}
		system("cls");

		byte key[CryptoPP::DES::DEFAULT_KEYLENGTH];
		byte iv[CryptoPP::DES::BLOCKSIZE];

		int mode;
		std::cout << "1. Generate new key and initial vector\n2. Use existing key and initial vector\n";
		std::cin >> mode;
		system("cls");
		if (mode == 1)
		{
			CryptoPP::AutoSeededRandomPool rand(true);
			std::cout << "Generating key and initial vector...\n";

			rand.GenerateBlock(key, CryptoPP::DES::DEFAULT_KEYLENGTH);
			rand.GenerateBlock(iv, CryptoPP::DES::BLOCKSIZE);

			for (int i = 0; i < CryptoPP::DES::DEFAULT_KEYLENGTH; i++)
			{
				_key.GetData().push_back(key[i]);
			}

			for (int i = 0; i < CryptoPP::DES::BLOCKSIZE; i++)
			{
				_iv.GetData().push_back(iv[i]);
			}

			_key.Write("key.txt");
			_iv.Write("iv.txt");
			std::cout << "Done!\n";
		}
		else
		{
			std::cout << "Enter name of key file: ";
			std::cin >> _filename;
			if (!_key.Open(_filename))
			{
				std::cout << "Cannot open the key file. Perhaps it doesn't exist.\n";
				return;
			}
			std::cout << "Enter name of initial vector file: ";
			std::cin >> _filename;
			if (!_iv.Open(_filename))
			{
				std::cout << "Cannot open the initial vector file. Perhaps it doesn't exist.\n";
				return;
			}
			for (int i = 0; i < CryptoPP::DES::DEFAULT_KEYLENGTH; i++)
			{
				key[i] = _key.GetData().at(i);
			}
			for (int i = 0; i < CryptoPP::DES::BLOCKSIZE; i++)
			{
				iv[i] = _iv.GetData().at(i);
			}
		}
		std::cout << "Encription... ";

		_ciphertext.GetData().resize(_plaintext.GetData().size() + CryptoPP::DES::BLOCKSIZE);
		CryptoPP::ArraySink buf(&_ciphertext.GetData()[0], _ciphertext.GetData().size());
		switch (cipher_mode)
		{
		case 1:
		{
			CryptoPP::ECB_Mode<CryptoPP::DES>::Encryption enc(key, sizeof(key));
			CryptoPP::ArraySource(_plaintext.GetData().data(), _plaintext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		case 2:
		{
			CryptoPP::CBC_Mode<CryptoPP::DES>::Encryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(_plaintext.GetData().data(), _plaintext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		case 3:
		{
			CryptoPP::CFB_Mode<CryptoPP::DES>::Encryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(_plaintext.GetData().data(), _plaintext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		case 4:
		{
			CryptoPP::OFB_Mode<CryptoPP::DES>::Encryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(_plaintext.GetData().data(), _plaintext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		case 5:
		{
			CryptoPP::CTR_Mode<CryptoPP::DES>::Encryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(_plaintext.GetData().data(), _plaintext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		}
		_ciphertext.GetData().resize(buf.TotalPutLength());
		std::cout << "The encryption is successful!" << std::endl << "Enter file name for cipher file: ";
		std::cin >> _filename;
		_ciphertext.Write(_filename);
		system("cls");
		std::cout << "File saved!" << "\n";
	}

	void Decrypt(int cipher_mode)
	{
		byte key[CryptoPP::DES::DEFAULT_KEYLENGTH];
		byte iv[CryptoPP::DES::BLOCKSIZE];
		std::cout << "Enter file name: ";
		std::cin >> _filename;
		if (!_ciphertext.Open(_filename))
		{
			std::cout << "Cannot open the coded file. Perhaps it doesn't exist.";
			return;
		}
		system("cls");
		if (!_key.Open("key.txt"))
		{
			std::cout << "Cannot open the key file. Perhaps it doesn't exist.";
			return;
		}
		if (!_iv.Open("iv.txt"))
		{
			std::cout << "Cannot open the initial vector file. Perhaps it doesn't exist.";
			return;
		}
		for (int i = 0; i < CryptoPP::DES::DEFAULT_KEYLENGTH; i++)
		{
			key[i] = _key.GetData().at(i);
		}
		for (int i = 0; i < CryptoPP::DES::BLOCKSIZE; i++)
		{
			iv[i] = _iv.GetData().at(i);
		}
		_plaintext.GetData().resize(_ciphertext.GetData().size() + CryptoPP::DES::BLOCKSIZE);
		CryptoPP::ArraySink buf(&_plaintext.GetData()[0], _plaintext.GetData().size());

		std::cout << "Decryption...";

		switch (cipher_mode)
		{
		case 1:
		{
			CryptoPP::ECB_Mode<CryptoPP::DES>::Decryption dec(key, sizeof(key));
			CryptoPP::ArraySource(_ciphertext.GetData().data(), _ciphertext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(dec, new CryptoPP::Redirector(buf)));
			break;
		}
		case 2:
		{
			CryptoPP::CBC_Mode<CryptoPP::DES>::Decryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(_ciphertext.GetData().data(), _ciphertext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		case 3:
		{
			CryptoPP::CFB_Mode<CryptoPP::DES>::Decryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(_ciphertext.GetData().data(), _ciphertext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		case 4:
		{
			CryptoPP::OFB_Mode<CryptoPP::DES>::Decryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(_ciphertext.GetData().data(), _ciphertext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		case 5:
		{
			CryptoPP::CTR_Mode<CryptoPP::DES>::Decryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(_ciphertext.GetData().data(), _ciphertext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		}
		_plaintext.GetData().resize(buf.TotalPutLength());
		std::cout << "The decryption was successful!" << std::endl << "Enter file name for decrypted file: ";
		std::cin >> _filename;
		system("cls");
		_plaintext.Write(_filename);
		std::cout << "File saved!\n";
	}

};

void main()
{
	DES des;
	int action;
	int mode;
	std::cout << "DES" << std::endl << "Choose mode:" << std::endl
		<< "1. To encrypt a file" << std::endl
		<< "2. To decrypt the file" << std::endl;
	std::cin >> action;
	system("cls");
	std::cout << "Choose block cipher mode:"
		<< std::endl << "1. Electronic code book (ECB)"
		<< std::endl << "2. Cipher block chaining (CBC)"
		<< std::endl << "3. Cipher feed back (CFB)"
		<< std::endl << "4. Output feed back (OFB)"
		<< std::endl << "5. Counter mode (CTR)" << std::endl;
	std::cin >> mode;
	system("cls");
	if (action == 1)
	{
		if (mode >= 1 && mode <= 5)
		{
			des.Encrypt(mode);
		}
		else
		{
			std::cout << "Input error\n";
		}
	}
	else if (action == 2)
	{
		if (mode >= 1 && mode <= 5)
		{
			des.Decrypt(mode);
		}
		else
		{
			std::cout << "Input error\n";
		}
	}
	system("pause");
}