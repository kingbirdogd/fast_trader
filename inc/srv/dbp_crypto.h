#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <string>
#include <set>
#include <vector>
#ifndef __DBP_CRYTPO__
#define __DBP_CRYTPO__
namespace dbp
{
	namespace crypto
	{
		static bool g_bOpenSSLAlgorithmsLoad = false;
		const static std::string g_StrBase64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		inline static void InitOpenSSLAlgorithms()
		{
			if (!g_bOpenSSLAlgorithmsLoad)
			{
				OpenSSL_add_all_algorithms();
				g_bOpenSSLAlgorithmsLoad = true;
			}
		}
		inline static const EVP_CIPHER* GetAes256CtrClipher()
		{
			InitOpenSSLAlgorithms();
			return EVP_get_cipherbyname("aes-256-ctr");
		}
		const static EVP_CIPHER* g_pAes256CtrCipher = GetAes256CtrClipher();
		inline static unsigned char base64Value(char c)
		{
			if (c >= 'A' && c <= 'Z')
			{
				return (unsigned char)(c - 'A');
			}
			else if (c >= 'a' && c <= 'z')
			{
				return (unsigned char) ((char)26 + c - 'a');
			}
			else if (c >= '0' && c <= '9')
			{
				return (unsigned char) ((char)52 + c - '0');
			}
			else if (c == '+')
			{
				return (unsigned char)62;
			}
			else if (c == '/')
			{
				return (unsigned char)63;
			}
			else
			{
				return (unsigned char)0xFF;
			}
		}
		inline static std::string encodeBase64 (const unsigned char* _pszBuffer, unsigned int _uLength)
		{
			std::string strRt = "";
			unsigned int uBaseTime = _uLength / 3;
			unsigned int uRemain = _uLength  % 3;
			for (unsigned int i = 0; i < uBaseTime; ++i)
			{
				unsigned char uValue1 = _pszBuffer[i * 3] >> 2;
				unsigned char uValue2 = ((_pszBuffer[i * 3] & 0b11) << 4) + (_pszBuffer[i * 3 + 1] >> 4);
				unsigned char uValue3 = ((_pszBuffer[i * 3 + 1] & 0b1111) << 2) + (_pszBuffer[i * 3 + 2] >> 6);
				unsigned char uValue4 = _pszBuffer[i * 3 + 2] & 0b111111;
				strRt += g_StrBase64[uValue1];
				strRt += g_StrBase64[uValue2];
				strRt += g_StrBase64[uValue3];
				strRt += g_StrBase64[uValue4];
			}
			if (0 != uRemain)
			{
				unsigned char tmpBuffer [3] = {0, 0, 0};
				tmpBuffer [0] = _pszBuffer[uBaseTime * 3];
				if (2 == uRemain)
				{
					tmpBuffer [1] = _pszBuffer[uBaseTime * 3 + 1];
				}
				unsigned char uValue1 = tmpBuffer[0] >> 2;
				unsigned char uValue2 = ((tmpBuffer[0] & 0b11) << 4) + (tmpBuffer[1] >> 4);
				unsigned char uValue3 = ((tmpBuffer[1] & 0b1111) << 2) + (tmpBuffer[2] >> 6);
				strRt += g_StrBase64[uValue1];
				strRt += g_StrBase64[uValue2];
				if (1 == uRemain)
				{
					strRt += "==";
				}
				else
				{
					strRt += g_StrBase64[uValue3];
					strRt += "=";
				}
			}
			return strRt;
		}
		inline static std::string encodeBase64 (const std::vector<unsigned char>& _buffer)
		{
			return encodeBase64(&_buffer[0], _buffer.size());
		}
		inline static std::string encodeBase64 (const std::string& _str, bool _bWithEnding)
		{
			return encodeBase64 ((const unsigned char*)_str.c_str(), (unsigned int)(_str.length() + (unsigned int)_bWithEnding));
		}
		inline static bool decodeBase64 (const std::string& _str, std::vector<unsigned char>& _buffer)
		{
			if (0 != _str.length() % 4)
			{
				return false;
			}
			unsigned int iBufferSize = _str.length() / 4 * 3;
			if ('=' == _str[_str.length() - 1])
			{
				--iBufferSize;
			}
			if ('=' == _str[_str.length() - 2])
			{
				--iBufferSize;
			}
			_buffer.resize(0);
			for (unsigned int i = 0; i < _str.length(); i += 4)
			{
				unsigned char uValue1 = base64Value(_str[i]);
				if ((unsigned char)0xFF == uValue1)
				{
					return false;
				}
				unsigned char uValue2 = base64Value(_str[i + 1]);
				if ((unsigned char)0xFF == uValue2)
				{
					return false;
				}
				unsigned char uValue3 = base64Value(_str[i + 2]);
				if ((unsigned char)0xFF == uValue3)
				{
					if (_str.length() - 4 == i)
					{
						if ('=' == _str[i + 2])
						{
							uValue3 = 0;
						}
						else
						{
							return false;
						}
					}
					else
					{
						return false;
					}
				}
				unsigned char uValue4 = base64Value(_str[i + 3]);
				if ((unsigned char)0xFF == uValue4)
				{
					if (_str.length() - 4 == i)
					{
						if ('=' == _str[i + 3])
						{
							uValue4 = 0;
						}
						else
						{
							return false;
						}
					}
					else
					{
						return false;
					}
				}
				unsigned char uInsert1 = (uValue1 << 2) + (uValue2 >> 4);
				unsigned char uInsert2 = ((uValue2 & 0b1111) << 4) + (uValue3 >> 2);
				unsigned char uInsert3 = ((uValue3 & 0b11) << 6) + uValue4;
				_buffer.push_back(uInsert1);
				_buffer.push_back(uInsert2);
				_buffer.push_back(uInsert3);
			}
			_buffer.resize(iBufferSize);
			return true;
		}
		inline static std::string decodeBase64 (const std::string& _str, bool _bWithEnding)
		{
			std::vector<unsigned char> _buffer;
			if (!decodeBase64(_str, _buffer))
			{
				return "";
			}
			else
			{
				for (unsigned int i = 0; i < _buffer.size(); ++i)
				{
					if (0 == _buffer[i])
					{
						if (!_bWithEnding)
						{
							return "";
						}
						else
						{
							if (i != _buffer.size() -1)
							{
								return "";
							}
						}
					}
				}
				if (_bWithEnding)
				{
					if (0 != _buffer[_buffer.size() - 1])
					{
						return "";
					}
				}
				if (!_bWithEnding)
				{
					_buffer.push_back(0);
				}
				return std::string((char*)(&(_buffer[0])));
			}
		}
		inline static std::string encodeHex (const unsigned char* _pszBuffer, unsigned int _uLength, bool _bLowerCase)
		{
			std::string strRt = "";
			char cBase = 'A';
			if (_bLowerCase)
			{
				cBase = 'a';
			}
			for (unsigned int i = 0; i < _uLength; ++i)
			{
				unsigned char uFirst = _pszBuffer[i] / 16;
				unsigned char uSecond = _pszBuffer[i] % 16;
				if (uFirst < 10)
				{
					strRt += ('0' + (char)uFirst);
				}
				else
				{
					strRt += (cBase + (char)uFirst - 10);
				}
				if (uSecond < 10)
				{
					strRt += ('0' + (char)uSecond);
				}
				else
				{
					strRt += (cBase + (char)uSecond - 10);
				}
			}
			return strRt;
		}
		inline static std::string encodeHex (const std::vector<unsigned char>& _buffer, bool _bLowerCase)
		{
			return encodeHex((unsigned char*)(&_buffer[0]), (unsigned int)_buffer.size(), _bLowerCase);
		}
		inline static std::string encodeHex (const std::string& _str, bool _bLowerCase, bool _bWithEnding)
		{
			return encodeHex((unsigned char*)(_str.c_str()), (unsigned int)(_str.length() + _bWithEnding), _bLowerCase);
		}
		inline static bool decodeHex (const std::string& _str, std::vector<unsigned char>& _buffer)
		{
			_buffer.resize(0);
			if (0 != _str.length() % 2)
			{
				return false;
			}
			for (unsigned int i = 0; i < _str.length(); i += 2)
			{
				unsigned char uFirst = 0;
				unsigned char uSecond = 0;
				if (_str[i] >= '0' && _str[i] <= '9')
				{
					uFirst = (unsigned char)(_str[i] - '0');
				}
				else if (_str[i] >= 'A' && _str[i] <= 'F')
				{
					uFirst = (unsigned char)(_str[i] - 'A' + 10);
				}
				else if (_str[i] >= 'a' && _str[i] <= 'f')
				{
					uFirst = (unsigned char)(_str[i] - 'a' + 10);
				}
				else
				{
					return false;
				}
				if (_str[i + 1] >= '0' && _str[i + 1] <= '9')
				{
					uSecond = (unsigned char)(_str[i + 1] - '0');
				}
				else if (_str[i + 1] >= 'A' && _str[i + 1] <= 'F')
				{
					uSecond = (unsigned char)(_str[i + 1] - 'A' + 10);
				}
				else if (_str[i + 1] >= 'a' && _str[i + 1] <= 'f')
				{
					uSecond = (unsigned char)(_str[i + 1] - 'a' + 10);
				}
				else
				{
					return false;
				}
				_buffer.push_back(uFirst * 16 + uSecond);
			}
			return true;
		}
		inline static std::string decodeHex (const std::string& _str, bool _bWithEnding)
		{
			std::vector<unsigned char> _buffer;
			if (!decodeHex(_str, _buffer))
			{
				return "";
			}
			else
			{
				for (unsigned int i = 0; i < _buffer.size(); ++i)
				{
					if (0 == _buffer[i])
					{
						if (!_bWithEnding)
						{
							return "";
						}
						else
						{
							if (i != _buffer.size() -1)
							{
								return "";
							}
						}
					}
				}
				if (_bWithEnding)
				{
					if (0 != _buffer[_buffer.size() - 1])
					{
						return "";
					}
				}
				if (!_bWithEnding)
				{
					_buffer.push_back(0);
				}
				return std::string((char*)(&(_buffer[0])));
			}
		}
		inline static bool encryptAes256(const unsigned char* _pszKey, const unsigned char* _pszInput, unsigned int _uKeyLength, unsigned int _uInputLen, std::vector<unsigned char>& _Output)
		{
			EVP_CIPHER_CTX* pCtx = EVP_CIPHER_CTX_new();
			unsigned char szKey[EVP_MAX_KEY_LENGTH];
			unsigned char szIv[EVP_MAX_IV_LENGTH];
			int iKeyLen =
			EVP_BytesToKey
			(
				g_pAes256CtrCipher,
				EVP_md5(),
				0,
				_pszKey,
				(int)(_uKeyLength),
				1,
				szKey,
				szIv
			);
			EVP_CipherInit_ex(pCtx, g_pAes256CtrCipher, 0, 0, 0, 0);
			if (!EVP_CIPHER_CTX_set_key_length(pCtx, iKeyLen))
			{
				EVP_CIPHER_CTX_cleanup(pCtx);
				EVP_CIPHER_CTX_free(pCtx);
				return false;
			}
			EVP_CipherInit_ex
			(
				pCtx,
				NULL,
				NULL,
				reinterpret_cast<unsigned char*>(szKey),
				reinterpret_cast<unsigned char*>(szIv),
				0
			);
			int iUpdateLen = (int)_uInputLen + EVP_CIPHER_CTX_block_size(pCtx);
			int iFinalEnd = EVP_CIPHER_CTX_block_size(pCtx);
			_Output.resize((unsigned int)(iUpdateLen + iFinalEnd));
			if(!EVP_CipherUpdate
			(
				pCtx,
				(&_Output[0]),
				&iUpdateLen,
				_pszInput,
				(int)_uInputLen
			))
			{
				EVP_CIPHER_CTX_cleanup(pCtx);
				EVP_CIPHER_CTX_free(pCtx);
				return false;
			}
			if (!EVP_CipherFinal_ex(pCtx, (&_Output[0 + iUpdateLen]), &iFinalEnd))
			{
				EVP_CIPHER_CTX_cleanup(pCtx);
				EVP_CIPHER_CTX_free(pCtx);
				return false;
			}
			EVP_CIPHER_CTX_cleanup(pCtx);
			EVP_CIPHER_CTX_free(pCtx);
			_Output.resize((unsigned int)(iUpdateLen + iFinalEnd));
			return true;
		}
		inline static bool encryptAes256(const unsigned char* _pszKey, unsigned int _uKeyLength, const std::vector<unsigned char>& _Input, std::vector<unsigned char>& _Output)
		{
			return encryptAes256(_pszKey, &_Input[0], _uKeyLength, _Input.size(), _Output);
		}
		inline static bool encryptAes256(const std::vector<unsigned char>& vecKey, const unsigned char* _pszInput, unsigned int _uInputLen, std::vector<unsigned char>& _Output)
		{
			return encryptAes256(&vecKey[0], _pszInput, vecKey.size(), _uInputLen, _Output);
		}
		inline static bool encryptAes256(const std::vector<unsigned char>& vecKey, const std::vector<unsigned char>& _Input, std::vector<unsigned char>& _Output)
		{
			return encryptAes256(&vecKey[0], &_Input[0], vecKey.size(), _Input.size(), _Output);
		}
		inline static bool encryptAes256(const std::string& strKey, const unsigned char* _pszInput, unsigned int _uInputLen, std::vector<unsigned char>& _Output)
		{
			std::vector<unsigned char> vecKey;
			vecKey.resize(strKey.length());
			memcpy(&vecKey[0], strKey.c_str(), vecKey.size());
			return encryptAes256(vecKey, _pszInput, _uInputLen, _Output);
		}
		inline static bool encryptAes256(const std::string& strKey, const std::vector<unsigned char>& _Input, std::vector<unsigned char>& _Output)
		{
			return encryptAes256(strKey, &_Input[0], _Input.size(), _Output);
		}
		inline static std::string encryptAes256Base64(const unsigned char* _pszKey, const unsigned char* _pszInput, unsigned int _uKeyLength, unsigned int _uInputLen)
		{
			std::vector<unsigned char> OutPut;
			if (encryptAes256(_pszKey, _pszInput, _uKeyLength, _uInputLen, OutPut))
			{
				return encodeBase64 (OutPut);
			}
			else
			{
				return "";
			}
		}
		inline static std::string encryptAes256Base64(const unsigned char* _pszKey, unsigned int _uKeyLength, const std::vector<unsigned char>& _Input)
		{
			return encryptAes256Base64(_pszKey, &_Input[0], _uKeyLength, _Input.size());
		}
		inline static std::string encryptAes256Base64(const std::vector<unsigned char>& vecKey, const unsigned char* _pszInput, unsigned int _uInputLen)
		{
			return encryptAes256Base64(&vecKey[0], _pszInput, vecKey.size(), _uInputLen);
		}
		inline static std::string encryptAes256Base64(const std::vector<unsigned char>& vecKey, const std::vector<unsigned char>& _Input)
		{
			return encryptAes256Base64(&vecKey[0], &_Input[0], vecKey.size(), _Input.size());
		}
		inline static std::string encryptAes256Base64(const std::string& strKey, const unsigned char* _pszInput, unsigned int _uInputLen)
		{
			std::vector<unsigned char> vecKey;
			vecKey.resize(strKey.length());
			memcpy(&vecKey[0], strKey.c_str(), vecKey.size());
			return encryptAes256Base64(&vecKey[0], _pszInput, vecKey.size(), _uInputLen);
		}
		inline static std::string encryptAes256Base64(const std::string& strKey, const std::vector<unsigned char>& _Input)
		{
			return encryptAes256Base64(strKey, &_Input[0], _Input.size());
		}
		inline static std::string encryptAes256Hex(const unsigned char* _pszKey, const unsigned char* _pszInput, unsigned int _uKeyLength, unsigned int _uInputLen, bool _bLowerCase)
		{
			std::vector<unsigned char> OutPut;
			if (encryptAes256(_pszKey, _pszInput, _uKeyLength, _uInputLen, OutPut))
			{
				return encodeHex (OutPut, _bLowerCase);
			}
			else
			{
				return "";
			}
		}
		inline static std::string encryptAes256Hex(const unsigned char* _pszKey, unsigned int _uKeyLength, const std::vector<unsigned char>& _Input, bool _bLowerCase)
		{
			return encryptAes256Hex(_pszKey, &_Input[0], _uKeyLength, _Input.size(), _bLowerCase);
		}
		inline static std::string encryptAes256Hex(const std::vector<unsigned char>& vecKey, const unsigned char* _pszInput, unsigned int _uInputLen, bool _bLowerCase)
		{
			return encryptAes256Hex(&vecKey[0], _pszInput, vecKey.size(), _uInputLen, _bLowerCase);
		}
		inline static std::string encryptAes256Hex(const std::vector<unsigned char>& vecKey, const std::vector<unsigned char>& _Input, bool _bLowerCase)
		{
			return encryptAes256Hex(&vecKey[0], &_Input[0], vecKey.size(), _Input.size(), _bLowerCase);
		}
		inline static std::string encryptAes256Hex(const std::string& strKey, const unsigned char* _pszInput, unsigned int _uInputLen, bool _bLowerCase)
		{
			std::vector<unsigned char> vecKey;
			vecKey.resize(strKey.length());
			memcpy(&vecKey[0], strKey.c_str(), vecKey.size());
			return encryptAes256Hex(&vecKey[0], _pszInput, vecKey.size(), _uInputLen, _bLowerCase);
		}
		inline static std::string encryptAes256Hex(const std::string& strKey, const std::vector<unsigned char>& _Input, bool _bLowerCase)
		{
			return encryptAes256Hex(strKey, &_Input[0], _Input.size(), _bLowerCase);
		}
		inline static bool encryptStringAes256(const unsigned char* _pszKey, unsigned int _uKeyLength, const std::string& _strInput, std::vector<unsigned char>& _Output, bool _bWithEnding)
		{
			return encryptAes256(_pszKey, (unsigned char*)(_strInput.c_str()), _uKeyLength, (unsigned int)(_strInput.length() + _bWithEnding), _Output);
		}
		inline static bool encryptStringAes256(const std::string& strKey, const std::string& _strInput, std::vector<unsigned char>& _Output, bool _bWithEnding)
		{
			return encryptAes256(strKey, (unsigned char*)(_strInput.c_str()), (unsigned int)(_strInput.length() + _bWithEnding), _Output);
		}
		inline static std::string encryptStringAes256Base64(const unsigned char* _pszKey, unsigned int _uKeyLength, const std::string& _strInput, bool _bWithEnding)
		{
			std::vector<unsigned char> OutPut;
			if (encryptStringAes256(_pszKey, _uKeyLength, _strInput, OutPut, _bWithEnding))
			{
				return encodeBase64 (OutPut);
			}
			else
			{
				return "";
			}
		}
		inline static std::string encryptStringAes256Base64(const std::string& strKey, const std::string& _strInput, bool _bWithEnding)
		{
			return encryptStringAes256Base64((unsigned char*)strKey.c_str(), (unsigned int)strKey.length(), _strInput, _bWithEnding);
		}
		inline static std::string encryptStringAes256Hex(const unsigned char* _pszKey, unsigned int _uKeyLength, const std::string& _strInput,  bool _bLowerCase, bool _bWithEnding)
		{
			std::vector<unsigned char> OutPut;
			if (encryptStringAes256(_pszKey, _uKeyLength, _strInput, OutPut, _bWithEnding))
			{
				encodeHex (OutPut, _bLowerCase);
			}
			else
			{
				return "";
			}
		}
		inline static std::string encryptStringAes256Hex(const std::string& strKey, const std::string& _strInput,  bool _bLowerCase, bool _bWithEnding)
		{
			return encryptStringAes256Hex((unsigned char*)strKey.c_str(), (unsigned int)strKey.length(), _strInput, _bLowerCase, _bWithEnding);
		}
		inline static bool decryptBase64Aes256(const unsigned char* _pszKey, unsigned int _uKeyLength, const std::string& strBase64, std::vector<unsigned char>& _Output)
		{
			std::vector<unsigned char> Base64Output;
			if (!decodeBase64(strBase64, Base64Output))
			{
				return false;
			}
			if (!encryptAes256(_pszKey, _uKeyLength, Base64Output, _Output))
			{
				return false;
			}
			return true;
		}
		inline static bool decryptBase64Aes256(const std::string& strKey, const std::string& strBase64, std::vector<unsigned char>& _Output)
		{
			return decryptBase64Aes256((unsigned char*)strKey.c_str(), (unsigned int)strKey.length(), strBase64, _Output);
		}
		inline static std::string decryptBase64Aes256(const unsigned char* _pszKey, unsigned int _uKeyLength, const std::string& strBase64, bool _bWithEnding)
		{
			std::vector<unsigned char> _buffer;
			if (!decryptBase64Aes256(_pszKey, _uKeyLength, strBase64, _buffer))
			{
				return "";
			}
			else
			{
				for (unsigned int i = 0; i < _buffer.size(); ++i)
				{
					if (0 == _buffer[i])
					{
						if (!_bWithEnding)
						{
							return "";
						}
						else
						{
							if (i != _buffer.size() -1)
							{
								return "";
							}
						}
					}
				}
				if (_bWithEnding)
				{
					if (0 != _buffer[_buffer.size() - 1])
					{
						return "";
					}
				}
				if (!_bWithEnding)
				{
					_buffer.push_back(0);
				}
				return std::string((char*)(&(_buffer[0])));
			}
		}
		inline static std::string decryptBase64Aes256(const std::string& strKey, const std::string& strBase64, bool _bWithEnding)
		{
			return decryptBase64Aes256((unsigned char*)strKey.c_str(), (unsigned int)strKey.length(), strBase64, _bWithEnding);
		}
		inline static bool decryptHexAes256(const unsigned char* _pszKey, unsigned int _uKeyLength, const std::string& strHex, std::vector<unsigned char>& _Output)
		{
			std::vector<unsigned char> Base64Output;
			if (!decodeHex(strHex, Base64Output))
			{
				return false;
			}
			if (!encryptAes256(_pszKey, _uKeyLength, Base64Output, _Output))
			{
				return false;
			}
			return true;
		}
		inline static bool decryptHexAes256(const std::string& strKey, const std::string& strHex, std::vector<unsigned char>& _Output)
		{
			return decryptHexAes256((unsigned char*)strKey.c_str(), (unsigned int)strKey.length(), strHex, _Output);
		}
		inline static std::string decryptHexAes256(const unsigned char* _pszKey, unsigned int _uKeyLength, const std::string& strHex, bool _bWithEnding)
		{
			std::vector<unsigned char> _buffer;
			if (!decryptHexAes256(_pszKey, _uKeyLength, strHex, _buffer))
			{
				return "";
			}
			else
			{
				for (unsigned int i = 0; i < _buffer.size(); ++i)
				{
					if (0 == _buffer[i])
					{
						if (!_bWithEnding)
						{
							return "";
						}
						else
						{
							if (i != _buffer.size() -1)
							{
								return "";
							}
						}
					}
				}
				if (_bWithEnding)
				{
					if (0 != _buffer[_buffer.size() - 1])
					{
						return "";
					}
				}
				if (!_bWithEnding)
				{
					_buffer.push_back(0);
				}
				return std::string((char*)(&(_buffer[0])));
			}
		}
		inline static std::string decryptHexAes256(const std::string& strKey, const std::string& strHex, bool _bWithEnding)
		{
			return decryptHexAes256((unsigned char*)strKey.c_str(), (unsigned int)strKey.length(), strHex, _bWithEnding);
		}
		inline static void sha1(const unsigned char* _pszInput, unsigned int _uInputLen, std::vector<unsigned char>& _Output)
		{
			_Output.resize(20);
			SHA1(_pszInput, _uInputLen, (&_Output[0]));
		}
		inline static void sha1(const std::vector<unsigned char>& _Input, std::vector<unsigned char>& _Output)
		{
			sha1(&_Input[0], _Input.size(), _Output);
		}
		inline static void sha1String(const std::string& _strInput, std::vector<unsigned char>& _Output, bool _bWithEnding)
		{
			sha1((unsigned char*)((unsigned char*)_strInput.c_str()), (unsigned int)(_strInput.length() + _bWithEnding), _Output);
		}
		inline static std::string sha1Base64(const unsigned char* _pszInput, unsigned int _uInputLen)
		{
			std::vector<unsigned char> OutPut;
			sha1(_pszInput, _uInputLen, OutPut);
			return encodeBase64(OutPut);
		}
		inline static std::string sha1Base64(const std::vector<unsigned char>& _Input)
		{
			return sha1Base64(&_Input[0], _Input.size());
		}
		inline static std::string sha1StringBase64(const std::string& _strInput, bool _bWithEnding)
		{
			std::vector<unsigned char> Output;
			sha1((unsigned char*)((unsigned char*)_strInput.c_str()), (unsigned int)(_strInput.length() + _bWithEnding), Output);
			return encodeBase64(Output);
		}
		inline static std::string sha1Hex(const unsigned char* _pszInput, unsigned int _uInputLen, bool _bLowerCase)
		{
			std::vector<unsigned char> OutPut;
			sha1(_pszInput, _uInputLen, OutPut);
			return encodeHex(OutPut, _bLowerCase);
		}
		inline static std::string sha1Hex(const std::vector<unsigned char>& _Input, bool _bLowerCase)
		{
			return sha1Hex(&_Input[0], _Input.size(), _bLowerCase);
		}
		inline static std::string sha1StringHex(const std::string& _strInput, bool _bWithEnding, bool _bLowerCase)
		{
			std::vector<unsigned char> Output;
			sha1((unsigned char*)(_strInput.c_str()), (unsigned int)(_strInput.length() + _bWithEnding), Output);
			return encodeHex(Output, _bLowerCase);
		}
		inline static unsigned int getRand(size_t uMax)
		{
			struct timespec tp;
			clock_gettime(CLOCK_REALTIME, &tp);
			unsigned int uSeed = 0;
			memcpy (&uSeed, &tp.tv_nsec, sizeof(unsigned int));
			return (unsigned int)(((long double)(rand_r(&uSeed)) * (long double)(uMax)) / (long double)RAND_MAX);
		}
		inline static void getRandomArray(std::vector<unsigned int>& _Result, size_t _uSize)
		{
			_Result.resize(0);
			_Result.reserve(_uSize);
			std::set<unsigned int> valueSet;
			for (size_t i = 0; i < _uSize; ++i)
			{
				valueSet.insert(i);
			}
			while (true)
			{
				size_t uCurrentSize = valueSet.size();
				unsigned int uIdx = 0;
				unsigned int uRandomIdx = getRand(uCurrentSize);
				std::set<unsigned int>::iterator it = valueSet.begin();
				while (uIdx < uRandomIdx)
				{
					++it;
					++uIdx;
				}
				_Result.push_back(*it);
				valueSet.erase(it);
				if (0 == valueSet.size())
				{
					break;
				}
			}
		}
		inline static void getRandomMatrix(std::vector<std::vector<unsigned int>>& _Result, size_t _X, size_t _Y)
		{
			_Result.resize(_Y);
			for (size_t i = 0; i < _Y; ++i)
			{
				getRandomArray(_Result[i], _X);
			}
		}
		inline static void getRandomBytes(std::vector<unsigned char>& _Buffer, unsigned int _uSize)
		{
			_Buffer.resize(_uSize);
			for (size_t i = 0; i < _Buffer.size(); ++i)
			{
				_Buffer[i] = getRand(255);
			}
		}
		inline static std::string getRandomBytesBase64(unsigned int _uSize)
		{
			std::vector<unsigned char> _Buffer;
			getRandomBytes(_Buffer, _uSize);
			return encodeBase64(_Buffer);
		}
		inline static std::string getRandomBytesHex(unsigned int _uSize, bool _bLowerCase)
		{
			std::vector<unsigned char> _Buffer;
			getRandomBytes(_Buffer, _uSize);
			return encodeHex(_Buffer, _bLowerCase);
		}
	}
}
#endif


