#pragma once
#include "..\stdafx.h"

// ThreadInject �Ի���

class ThreadInject 
{
public:
	// ����ID
	DWORD m_dwPid;
	std::string m_strDllPath;
	void OnBnClickedButton2();
	void OnBnClickedButton3(std::string path);
	void OnBnClickedButton4();
};
