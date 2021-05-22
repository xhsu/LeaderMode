#ifndef THREADTOOLS_H
#define THREADTOOLS_H

class CThreadSpinRWLock
{
public:
	inline void LockForWrite() {}
	inline void UnlockWrite() {}
	inline void LockForRead() {}
	inline void UnlockRead() {}
};

class CThreadFastMutex {};
class CInterlockedInt {};

#define AUTO_LOCK(...) ((void)0)

inline bool ThreadInterlockedAssignIf64(...) { return false; }
#define ThreadPause() ((void)0)

#endif