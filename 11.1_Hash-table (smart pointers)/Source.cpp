#define _CRT_SECURE_NO_WARNINGS
#define mybyte unsigned char
#include "..\shared\MyFile.h"
#include "..\shared\MyHash.h"
#include "..\shared\sha.h"
#include "..\shared\Dimas.h"
#include "..\shared\Ksuha.h"
#include <vector>
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdint.h>
#include <sstream>
#include <iostream>
#include <ctime>
#include <sstream>

#pragma comment(lib,"..\\third-party\\lib\\my.lib")

struct Ht
{
	std::string key;
	std::vector<std::string> attributes;
	std::shared_ptr<Ht> next = NULL;
};

int CreateHashTable(LPCSTR InitialPath, std::vector<std::shared_ptr<Ht>> &table,
	int &count, int &rows, int hashF = 0, double alpha_max = 2)
{
	double alpha = 0;
	WIN32_FIND_DATA FindFileData;
	HANDLE hf;
	SYSTEMTIME time;
	TCHAR searchPath[MAX_PATH + 1];
	LPCSTR all = "*";
	LPCSTR slash = "\\";
	lstrcpy(searchPath, InitialPath);
	lstrcat(searchPath, slash);
	lstrcat(searchPath, all);
	TCHAR fullPath[MAX_PATH + 1];
	hf = FindFirstFile(searchPath, &FindFileData);
	FindNextFile(hf, &FindFileData);

	if (FindNextFile(hf, &FindFileData))
	{
		if (hf != INVALID_HANDLE_VALUE)
		{
			if (count / (double)rows >= alpha_max)
			{
				return 13;
			}
			do
			{
				std::stringstream ss;
				bool folder = false;
				std::vector<mybyte> src;
				std::vector<mybyte> hash;
				uint16_t index;
				std::vector<std::string> attributes;
				lstrcpy(fullPath, InitialPath);
				lstrcat(fullPath, slash);
				lstrcat(fullPath, FindFileData.cFileName);
				char ch = FindFileData.cFileName[0];
				int i = 0;
				while (ch != '\0')
				{
					ch = fullPath[i + 1];
					src.push_back(fullPath[i]);
					i++;
				}
				switch (hashF)
				{
				case 0: hash = My_hash(src, 2); break;
				case 1: hash = SHA(src); hash.resize(2); break;
				case 2: hash = Dimas_hash(src, 2); hash.resize(2); break;
				case 3: hash = Ksusha_hash(src, 2); hash.resize(2); break;
				default:
					break;
				}
				index = (hash.at(0) << 8) + hash.at(1);

				bool writed = false;

				if (table.at(index) != NULL)
				{
					std::shared_ptr<Ht> elem = table.at(index);
					while (elem != NULL && !writed)
					{
						Ht element = *elem.get();
						if (element.key == fullPath)
						{
							writed = true;
						}
						elem = (*elem.get()).next;
					}
				}
				if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
				{
					folder = true;
				}
				if (!writed)
				{
					count++;
					if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
					{
						folder = true;
						attributes.push_back("Type: Folder");
					}
					else
					{
						attributes.push_back("Type: File");
					}

					ss << "Full path: " << fullPath;
					attributes.push_back(ss.str());

					ss.str(std::string());
					ss << "Name: " << FindFileData.cFileName;
					attributes.push_back(ss.str());

					ss.str(std::string());
					FileTimeToSystemTime(&FindFileData.ftCreationTime, &time);
					ss << "Creation time: " << time.wDay << "." << time.wMonth << "." << time.wYear << " " << time.wHour << ":" << time.wMinute;
					attributes.push_back(ss.str());

					ss.str(std::string());
					FileTimeToSystemTime(&FindFileData.ftLastWriteTime, &time);
					ss << "Last write time: " << time.wDay << "." << time.wMonth << "." << time.wYear << " " << time.wHour << ":" << time.wMinute;
					attributes.push_back(ss.str());

					ss.str(std::string());
					ss << "Size: " << (FindFileData.nFileSizeHigh * (MAXDWORD + 1)) + FindFileData.nFileSizeLow;
					attributes.push_back(ss.str());

					if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_READONLY)
					{
						attributes.push_back("Readonly: Yes");
					}
					else
					{
						attributes.push_back("Readonly: No");
					}

					if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN)
					{
						attributes.push_back("Hidden: Yes");
					}
					else
					{
						attributes.push_back("Hidden: No");
					}

					if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_SYSTEM)
					{
						attributes.push_back("System: Yes");
					}
					else
					{
						attributes.push_back("System: No");
					}

					if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_COMPRESSED)
					{
						attributes.push_back("Compressed: Yes");
					}
					else
					{
						attributes.push_back("Compressed: No");
					}

					if (table.at(index) == NULL)
					{
						rows++;
						Ht element;
						element.key = fullPath;
						element.attributes = attributes;
						element.next = NULL;
						table.at(index) = std::make_shared<Ht>(element);
					}
					else
					{
						//Ht element = (*table.at(index).get());
						std::shared_ptr<Ht> el = table.at(index);
						while ((*el.get()).next != NULL)
						{
							el = (*el.get()).next;
						}
						Ht new_element;
						new_element.key = fullPath;
						new_element.attributes = attributes;
						new_element.next = NULL;
						(*el.get()).next = std::make_shared<Ht>(new_element);
					}
				}

				if (folder)
				{
					if (CreateHashTable(fullPath, table, count, rows, hashF, alpha_max) == 13)
					{
						return 13;
					}
				}
				if (count % 50 == 0)
				{
					std::cout << "\r" << "Count of objects: " << count << " \\ alpha average: " << count / (double)rows;
				}

			} while (FindNextFile(hf, &FindFileData));
			FindClose(hf);
			return 0;
		}
	}
}

void Search(std::string path, std::vector<std::vector<std::pair<std::string, std::vector<std::string>>>> &table, int hashF = 0)
{
	std::vector<mybyte> _path;
	std::vector<mybyte> hash;
	std::vector<std::pair<std::string, std::vector<std::string>>> element;
	uint16_t index;

	for (int i = 0; i < path.size(); i++)
	{
		_path.push_back(path[i]);
	}

	switch (hashF)
	{
	case 0: hash = My_hash(_path, 2); break;
	case 1: hash = SHA(_path); hash.resize(2); break;
	case 2: hash = Dimas_hash(_path, 2); hash.resize(2); break;
	case 3: hash = Ksusha_hash(_path, 2); hash.resize(2); break;
	default:
		break;
	}

	index = (hash.at(0) << 8) + hash.at(1);

	element = table.at(index);

	if (element.size() == 0)
	{
		std::cout << "Not found" << std::endl;
		return;
	}

	for (int i = 0; i < element.size(); i++)
	{
		if (element[i].first == path)
		{
			for (auto k : element[i].second)
			{
				std::cout << k << std::endl;
			}
			break;
		}
		std::cout << "not found" << std::endl;
	}
}

void main()
{
	int count = 0;
	int rows = 0;
	int b = 16;
	int hashF = 0;
	bool tableCreated = false;
	double alpha_max = 1.29;
	unsigned int start_time;
	unsigned int end_time;
	bool flag = true;
	LPCSTR path = "C:\\Program Files";

	std::vector<std::shared_ptr<Ht>> ptr_table;
	ptr_table.resize(pow(2, b));

	std::vector<std::shared_ptr<Ht>> ptr_tmp;
	ptr_tmp.resize(pow(2, b));

	while (flag)
	{
		system("cls");
		std::cout << "1 - Create new hash-table" << std::endl;
		std::cout << "2 - Enter alpha max" << std::endl;
		std::cout << "3 - Write hash-table to file" << std::endl;
		std::cout << "4 - Find file" << std::endl;
		std::cout << "5 - Exit" << std::endl;
		std::cout << "Choose mode: ";
		int mode;
		std::cin >> mode;
		switch (mode)
		{
		case 1:
		{
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			int col = 0;
			bool created = false;
			bool tried = false;

			system("cls");
			std::cout << "1 - My hash" << std::endl;
			std::cout << "2 - SHA" << std::endl;
			std::cout << "3 - D1M@$ hash" << std::endl;
			std::cout << "4 - Ksu hash" << std::endl;
			std::cout << "Choose hash-function: ";
			int hash_num;
			std::cin >> hash_num;

			while (!created && col < 4)
			{
				col++;
				std::string name;
				switch ((hash_num - 1) % 4)
				{
				case 0: name = "My hash function"; break;
				case 1: name = "SHA hash function"; break;
				case 2: name = "D1M@$ hash function"; break;
				case 3: name = "Ksu hash function"; break;
				default:
					break;
				}
				hashF = (hash_num - 1) % 4;

				system("cls");

				count = 0;
				rows = 0;

				if (tried)
				{
					std::cout << "Rebuilding table..." << std::endl;
					for (int i = 0; i < ptr_table.size(); i++)
					{
						std::shared_ptr<Ht> el = ptr_table.at(i);
						if (el != NULL)
						{
							while (el != NULL)
							{
								count++;
								Ht element = *el.get();
								std::vector<mybyte> src;
								for (int j = 0; j < element.key.size(); j++)
								{
									src.push_back(element.key[j]);
								}
								std::vector<mybyte> hash;
								switch (hashF)
								{
								case 0: hash = My_hash(src, 2); break;
								case 1: hash = SHA(src); hash.resize(2); break;
								case 2: hash = Dimas_hash(src, 2); hash.resize(2); break;
								case 3: hash = Ksusha_hash(src, 2); hash.resize(2); break;
								default:
									break;
								}
								uint16_t index = (hash.at(0) << 8) + hash.at(1);

								if (ptr_tmp.at(index) == NULL)
								{
									rows++;
									Ht tmp_element = *el.get();
									ptr_tmp.at(index) = std::make_shared<Ht>(tmp_element);
									(*ptr_tmp.at(index).get()).next = NULL;
								}
								else
								{
									std::shared_ptr<Ht> tmp = ptr_tmp.at(index);
									while ((*tmp.get()).next != NULL)
									{
										tmp = (*tmp.get()).next;
									}
									Ht tmp_element = *el.get();
									(*tmp.get()).next = std::make_shared<Ht>(tmp_element);
									tmp = (*tmp.get()).next;
									(*tmp.get()).next = NULL;
								}
								el = (*el.get()).next;
							}
						}
					}
					ptr_table.clear();
					ptr_table.resize(pow(2, b));
					ptr_table = ptr_tmp;
					ptr_tmp.clear();
					ptr_tmp.resize(pow(2, b));
					std::cout << "Done!" << std::endl;
				}

				std::cout << "Path for hash-table: " << path << std::endl;
				std::cout << "Alpha max: " << alpha_max << std::endl;
				std::cout << "Creating hash-table using ";
				SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 6));
				std::cout << name;
				SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));
				std::cout << "..." << std::endl;
				start_time = clock();
				if (CreateHashTable(path, ptr_table, count, rows, (hash_num - 1) % 4, alpha_max) == 13)
				{
					tried = true;
					end_time = clock();
					std::cout << "\r" << "Count of objects: " << count << " \\ alpha average: " << count / (double)rows << std::endl;
					SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 6));
					std::cout << "Total time: " << (end_time - start_time) / (double)1000 << "sec." << std::endl;
					std::cout << "Alpha average: " << count / (double)rows << std::endl << std::endl;
					SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 12));
					std::cout << "Oops... Alpha average is more then alpha max =(" << std::endl << std::endl;
					SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));
					hash_num++;
					for (int i = 7; i >= 0; i--)
					{
						std::cout << "\rThe table will be rebuilt in ";
						SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 6));
						std::cout << i << " seconds";
						SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));
						Sleep(1000);
					}
				}
				else
				{

					created = true;
					end_time = clock();
					std::cout << "\r" << "Count of objects: " << count << " \\ alpha average: " << count / (double)rows << std::endl;
					SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 6));
					std::cout << "Total time: " << (end_time - start_time) / (double)1000 << "sec." << std::endl;
					std::cout << "Alpha average: " << count / (double)rows << std::endl << std::endl;
					SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 2));
					std::cout << "Hash-table created =)" << std::endl << std::endl;
					SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));
					tableCreated = true;
					system("pause");
				}
			}
			if (!created && col >= 4)
			{
				system("cls");
				SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 12));
				std::cout << "Table with specified options can't be created =( " << std::endl;
				SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));
				std::cout << "Try to increase alpha average" << std::endl << std::endl;
				system("pause");
			}
			break;
		}
		case 2:
		{
			system("cls");
			std::cout << "Enter alpha max: ";
			std::cin >> alpha_max;
			break;
		}
		case 3:
		{
			if (!tableCreated)
			{
				system("cls");
				std::cout << "The hash-table is not created.";
				system("pause");
				break;
			}
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			system("cls");
			FILE* hTable(NULL);
			std::string fName;
			std::string split;
			for (int i = 0; i < 10; i++)
			{
				split += "----------";
			}
			std::cout << "Enter file name for saving: ";
			std::cin >> fName;
			hTable = fopen(fName.c_str(), "w");
			std::stringstream ss;
			std::cout << "\nWriting...\n";
			for (int i = 0; i < ptr_table.size(); i++)
			{
				std::shared_ptr<Ht> el = ptr_table.at(i);
				if (el != NULL)
				{
					bool f = false;
					fprintf(hTable, "%-6d|  ", i);
					while (el != NULL)
					{
						Ht element = *el.get();
						el = (*el.get()).next;
						ss.str(std::string());
						ss << element.key;
						if (!f)
						{
							if (ss.str().size() > 100)
							{
								fprintf(hTable, "%-100s|\n", ss.str().substr(0, 100).c_str());
								fprintf(hTable, "      |    %-98s|\n", ss.str().substr(100, ss.str().size() - 100).c_str());
							}
							else
							{
								fprintf(hTable, "%-100s|\n", ss.str().c_str());
							}
							f = true;
						}
						else
						{
							if (ss.str().size() > 100)
							{
								fprintf(hTable, "      |  %-100s|\n", ss.str().substr(0, 100).c_str());
								fprintf(hTable, "      |    %-98s|\n", ss.str().substr(100, ss.str().size() - 100).c_str());
							}
							else
							{
								fprintf(hTable, "      |  %-100s|\n", ss.str().c_str());
							}
						}
					}
					fprintf(hTable, "------|%s--|\n", split.c_str());
				}
			}
			fclose(hTable);
			SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 2));
			std::cout << "Done!\n\n";
			SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));
			system("pause");
			break;
		}
		case 4:
		{
			if (!tableCreated)
			{
				system("cls");
				std::cout << "The hash-table is not created.";
				system("pause");
				break;
			}
			system("cls");




			system("pause");
			break;
		}
		case 5: flag = false; break;
		default:
			break;
		}
	}
}