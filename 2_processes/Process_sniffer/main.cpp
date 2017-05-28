#include "windows.h"
#include "iostream"
#include <stdio.h>
#include "tlhelp32.h"
#include <algorithm> 
#include <vector>   
#include <set>
#include <iterator>
#include <string>
#include <conio.h>

class ProcessList
{
private:
	std::vector<std::pair<DWORD, std::string>> _procPID;

public:
	ProcessList()
		: _procPID(NULL)
	{
		
	}

	std::vector<std::pair<DWORD, std::string>> &GetData()
	{ 
		std::sort(_procPID.begin(), _procPID.end());
		return _procPID; 
	}

	void GetProcList()
	{
		PROCESSENTRY32 proc;
		HANDLE hSnap;
		hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		proc.dwSize = sizeof(PROCESSENTRY32);

		Process32First(hSnap, &proc); 
		_procPID.clear();

		char c_name[sizeof(proc.szExeFile)];                      
		wcstombs(c_name, proc.szExeFile, sizeof(proc.szExeFile)); 
			                                                      
        std::string name = std::string(c_name);

		_procPID.push_back(std::make_pair((DWORD)proc.th32ProcessID, name));
		while (Process32Next(hSnap, &proc)) {

			char c_name[sizeof(proc.szExeFile)];
			wcstombs(c_name, proc.szExeFile, sizeof(proc.szExeFile));
            std::string name = std::string(c_name);

			_procPID.push_back(std::make_pair((DWORD)proc.th32ProcessID, name));
		} 
		CloseHandle(hSnap);
	}
};


int main(int argc, char* argv[])
{
	ProcessList p1;
	ProcessList p2;
	p1.GetProcList();
    bool isKeyDown = false;

	std::vector<std::pair<DWORD, std::string>> diff;

	while (!isKeyDown)
	{
		diff.clear(); 
		p2.GetProcList();
		std::set_difference(p2.GetData().begin(), p2.GetData().end(), p1.GetData().begin(), p1.GetData().end(),
			std::inserter(diff, diff.begin())); 
		if (!diff.empty())
		{
			for (auto i : diff)
			{
				printf("Process started:\n");
				printf("PID: %d\n", i.first);
                std::cout << "Name: " << i.second << std::endl << std::endl;
			}
		}

		diff.clear();
		std::set_difference(p1.GetData().begin(), p1.GetData().end(), p2.GetData().begin(), p2.GetData().end(),
			std::inserter(diff, diff.begin())); 
		if (!diff.empty())
		{
			for (auto i : diff)
			{
				printf("Process closed:\n");
				printf("PID: %d\n", i.first);
                std::cout << "Name: " << i.second << std::endl << std::endl;
			}
		}
		p1.GetProcList();
		Sleep(100);
        isKeyDown = _kbhit();
	}

	return 0;
}