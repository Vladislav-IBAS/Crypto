#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <locale>
#include <string>
#include <vector>
#include "..\shared\MyFile.h"

#pragma comment(lib,"..\\third-party\\lib\\my.lib")

class OTP
{
private:
	MyFile _plaintext;
	MyFile _key;
	MyFile _ciphertext;

	void createKey()
	{
		srand(time(NULL));
		std::vector<char> data;
		FILE* key = fopen("key.txt", "wb");
		FILE* plaintext;
		if (!(plaintext = fopen("..\\resources\\plaintext.txt", "rb")))
			return;

		int size = 0;
		fseek(plaintext, 0, SEEK_END);
		size = ftell(plaintext);
		fseek(plaintext, 0, SEEK_SET);
		for (int i = 0; i < size; i++)
		{
			char c = rand() % 256;
			data.push_back(c);
		}
		fwrite(data.data(), data.size(), 1, key);
		fclose(key);
		fclose(plaintext);
	}

public:
	void Decrypt()
	{
		if (!_ciphertext.Open("ciphertext.txt"))
		{
			std::cout << "Cannot open the coded file. Perhaps it doesn't exist." << std::endl;
			return;
		}

		if (!_key.Open("key.txt"))
		{
			std::cout << "Cannot open key file. Perhaps it doesn't exist." << std::endl;
			return;
		}

		if (_ciphertext.GetData().size() != _key.GetData().size())
		{
			std::cout << "The size of encrypted file and key file do not match." <<
				std::endl << "It is impossible to decrypt the file." << std::endl;
			return;
		}
		else
		{
			std::cout << "Decryption..." << std::endl;
			int i = 0;
			for (; i < _ciphertext.GetData().size(); i++)
			{
				_plaintext.GetData().push_back(_ciphertext.GetData().at(i) ^ _key.GetData().at(i));
			}

			_plaintext.Write("decrypted.txt");
			if (_ciphertext.GetData().size() == _plaintext.GetData().size())
				std::cout << "The decryption was successful!" << std::endl;
			else std::cout << "Error while decoding" << std::endl;
		}
	}

	void Encrypt()
	{
		if (!_plaintext.Open("..\\resources\\plaintext.txt"))
		{
			std::cout << "Cannot open source file. Perhaps it doesn't exist." << std::endl;
			return;
		}

		std::cout << "Creating key..." << std::endl;
		createKey();
		std::cout << "Done!" << std::endl;
		if (!_key.Open("key.txt"))
		{
			std::cout << "Cannot open key file. Perhaps it doesn't exist." << std::endl;
			return;
		}

		if (_plaintext.GetData().size() != _key.GetData().size())
		{
			std::cout << "The size of the source file and key file do not match." << std::endl;
			return;
		}
		else
		{
			std::cout << "Encryption..." << std::endl;
			int i = 0;
			for (; i < _plaintext.GetData().size(); i++)
			{
				_ciphertext.GetData().push_back(_plaintext.GetData().at(i) ^ _key.GetData().at(i));
			}
			_ciphertext.Write("ciphertext.txt");
			if (_ciphertext.GetData().size() == _plaintext.GetData().size())
				std::cout << "The encryption is successful!" << std::endl;
			else std::cout << "Error while encrypting" << std::endl;
		}
	}
};

int main()
{
	OTP OTP_alg;
	int mode;
	std::cout << "Choose mode:" << std::endl << "1 - to encrypt a file" << std::endl << "2 - to decrypt the file" << std::endl;
	std::cin >> mode;
	system("cls");
	if (mode == 1)
	{
		OTP_alg.Encrypt();
	}
	else if (mode == 2)
	{
		OTP_alg.Decrypt();
	}
	system("pause");
	return 0;
}