#ifndef CORE_SERVICE_INC_SERVICE_HPP_
#define CORE_SERVICE_INC_SERVICE_HPP_

namespace core
{
	class service
	{
	public:
		virtual ~service() = default;
		virtual bool run() = 0;
	};
}



#endif /* CORE_SERVICE_INC_SERVICE_HPP_ */
