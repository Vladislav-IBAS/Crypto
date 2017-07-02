#include "..\third-party\src\mpir-2.7.2\lib_mpir_cxx\Win32\Debug\mpirxx.h"
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <winsock.h>
#include <sstream>
#include <vector>
#include "..\shared\DES.h"

#pragma comment(lib,"..\\third-party\\lib\\WS2_32.LIB")
#pragma comment(lib,"..\\third-party\\lib\\mpirxx.lib")
#pragma comment(lib,"..\\third-party\\lib\\mpir.lib")
#pragma comment(lib,"..\\third-party\\lib\\my.lib")

#define PORT 500
#define SERVER "127.0.0.1" 

int main(int argc, char* argv[])
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	bool is_Server = false;
	srand(time(0));
	std::string name;
	std::stringstream ss;
	if (argc > 1)
	{
		name = argv[1];
	}
	else
	{
		name = "Alice";
	}

	char buff[1024];
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 6));
	printf("Hi, I'am %s =)\n", name.c_str());
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));

	WSAStartup(0x0202, (WSADATA *)&buff[0]);

	SOCKET mysocket;
	mysocket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	HOSTENT *hst;

	if (inet_addr(SERVER) != INADDR_NONE)
	{
		dest_addr.sin_addr.s_addr = inet_addr(SERVER);
	}
	else
	{
		if (hst = gethostbyname(SERVER))
		{
			((unsigned long *)&dest_addr.sin_addr)[0] = ((unsigned long **)hst->h_addr_list)[0][0];
		}
	}

	if (connect(mysocket, (sockaddr *)&dest_addr, sizeof(dest_addr)))
	{
		std::cout << "No one is online =(" << std::endl;
		std::cout << "Waiting...\n" << std::endl;
		is_Server = true;
	}

	if (!is_Server)
	{
		int nsize;

		ss.str(std::string());
		ss << name;
		send(mysocket, ss.str().c_str(), sizeof(ss.str()), 0);

		std::string friendName = "";
		nsize = recv(mysocket, &buff[0], sizeof(buff), 0);
		int f = 0;
		while (buff[f] != '\0')
		{
			friendName += buff[f];
			f++;
		}
		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 6));
		std::cout << "Hello " << friendName << "!" << std::endl;
		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));

		std::cout << std::endl;
		gmp_randstate_t state;
		mp_bitcnt_t init = rand() % 100;
		gmp_randinit_lc_2exp_size(state, init);

		nsize = recv(mysocket, &buff[0], sizeof(buff), 0);
		mpz_t p;
		mpz_init_set_str(p, buff, 10);
		std::cout << "p: " << p << std::endl;

		nsize = recv(mysocket, &buff[0], sizeof(buff), 0);
		mpz_t n;
		mpz_init_set_str(n, buff, 10);
		std::cout << "N: " << n << std::endl;

		mpz_t B;
		std::cout << "Get A from Friend: ";
		nsize = recv(mysocket, &buff[0], sizeof(buff), 0);
		mpz_init_set_str(B, buff, 10);
		std::cout << B << std::endl;

		mpz_t a;
		mpz_init(a);
		mpz_urandomb(a, state, 64);
		mpz_t A;
		mpz_init(A);
		mpz_powm(A, p, a, n);

		ss.str(std::string());
		ss << A;
		std::cout << "Send B to Friend: ";
		send(mysocket, ss.str().c_str(), sizeof(ss.str()), 0);
		std::cout << A << std::endl;

		mpz_t KA;
		mpz_init(KA);
		mpz_powm(KA, B, a, n);
		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 2));
		std::cout << "Our key is: " << KA << std::endl;
		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));

		nsize = recv(mysocket, &buff[0], sizeof(buff), 0);
		std::string msize = "";
		f = 0;
		while (buff[f] != '\0')
		{
			msize += buff[f];
			f++;
		}
		int imsize = std::stoi(msize);

		nsize = recv(mysocket, &buff[0], sizeof(buff), 0);
		std::string message = "";
		std::vector<byte> cipher;
		for (int i = 0; i < imsize; i++)
		{
			message += buff[i];
		} 

		std::cout << "Get message from " << friendName << ": " << message << std::endl;

		ss.str(std::string());
		ss << KA;
		std::vector<byte> key;
		for (int i = 0; i < CryptoPP::DES::DEFAULT_KEYLENGTH; i++)
		{
			key.push_back(ss.str()[i]);
		}	
		for (int i = 0; i < message.length(); i++)
		{
			cipher.push_back(message[i]);
		}

		std::vector<byte> dec = Decrypt(cipher, key);
		std::string decStr = "";
		for (int i = 0; i < dec.size(); i++)
		{
			decStr += dec.at(i);
		}
		std::cout << "Decoded: " << decStr << std::endl;

		system("pause");

	}
	else
	{
		SOCKET mysocket2;
		mysocket2 = socket(AF_INET, SOCK_STREAM, 0);
		sockaddr_in local_addr;
		local_addr.sin_family = AF_INET;
		local_addr.sin_port = htons(PORT);
		local_addr.sin_addr.s_addr = 0;

		bind(mysocket2, (sockaddr *)&local_addr, sizeof(local_addr));

		listen(mysocket2, 0x100);

		SOCKET client_socket;
		sockaddr_in client_addr;

		int nsize;
		int client_addr_size = sizeof(client_addr);

		while ((client_socket = accept(mysocket2, (sockaddr *)&client_addr, &client_addr_size)))
		{
			char buff[20 * 1024];

			std::string friendName = "";
			nsize = recv(client_socket, &buff[0], sizeof(buff), 0);
			int f = 0;
			while (buff[f] != '\0')
			{
				friendName += buff[f];
				f++;
			}
			SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 6));
			std::cout << "Hello " << friendName << "!" << std::endl;
			SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));

			ss.str(std::string());
			ss << name;
			send(client_socket, ss.str().c_str(), sizeof(ss.str()), 0);

			gmp_randstate_t state;
			mp_bitcnt_t init = rand() % 100;
			gmp_randinit_lc_2exp_size(state, init);

			mpz_t p;
			mpz_t n;
			mpz_t a;
			mpz_init(p);
			mpz_init(n);
			mpz_init(a);

			mpz_urandomb(p, state, 64);
			mpz_urandomb(n, state, 64);

			ss.str(std::string());
			ss << p;
			std::cout << "p: " << ss.str() << std::endl;
			send(client_socket, ss.str().c_str(), sizeof(ss.str()), 0);

			ss.str(std::string());
			ss << n;
			std::cout << "N: " << ss.str() << std::endl;
			send(client_socket, ss.str().c_str(), sizeof(ss.str()), 0);

			mpz_urandomb(a, state, 64);
			mpz_t A;
			mpz_init(A);
			mpz_powm(A, p, a, n);

			ss.str(std::string());
			ss << A;
			std::cout << "Sending A to Friend: ";
			send(client_socket, ss.str().c_str(), sizeof(ss.str()), 0);
			std::cout << ss.str() << std::endl;

			mpz_t B;
			nsize = recv(client_socket, &buff[0], sizeof(buff), 0);
			mpz_init_set_str(B, buff, 10);
			std::cout << "Get B from Friend: " << B << std::endl;

			mpz_t KA;
			mpz_init(KA);
			mpz_powm(KA, B, a, n);
			SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 2));
			std::cout << "Our key is: " << KA << std::endl;
			SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));

			std::string message = "I love cryptography!!!";
			std::cout << "Message for " << friendName << ": " << message << std::endl;

			ss.str(std::string());
			ss << KA;
			std::vector<byte> key;
			for (int i = 0; i < CryptoPP::DES::DEFAULT_KEYLENGTH; i++)
			{
				key.push_back(ss.str()[i]);
			}

			std::vector<byte> plaintext;
			for (int i = 0; i < message.length(); i++)
			{
				plaintext.push_back(message[i]);
			}

			std::vector<byte> enc;
			enc = Encrypt(plaintext, key);
			//enc.resize(sizeof(Encrypt(plaintext, key)));

			std::string encStr = "";
			std::cout << enc.size();
			ss.str(std::string());
			ss << enc.size();
			send(client_socket, ss.str().c_str(), sizeof(ss.str()), 0);
			for (int i = 0; i < enc.size(); i++)
			{
				encStr += enc.at(i);
			}
			//encStr += '\0';
			std::cout << "Encoded: " << encStr << std::endl;

			std::cout << "Sending encoded message to " << friendName << "..." << std::endl;
			ss.str(std::string());
			ss << encStr.c_str();
			send(client_socket, ss.str().c_str(), sizeof(ss.str()), 0);

			system("pause");
		}
	}
}