#include "StdAfx.h"
#include "QTest.h"

CQTest::CQTest(void)
{
}

CQTest::~CQTest(void)
{
}

float CQTest::GetTest(std::map<int,int>& chq)
{
	CSingleLock chQ_lock(&m_chQ_Lock,TRUE);
	
	std::map<int,int>::iterator i;
	int sum = 0, sum0 = 0;
	for( i = chq.begin(); i != chq.end(); ++i)
	{
		if( i->first == 0 )
			sum0 += i->second;
		sum += i->second;
	}
	
	if( sum == sum0 )
		return 0;
	return sum;
}

float CQTest::GetTestProc()
{
	CSingleLock chQ_lock(&m_chQ_Lock,TRUE);
	
	std::map<int,int>& chq = m_chQG;

	std::map<int,int>::iterator i;
	int sum = 0, sum0 = 0,sum1 = 0;
	for( i = chq.begin(); i != chq.end(); ++i)
	{
		if( i->first == 0 )
			sum0 += i->second;
		else
			sum1 += i->second;

		sum += i->second;
	}
	
	if( sum == sum0 || sum <= 0 )
		return 0;

	return 100.0f * sum1 / sum;
}