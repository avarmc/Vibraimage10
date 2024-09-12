#pragma once

class CQTest
{
public:
	std::map<int,int>	m_chQL;
	std::map<int,int>	m_chQG;
	CCriticalSection	m_chQ_Lock;

public:
	CQTest(void);
	~CQTest(void);


	void PutCHQ(std::map<int,int>& chq,int q) { CSingleLock lock(&m_chQ_Lock,TRUE); if(q >= 0) chq[q] ++; else chq.clear(); }

	void PutCHQ(int q) { PutCHQ(m_chQL,q); PutCHQ(m_chQG,q); }

	void Reset()
	{
		m_chQL.clear();
		m_chQG.clear();
	}

	float GetTest(std::map<int,int>& chq);
	float GetTestProc();
	
};
