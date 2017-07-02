#ifndef _MYFILE_H_
#define _MYFILE_H_
#define mybyte unsigned char
#include <vector>

class MyFile
{
private:
	std::vector<mybyte> _data;
	FILE* _file;

public:
	MyFile();
	bool Open(std::string name);
	bool Write(std::string name);
	std::vector<mybyte> &GetData();
};
#endif
