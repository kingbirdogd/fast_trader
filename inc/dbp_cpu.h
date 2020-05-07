#ifndef __DBP_CPU__
#define __DBP_CPU__
#include <unistd.h>
#include <sys/syscall.h>
#include <vector>
namespace dbp
{
	namespace cpu
	{
		class CpuInfo
		{
		private:
			typedef std::vector<int> CCpuVec;
		private:
			CCpuVec m_Vec;
			int m_iCoreCnt;
			unsigned int m_uCurrentIdx;
		public:
			CpuInfo():
				m_Vec(),
				m_iCoreCnt(sysconf(_SC_NPROCESSORS_ONLN)),
				m_uCurrentIdx(0)
			{
			}
			~CpuInfo()
			{
			}
			CpuInfo(const CpuInfo&) = delete;
			CpuInfo(CpuInfo&&) = delete;
			CpuInfo& operator= (const CpuInfo&) = delete;
			CpuInfo& operator= (CpuInfo&&) = delete;
			bool setCpu(int _cpuId)
			{
				if (_cpuId < 0)
				{
					return false;
				}
				if (_cpuId >= m_iCoreCnt)
				{
					return false;
				}
				m_Vec.push_back(_cpuId);
				return true;
			}
			int getCore()
			{
				if (0 == m_Vec.size())
				{
					return -1;
				}
				int iRt = m_Vec[m_uCurrentIdx];
				++m_uCurrentIdx;
				if (m_uCurrentIdx == m_Vec.size())
				{
					m_uCurrentIdx = 0;
				}
				return iRt;
			}
		};
	}
}
#endif


