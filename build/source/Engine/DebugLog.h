#if !defined(_ENGINE_DEBUG_LOG_H_)
#define _ENGINE_DEBUG_LOG_H_

#if !defined(_DEBUG)
#   define ASSERT(x) x
#   define VERIFY(x)
#else
#   include <assert.h>
#   define ASSERT(x) assert(x)
#   define VERIFY(x) assert(x)
#endif

#endif // _ENGINE_DEBUG_LOG_H_