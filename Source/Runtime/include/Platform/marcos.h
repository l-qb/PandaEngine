#pragma once



#if __cplusplus >= 201100L
	#define P_UTF8(str) u8##str
#else
	#define P_UTF8(str) str
#endif

