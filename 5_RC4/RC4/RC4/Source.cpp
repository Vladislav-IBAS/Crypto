#define _CRT_SECURE_NO_WARNINGS
#define byte unsigned char

#include <iostream>
#include <vector>
#include <string>
#include <locale>

class MyFile
{
private:
	std::vector<byte> _data;
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

	std::vector<byte> &GetData() { return _data; }

};

class RC4
{
private:
	MyFile _plaintext;
	MyFile _key;
	MyFile _ciphertext;
	byte _s[256];
	std::string _filename;

	void generateKey(int lenght)
	{
		srand(time(NULL));
		std::vector<byte> data;
		FILE* key = fopen("key.txt", "wb");
		int size = lenght;
		for (int i = 0; i < size; i++)
		{
			byte c = rand() % 256;
			data.push_back(c);
		}
		fwrite(data.data(), data.size(), 1, key);
		fclose(key);
	}

	void create_S_Table()
	{
		for (int i = 0; i < 256; i++)
		{
			_s[i] = (byte)i;
		}
		int j = 0;
		for (int i = 0; i < 256; i++)
		{
			j = (j + _s[i] + _key.GetData().at(i % _key.GetData().size())) % 256;
			byte tmp = _s[i];
			_s[i] = _s[j];
			_s[j] = tmp;
		}
	}

public:
	void Encrypt()
	{
		system("cls");
		int gen;
		std::cout << "1. Generate new key\n2. Use existing key\n";
		std::cin >> gen;
		system("cls");
		if (gen == 1)
		{
			int keyLenght;
			std::cout << "Enter key lenght: ";
			std::cin >> keyLenght;
			generateKey(keyLenght);
			_key.Open("key.txt");
		}
		if (gen == 2)
		{
			std::cout << "Enter key file name: ";
			std::cin >> _filename;
			if (!_key.Open(_filename))
			{
				std::cout << "Cannot open key file. Perhaps it doesn't exist.";
				return;
			}
		}
		system("cls");
		create_S_Table();

		std::cout << "Enter plaintext file name: ";
		std::cin >> _filename;
		if (!_plaintext.Open(_filename))
		{
			std::cout << "Cannot open plaintext file. Perhaps it doesn't exist.";
			return;
		}

		int i = 0;
		int j = 0;
		byte k;
		for (int t = 0; t < _plaintext.GetData().size(); t++)
		{
			i = (i + 1) % 256;
			j = (j + _s[i]) % 256;
			byte tmp = _s[i];
			_s[i] = _s[j];
			_s[j] = tmp;
			k = _s[(_s[i] + _s[j]) % 256];
			_ciphertext.GetData().push_back(_plaintext.GetData().at(t) ^ k);
		}
		system("cls");
		std::cout << "Save ciphertext as: ";
		std::cin >> _filename;
		_ciphertext.Write(_filename);
	}

	void Decrypt()
	{
		system("cls");

		std::cout << "Enter ciphertext file name: ";
		std::cin >> _filename;
		if (!_ciphertext.Open(_filename))
		{
			std::cout << "Cannot open ciphertext file. Perhaps it doesn't exist.";
			return;
		};
		system("cls");

		std::cout << "Enter key file name: ";
		std::cin >> _filename;
		if (!_key.Open(_filename))
		{
			std::cout << "Cannot open key file. Perhaps it doesn't exist.";
			return;
		}

		system("cls");
		create_S_Table();

		int i = 0;
		int j = 0;
		byte k;
		for (int t = 0; t < _ciphertext.GetData().size(); t++)
		{
			i = (i + 1) % 256;
			j = (j + _s[i]) % 256;
			byte tmp = _s[i];
			_s[i] = _s[j];
			_s[j] = tmp;
			k = _s[(_s[i] + _s[j]) % 256];
			_plaintext.GetData().push_back(_ciphertext.GetData().at(t) ^ k);
		}
		system("cls");
		std::cout << "Save decrypted file as: ";
		std::cin >> _filename;
		_plaintext.Write(_filename);

	}
};

void main()
{
	int n = 8;
	int mode;
	std::cout << "Choose mode:\n1. Encrypt\n2. Decrypt\n";
	std::cin >> mode;
	RC4 rc4;

	if (mode == 1)
	{
		rc4.Encrypt();
	}
	if (mode == 2)
	{
		rc4.Decrypt();
	}

	system("pause");
}