#pragma once
#include <map>
#include <string>
#include <mutex>

class CInc
{
LONG *p;
LONG cur;
public:
	CInc(unsigned int *_p):p((LONG*)_p) { if(p) cur = InterlockedIncrement(p); }
	CInc(int *_p):p((LONG*)_p) { if(p) cur = InterlockedIncrement(p); }
	CInc(long *_p):p((LONG*)_p) { if(p) cur = InterlockedIncrement(p); }
	CInc(unsigned long *_p):p((LONG*)_p) { if(p) cur = InterlockedIncrement(p); }

	~CInc() {if(p) InterlockedDecrement(p);}
	LONG get() { return cur; } 
};

class CIncStr
{
	static std::recursive_mutex lock;
	std::map<std::string, HANDLE>* p;
	std::string cur;
public:
	CIncStr(std::map<std::string, HANDLE>* a, std::string s)  { 
		std::unique_lock<std::recursive_mutex> ulock(lock);
		p = a;
		cur = s;
		(*p)[cur] = GetCurrentThread();
	}


	~CIncStr() { 
		std::unique_lock<std::recursive_mutex> ulock(lock);
		if (p) p->erase(cur); 
	}
	
};