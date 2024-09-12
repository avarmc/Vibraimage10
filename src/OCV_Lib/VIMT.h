#pragma once

class CMTCriticalSection
{
public:
	CRITICAL_SECTION m_CriticalSection;
public:
	CMTCriticalSection(void);
	CMTCriticalSection(const CMTCriticalSection& i);
	~CMTCriticalSection(void);
public:
	bool IsLocked();
};

inline CMTCriticalSection::CMTCriticalSection(void)
{
	BOOL ok = InitializeCriticalSectionAndSpinCount(&m_CriticalSection,0);
}
inline CMTCriticalSection::CMTCriticalSection(const CMTCriticalSection& i)
{
	BOOL ok = InitializeCriticalSectionAndSpinCount(&m_CriticalSection,0);
}

inline CMTCriticalSection::~CMTCriticalSection(void)
{
	DeleteCriticalSection(&m_CriticalSection);
}

inline bool CMTCriticalSection::IsLocked()
{
	return TryEnterCriticalSection(&m_CriticalSection) ? true:false;
}

class CMTSingleLock
{
protected:
	CMTCriticalSection *m_pLock;
	bool				m_bLock;
public:
	CMTSingleLock(CMTCriticalSection* pLock,bool bLock);
	~CMTSingleLock(void);
public:
	void Lock();
	void Unlock();
	bool IsLocked();
	bool IsStarted();
};


inline CMTSingleLock::CMTSingleLock(CMTCriticalSection* pLock,bool bLock)
{
	m_pLock = pLock;
	m_bLock = false;
	if(bLock)
		Lock();
}

inline CMTSingleLock::~CMTSingleLock(void)
{
	if(IsStarted())
		Unlock();
}

inline void CMTSingleLock::Lock()
{
	if(m_bLock)
		return;
	m_bLock = true;
	EnterCriticalSection(&(m_pLock->m_CriticalSection)); 
}

inline void CMTSingleLock::Unlock()
{
	if(!m_bLock)
		return;
	m_bLock = false;
	LeaveCriticalSection(&(m_pLock->m_CriticalSection));
}

inline bool CMTSingleLock::IsLocked()
{
	return m_pLock->IsLocked();
}

inline bool CMTSingleLock::IsStarted()
{
	return m_bLock;
}

template <class T>
class CMTObject : public CMTCriticalSection
{
public:
	T v;
public:
	CMTObject() {};
	~CMTObject() {};
};