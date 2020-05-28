#ifndef __DBP_TOOLS_SRV__
#define __DBP_TOOLS_SRV__
#include <vector>
#include <sys/stat.h>
#include <unistd.h>
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
			template <class Container>
			inline void split(const std::string& str, Container& cont,
			              const std::string& delims = " ")
			{
			    std::size_t current, previous = 0;
			    current = str.find_first_of(delims);
			    while (current != std::string::npos) {
			        cont.push_back(str.substr(previous, current - previous));
			        previous = current + 1;
			        current = str.find_first_of(delims, previous);
			    }
			    cont.push_back(str.substr(previous, current - previous));
			}
			inline int sub_process(const char* cmd, FILE*& input_stream, FILE*& output_stream)
			{
				int pid = 0;
				std::string command = cmd;
				std::vector<std::string> params;
				split(command, params);
				int inpipefd[2];
				int outpipefd[2];
				pipe(inpipefd);
				pipe(outpipefd);
				pid = ::fork();
				if (pid < 0)
				{
					pid = 0;
					return pid;
				}
				if (0 == pid)
				{
					std::vector<const char*> call_params;
					for (std::size_t i = 0; i < params.size(); ++i)
					{
						call_params.push_back(params[i].c_str());
					}
					call_params.push_back(nullptr);
					::close(outpipefd[1]);
					::close(inpipefd[0]);
					dup2(outpipefd[0], STDIN_FILENO);
					dup2(inpipefd[1], STDOUT_FILENO);
					::close(outpipefd[0]);
					::close(inpipefd[1]);
					char** ptr = const_cast<char**>(&call_params[0]);
					::execvp(ptr[0], ptr);
					exit(0);
				}
				else
				{
					sleep(1);
					::close(outpipefd[0]);
					::close(inpipefd[1]);
					output_stream = fdopen(outpipefd[1], "w");
					input_stream = fdopen(inpipefd[0], "r");
				}
				return pid;
			}
			inline std::string replace_env(const std::string& str)
			{
				auto len = str.length();
				std::string rt = "";
				std::size_t start_idx = 0;
				while (true)
				{
					std::size_t end_idx = str.find("${", start_idx);
					if (std::string::npos == end_idx)
					{
						rt += str.substr(start_idx, len  - start_idx);
						break;
					}
					else
					{
						rt += str.substr(start_idx, end_idx  - start_idx);
						start_idx = end_idx + 2;
						end_idx = str.find("}", start_idx);
						if (std::string::npos == end_idx)
						{
							rt += str.substr(start_idx, len  - start_idx);
							break;
						}
						else
						{
							auto key = str.substr(start_idx, end_idx  - start_idx);
							auto replace = std::getenv(key.c_str());
							if (nullptr == replace)
							{
								rt += "${";
								rt += key;
								rt += "}";
							}
							else
							{
								rt += replace;
							}
							start_idx = end_idx + 1;
						}
					}
				}
				return rt;
			}
			inline std::string get_line(FILE* stream)
			{
				char buffer [4096];
				std::string input = "";
				while ('\n' != input[input.length() - 1])
					input += ::fgets(buffer, 4096, stream);
				input = input.substr(0, input.length() - 1);
				return input;
			}
			inline void output(FILE* stream, const std::string& out)
			{
				fprintf(stream, "%s\n", out.c_str());
				fflush(stream);
			}
		}
	}
}
#endif


