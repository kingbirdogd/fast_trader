#ifndef __DBP_INTERNAL_CRYTPO__
#define __DBP_INTERNAL_CRYTPO__
#include <srv/dbp_crypto.h>
#include <srv/dbp_private_key.h>
namespace dbp
{
	namespace crypto
	{
		inline static std::string getDbpAesPassWithKey (const std::string& _strKey1, const std::string& _strKey2)
		{
			std::string strRt = "";
			for (unsigned int i = 0; i < 32; ++i)
			{
				unsigned int idx = g_AesIdxArray[i][0];
				if (0 == idx)
				{
					strRt += _strKey1[g_AesPassArray[idx][i]];
				}
				else
				{
					strRt += _strKey2[g_AesPassArray[idx][i]];
				}
				idx = g_AesIdxArray[i][1];
				if (0 == idx)
				{
					strRt += _strKey1[g_AesPassArray[idx][i]];
				}
				else
				{
					strRt += _strKey2[g_AesPassArray[idx][i]];
				}
			}
			return strRt;
		}
		inline static std::string decodeDbpAesPass (const std::string& _str)
		{
			if (64 != _str.length())
			{
				return "";
			};
			char szBuffer[33];
			memset (szBuffer, 0, 33);
			for (unsigned int i = 0; i < 32; ++i)
			{
				unsigned int idx = g_AesIdxArray[i][0];
				if (0 == idx)
				{
					szBuffer[g_AesPassArray[0][i]] = _str[i * 2];
				}
				else
				{
					szBuffer[g_AesPassArray[0][i]] = _str[i * 2 + 1];
				}
			}
			return szBuffer;
		}
		inline static std::string encryptDbpAes256 (const std::string& _str)
		{
			std::string strKey = getRandomBytesBase64(24);
			std::string strRandomNothing = getRandomBytesBase64(24);
			std::string strEncriptKey = getDbpAesPassWithKey(g_strAesPass, strKey);
			std::string strBaseOutput = encryptStringAes256Base64(strEncriptKey, _str, true);
			std::string strRandomOutputPass = getDbpAesPassWithKey(strKey, strRandomNothing);
			unsigned int iFirst = strBaseOutput.length() / 2;
			std::string strRt = strRandomOutputPass.substr(0, 32);
			strRt += strBaseOutput.substr(0, iFirst);
			strRt += strRandomOutputPass.substr(32, 32);
			strRt += strBaseOutput.substr(iFirst, strBaseOutput.length() - iFirst);
			return strRt;
		}
		inline static std::string decryptDbpAes256(const std::string& _str)
		{
			if (_str.length() < 64)
			{
				return "";
			}
			unsigned int uOriLength = _str.length() - 64;
			unsigned int uFirstPart = uOriLength / 2;
			std::string strRandomOutputPass = _str.substr(0, 32);
			std::string strBaseOutput = _str.substr(32, uFirstPart);
			strRandomOutputPass += _str.substr(32 + uFirstPart, 32);
			strBaseOutput += _str.substr(64 + uFirstPart);
			std::string strKey = decodeDbpAesPass(strRandomOutputPass);
			std::string strEncriptKey = getDbpAesPassWithKey(g_strAesPass, strKey);
			return decryptBase64Aes256(strEncriptKey, strBaseOutput, true);
		}

	}
}
#endif



