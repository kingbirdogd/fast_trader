#ifndef __DBP_TOOLS_SRV__
#define __DBP_TOOLS_SRV__
#include <string.h>
#include <signal.h>
#include <string>
#include <srv/macro.h>
namespace dbp
{
	namespace tools
	{
		namespace srv
		{
			inline static std::string getString(char* _pszBuffer, unsigned int _uOfferSet, unsigned int _uSize)
			{
				char szBuffer[2048];
				memset (szBuffer, 0, 2048);
				memcpy(szBuffer, _pszBuffer + _uOfferSet, _uSize);
				szBuffer[_uSize] = 0;
				return szBuffer;
			}
			inline static unsigned long long current()
			{
				struct timespec tp;
				clock_gettime(CLOCK_REALTIME, &tp);
				unsigned long long uRt = tp.tv_sec;
				uRt *= 1000000000;
				uRt += ((unsigned long long)((unsigned long long)tp.tv_nsec));
				return uRt;
			}
			inline static void blockSigPipe()
			{
				sigset_t signal_mask;
				sigemptyset (&signal_mask);
				sigaddset (&signal_mask, SIGPIPE);
				if (0 != pthread_sigmask (SIG_BLOCK, &signal_mask, nullptr))
				{
					flush_printf("tm:%llu, block sigpipe error\n", current());
				}
			}
			inline static void printErrorLog(const char* _pszUserName, const char* _pszErrorMsg)
			{
				flush_printf("tm:%llu, %s error stop websocket, error msg:%s\n", current(), _pszUserName, _pszErrorMsg);
			}
			class CSpread
			{
			public:
				const static unsigned int SpreadTableA[11][2];
				inline static unsigned int NextBidDiff (unsigned int iPrice)
				{
					unsigned int i = 0;
					unsigned int j = 10;
					if (0 == iPrice)
					{
						return 0;
					}
					while (j - i > 1)
					{
						unsigned int k = (i + j) / 2;
						if (iPrice <= SpreadTableA[k][0])
						{
							j = k - 1;
						}
						else
						{
							i = k;
						}
					}

					if (i == j)
					{
						return SpreadTableA[i][1];
					}
					else
					{
						return ((iPrice > SpreadTableA[j][0]) ? SpreadTableA[j][1] : SpreadTableA[i][1]);
					}
				}
				inline static unsigned int NextAskDiff (unsigned int iPrice)
				{
					unsigned int i = 0;
					unsigned int j = 10;
					if (0 == iPrice)
					{
						return 0;
					}

					while (j - i > 1)
					{
						unsigned int k = (i + j) / 2;
						if (iPrice < SpreadTableA[k][0])
						{
							j = k - 1;
						}
						else
						{
							i = k;
						}
					}

					if (i == j)
					{
						return SpreadTableA[i][1];
					}
					else
					{
						return ((iPrice >= SpreadTableA[j][0]) ? SpreadTableA[j][1] : SpreadTableA[i][1]);
					}
				}
			};
			const unsigned int CSpread::SpreadTableA[11][2] =
			{
				{10,1},
				{250,5},
				{500,10},
				{10000,20},
				{20000,50},
				{100000,100},
				{200000,200},
				{500000,500},
				{1000000,1000},
				{2000000,2000},
				{5000000,5000}
			};
			inline static unsigned int NextBidDiff (unsigned int iPrice)
			{
				return CSpread::NextBidDiff(iPrice);
			}
			inline static unsigned int NextAskDiff (unsigned int iPrice)
			{
				return CSpread::NextAskDiff(iPrice);
			}
		}
	}
}
#endif


