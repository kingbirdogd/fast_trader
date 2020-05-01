#ifndef CORE_MACRO_INC_FILE_NAME_HPP_
#define CORE_MACRO_INC_FILE_NAME_HPP_

namespace core
{
	template <unsigned long long S>
	inline constexpr unsigned long long get_file_name_offset(const char (& str)[S], unsigned long long i = S - 1)
	{
		return (str[i] == '/' || str[i] == '\\') ? i + 1 : (i > 0 ? get_file_name_offset(str, i - 1) : 0);
	}

	template <unsigned long long S>
	inline constexpr unsigned long long get_file_name_offset(const wchar_t (& str)[S], unsigned long long i = S - 1)
	{
	    return (str[i] == L'/' || str[i] == L'\\') ? i + 1 : (i > 0 ? get_file_name_offset(str, i - 1) : 0);
	}

	template <unsigned long long S>
	inline constexpr unsigned long long get_file_name_offset(const char16_t (& str)[S], unsigned long long i = S - 1)
	{
	    return (str[i] == u'/' || str[i] == u'\\') ? i + 1 : (i > 0 ? get_file_name_offset(str, i - 1) : 0);
	}

	template <unsigned long long S>
	inline constexpr unsigned long long get_file_name_offset(const char32_t (& str)[S], unsigned long long i = S - 1)
	{
	    return (str[i] == U'/' || str[i] == U'\\') ? i + 1 : (i > 0 ? get_file_name_offset(str, i - 1) : 0);
	}

	template <typename T>
	inline constexpr unsigned long long get_file_name_offset(T (& str)[1])
	{
	    return 0;
	}
}

#define __FILENAME__ (&__FILE__[get_file_name_offset(__FILE__)])




#endif /* CORE_MACRO_INC_FILE_NAME_HPP_ */
