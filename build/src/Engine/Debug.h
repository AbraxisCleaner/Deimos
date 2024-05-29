#if !defined(_DEBUG_H_)
#define _DEBUG_H_

#ifdef _DEBUG
#	define LOG(x) printf("<%s> :: " ## x, __FUNCTION__)
#	define LOGF(x, ...) printf("<%s> :: " ## x, __FUNCTION__, __VA_ARGS__)
#	define ASSERT(x) if (!(x)) { printf("---<Assertion Failed>---\nStatement: %s\nFile: %s\nLine: %d\n", #x, __FILE__, __LINE__); __debugbreak(); *(int*)0 = 0; } 
#	define VERIFY(x) ASSERT(x)
#else
#	define LOG(x)
#	define LOGF(x, ...)
#	define ASSERT(x) x
#	define VERIFY(x)
#endif // _DEBUG

#endif // _DEBUG_H_