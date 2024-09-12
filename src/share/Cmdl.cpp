#include "StdAfx.h"
#include "Cmdl.h"

CCmdl::CCmdl(void)
{
}

CCmdl::~CCmdl(void)
{
}

void CCmdl::ParseParam(const TCHAR* pszParam,BOOL bFlag,BOOL bLast)
{
	if(bFlag)
		m_cmdl.push_back(CString(_T("/"))+pszParam);
	else
		m_cmdl.push_back(pszParam);
}

bool CCmdl::IsSet(LPCWSTR param,int count)
{
	int cmd_size = (int)m_cmdl.size();
	if (!cmd_size)
		return false;
	int last = min( cmd_size -count, cmd_size-1 );
	for(int i = 0; i <= last; ++i)
		if( m_cmdl[i].CompareNoCase(param) == 0 )
			return true;
	return false;
}

bool CCmdl::Remove(LPCWSTR param, int cnt) {

	for (int i = 0; i < m_cmdl.size(); ++i)
		if (m_cmdl[i].CompareNoCase(param) == 0) {
			for (int j = 0; j < cnt && ((i + j) < m_cmdl.size()); ++j)
				m_cmdl[i + j].Empty();
			return true;
		}
	return false;
}

void CCmdl::Replace(LPCWSTR param,LPCWSTR var,int shift)
{
	int last = ((int)m_cmdl.size()) -shift-1;
	for(int i = 0; i <= last; ++i)
		if( m_cmdl[i].CompareNoCase(param) == 0 )
		{
			m_cmdl[i+shift] = var;
			return;
		}
}

CString CCmdl::GetParam(int pos) {
	if (pos >= 0 && pos < ((int)m_cmdl.size()))
		return m_cmdl[pos];
	return CString();
}

CString CCmdl::GetParam(LPCWSTR param, int shift)
{
	int last = ((int)m_cmdl.size()) -shift;
	for(int i = 0; i < last; ++i)
		if( m_cmdl[i].CompareNoCase(param) == 0 )
			return m_cmdl[i+shift];

	return CString();
}

CString CCmdl::GetString(LPCWSTR separator)
{
	CString ret;
	int last = (int)m_cmdl.size();
	for(int i = 0; i < last; ++i)
	{
		if(i) ret += separator;
		ret += m_cmdl[i];
	}
	return ret;
}