#define _CRT_SECURE_NO_WARNINGS
#include "iostream"
#include <string>
#include <conio.h>
#include <string>
#include "..\..\crypto++\des.h"
#include "..\..\crypto++\cryptlib.h"
#include "..\..\crypto++\modes.h"
#include "..\..\crypto++\osrng.h"
#include "..\..\crypto++\filters.h"
#include "..\..\crypto++\cbcmac.h"
#include <vector>

#pragma comment(lib,"..\\..\\cryptlib.lib")

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

//std::vector<byte> Encrypt(std::vector<byte> &plaintext, std::vector<byte> myKey)
//{
//	byte key[CryptoPP::DES::DEFAULT_KEYLENGTH];
//	
//	for (int i = 0; i < CryptoPP::DES::DEFAULT_KEYLENGTH; i++)
//	{
//		key[i] = myKey.at(i);
//	}
//	std::vector<byte> ciphertext;
//	ciphertext.resize(plaintext.size() + CryptoPP::DES::BLOCKSIZE);
//	CryptoPP::ArraySink cs(&ciphertext[0], ciphertext.size());
//	CryptoPP::ArraySource(plaintext.data(), plaintext.size(), true,
//		new CryptoPP::StreamTransformationFilter(CryptoPP::ECB_Mode<CryptoPP::DES>::Encryption(key, 8),
//			new CryptoPP::Redirector(cs)));
//	ciphertext.resize(cs.TotalPutLength());
//
//	return ciphertext;
//}
//std::vector<byte> Decrypt(std::vector<byte> &ciphertext, std::vector<byte> myKey)
//{
//	byte key[CryptoPP::DES::DEFAULT_KEYLENGTH];
//
//	for (int i = 0; i < CryptoPP::DES::DEFAULT_KEYLENGTH; i++)
//	{
//		key[i] = myKey.at(i);
//	}
//	std::vector<byte> plaintext;
//	plaintext.resize(ciphertext.size() + CryptoPP::DES::BLOCKSIZE);
//	CryptoPP::ArraySink ds(&plaintext[0], plaintext.size());
//	CryptoPP::ArraySource(ciphertext.data(), ciphertext.size(), true,
//		new CryptoPP::StreamTransformationFilter(CryptoPP::ECB_Mode<CryptoPP::DES>::Decryption(key, 8),
//			new CryptoPP::Redirector(ds)));
//	plaintext.resize(ds.TotalPutLength());
//
//	return plaintext;
//}