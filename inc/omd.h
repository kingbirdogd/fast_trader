#ifndef __DBP_OMD__
#define __DBP_OMD__
namespace dbp
{
	namespace omd
	{
		#pragma pack(push, 1)
		class COmdPkgHeader
		{
		public:
			unsigned short int m_uPkgSize;
			unsigned char m_uMsgCnt;
			unsigned char m_uFillter;
			unsigned int m_uSeq;
			unsigned long long m_uTimeStamp;
		public:
			COmdPkgHeader():
				m_uPkgSize(0),
				m_uMsgCnt(0),
				m_uFillter(0),
				m_uSeq(0),
				m_uTimeStamp(0)
			{
			}
			~COmdPkgHeader()
			{
			}
		};
		class COmdMsgHeader
		{
		public:
			unsigned short int m_uMsgSize;
			unsigned short int m_uMsgType;
		public:
			COmdMsgHeader():
				m_uMsgSize(0),
				m_uMsgType(0)
			{
			}
			~COmdMsgHeader()
			{
			}
		};
		#pragma pack(pop)
	}
}
#endif


