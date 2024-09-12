#pragma once

#include <mutex>
class CNewVarEvent
{
public:
	CNewVarEvent();
	virtual ~CNewVarEvent();
public:
	static std::recursive_mutex			m_lock;
	static std::list<CNewVarEvent *>	m_listeners;

	void PushVar(int id, int subID, int core);

	virtual void OnNewVar(int id, int subID) {}
	virtual void OnNewVar(int id, int subID, int core);
	virtual int getCore() = 0;
};

