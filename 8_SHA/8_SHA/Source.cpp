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

void SHA(std::string filename)
{
	MyFile plaintext;
	MyFile hash;
	
	if (!plaintext.Open(filename))
	{
		std::cout << "Cannot open plaintext file";
		return;
	}

	std::string message;
	for (int i = 0; i < plaintext.GetData().size(); i++)
	{
		message += plaintext.GetData().at(i);
	}

	std::string hashstr;
	CryptoPP::SHA512 sha512;
	CryptoPP::HashFilter filter(sha512, new CryptoPP::HexEncoder(new CryptoPP::StringSink(hashstr)));
	
	CryptoPP::ChannelSwitch cs;
	cs.AddDefaultRoute(filter);

	CryptoPP::StringSource ss(message, true, new CryptoPP::Redirector(cs));

	for (int i = 0; i < hashstr.size(); i++)
	{
		hash.GetData().push_back(hashstr[i]);
	}
	hash.Write("Hash.txt");
}

void main()
{
	SHA("plaintext.txt");

	system("pause");
}