#ifndef __DBP_TOOLS_SRV__
#define __DBP_TOOLS_SRV__
#include <string.h>
#include <signal.h>
#include <string>
#include <macro.h>
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
		}
	}
}
#endif


