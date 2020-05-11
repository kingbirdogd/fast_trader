#ifndef __DBP_MARCO_SRV__
#define __DBP_MARCO_SRV__
#define macro_getString(_pszBuffer, _uOfferSet, _uSize) (std::string(_pszBuffer+_uOfferSet,_uSize))
#define flush_printf(msg, ...) printf(msg, __VA_ARGS__);fflush(stdout);
#define OMD_GET_VALUE(POINTER, OFFERSET, TYPE) (*((TYPE*)((void*)(((char*)((void*)POINTER)) + OFFERSET))))
#define OMD_GET_POINTER(POINTER, OFFERSET, TYPE) ((TYPE*)((void*)(((char*)((void*)POINTER)) + OFFERSET)))
#define OMD_GET_STR(POINTER, OFFERSET, LEN) macro_getString(((char*)((void*)POINTER)), OFFERSET, LEN)
#endif


