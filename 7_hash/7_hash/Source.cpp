#define _CRT_SECURE_NO_WARNINGS
#define byte unsigned char
#include <iostream>
#include <vector>
#include  <math.h>

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

void My_hash(std::string filename, int size = 256)
{
	MyFile plaintext;
	MyFile hash;
	int lenght;
	if (!plaintext.Open(filename))
	{
		std::cout << "Cannot open plaintext.";
		return ;
	}
	byte a = 13;
	for (int i = 0; i < size; i++)
	{
		hash.GetData().push_back(plaintext.GetData().at(i % plaintext.GetData().size()) << (a % 8));
		a ^= plaintext.GetData().at((i + int(a ^ i)) % plaintext.GetData().size());
	}
	a = 18;

	if (plaintext.GetData().size() > size)
	{
		lenght = plaintext.GetData().size();
	}
	else
	{
		lenght = size;
	}
	for (int i = 0; i < lenght; i++)
	{
		hash.GetData().at(i % size) ^= (plaintext.GetData().at((i + a) % plaintext.GetData().size()) ^ a);
		a ^= (hash.GetData().at((i + int(a ^ i)) % hash.GetData().size()) ^ ((a + i) % 256));
		/*if (i % 100 == 0)
		{
			system("cls");
			std::cout << "Hashing... " << (i / double(lenght)) * 100 << "%";
		}*/
	}
	hash.Write("hash.txt");
}

void main()
{
	My_hash("plaintext.txt");
	system("pause");
}