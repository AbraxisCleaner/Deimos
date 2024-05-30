#if !defined(_ENGINE_DEBUG_LOG_H_)
#define _ENGINE_DEBUG_LOG_H_

#if !defined(_DEBUG)
#   define LOG(x) 
#   define LOGF(x, ...)
#   define ASSERT(x) x
#   define VERIFY(x)
#else
#   include <assert.h>
#   define LOG(x) wprintf(L"<%s> :: " ## x, __FUNCTION__)
#   define LOGF(x, ...) wprintf(L"<%s> :: " ## #x, __FUNCTION__, __VA_ARGS__)
#   define ASSERT(x) assert(x)
#   define VERIFY(x) assert(x)
#endif

#endif // _ENGINE_DEBUG_LOG_H_