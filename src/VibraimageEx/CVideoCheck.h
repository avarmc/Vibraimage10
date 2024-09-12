#pragma once

class CVideoCheck
{
	int			m_id;
	ULONGLONG	m_tFrame;
	double		m_tVideo;
	ULONGLONG	m_nFrame;

	CString		m_docRestart;
	ULONGLONG	m_devRestartTimer;
public:
	CVideoCheck(int core);
	void Check();
	void OnTimeout();
	void Restart();
	static void CheckAll();
};

