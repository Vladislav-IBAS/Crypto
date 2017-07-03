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

#pragma comment(lib,"..\\third-party\\lib\\cryptlib.lib")

std::vector<byte> Encrypt(std::vector<byte> plaintext, std::vector<byte> myKey)
{
	std::vector<byte> ciphertext;

	byte key[CryptoPP::DES::DEFAULT_KEYLENGTH];

	for (int i = 0; i < CryptoPP::DES::DEFAULT_KEYLENGTH; i++)
	{
		key[i] = myKey.at(i);
	}

	ciphertext.resize(plaintext.size() + CryptoPP::DES::BLOCKSIZE);
	CryptoPP::ArraySink buf(&ciphertext[0], ciphertext.size());

	CryptoPP::ECB_Mode<CryptoPP::DES>::Encryption enc(key, sizeof(key));
	CryptoPP::ArraySource(plaintext.data(), plaintext.size(), true,
		new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf), CryptoPP::StreamTransformationFilter::ZEROS_PADDING));

	ciphertext.resize(buf.TotalPutLength());

	return ciphertext;
}

std::vector<byte> Decrypt(std::vector<byte> ciphertext, std::vector<byte> myKey)
{
	std::vector<byte> plaintext;

	byte key[CryptoPP::DES::DEFAULT_KEYLENGTH];

	for (int i = 0; i < CryptoPP::DES::DEFAULT_KEYLENGTH; i++)
	{
		key[i] = myKey.at(i);
	}

	plaintext.resize(ciphertext.size() + CryptoPP::DES::BLOCKSIZE);
	CryptoPP::ArraySink buf(&plaintext[0], plaintext.size());

	CryptoPP::ECB_Mode<CryptoPP::DES>::Decryption dec(key, sizeof(key));
	CryptoPP::ArraySource(ciphertext.data(), ciphertext.size(), true,
		new CryptoPP::StreamTransformationFilter(dec, new CryptoPP::Redirector(buf), CryptoPP::StreamTransformationFilter::ZEROS_PADDING));

	plaintext.resize(buf.TotalPutLength());
	
	return plaintext;
}