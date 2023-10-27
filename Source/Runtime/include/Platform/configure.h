#if __has_include("stdint.h")
#include <stdint.h>
#endif

#ifndef __cplusplus
	#include <stdbool.h>
#endif

#ifndef PANDA_EXPORT
	#if defined(_MSC_VER)
		#define PANDA_EXPORT __declspec(dllexport)
	#else
		#define PANDA_EXPORT __attribute__((visibility("default")))
	#endif
#endif

#ifndef PANDA_IMPORT
	#if defined(_MSC_VER)
		#define PANDA_IMPORT __declspec(dllimport)
	#else
		#define PANDA_IMPORT __attribute__((visibility("default")))
	#endif
#endif

/*Defined in the build system*/
#ifndef PANDA_RUNTIME_API
	#ifdef RUNTIME_SHARED		
		#if defined(_MSC_VER)
			#define PANDA_RUNTIME_API __declspec(dllimport)
			#define PANDA_RUNTIME_LOCAL
		#elif defined(__CYGWIN__)
			#define PANDA_RUNTIME_API __attribute__((dllimport))
			#define PANDA_RUNTIME_LOCAL
		#elif (defined(__GNUC__) && (__GNUC__ >= 4))
			#define PANDA_RUNTIME_API __attribute__((visibility("default")))
			#define PANDA_RUNTIME_LOCAL __attribute__((visibility("hidden")))
		#else
			#define PANDA_RUNTIME_API
			#define PANDA_RUNTIME_LOCAL
		#endif
	#else
		#define PANDA_RUNTIME_API
		#define PANDA_RUNTIME_LOCAL
#endif
#endif


#if defined(__cplusplus)
	#define PE_UNUSED [[maybe_unused]]
#elif defined(__GNUC__) || defined(__clang__)
	#define PE_UNUSED __attribute__((unused))
#elif defined(_MSC_VER)
	#define PE_UNUSED 
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif
#define DEFINE_ALIGNED(def, a) __declspec(align(a)) def
#else
#ifndef FORCEINLINE
#define FORCEINLINE inline __attribute__((always_inline))
#endif
#define DEFINE_ALIGNED(def, a) __attribute__((aligned(a))) def
#endif