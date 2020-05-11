#ifndef __DBP_MARCO_SRV__
#define __DBP_MARCO_SRV__
#define flush_printf(msg, ...) printf(msg, __VA_ARGS__);fflush(stdout);
#define OMD_GET_VALUE(POINTER, OFFERSET, TYPE) (*((TYPE*)((void*)(((char*)((void*)POINTER)) + OFFERSET))))
#define OMD_GET_POINTER(POINTER, OFFERSET, TYPE) ((TYPE*)((void*)(((char*)((void*)POINTER)) + OFFERSET)))
#define OMD_GET_STR(POINTER, OFFERSET, LEN) getString(((char*)((void*)POINTER)), OFFERSET, LEN)
#endif


