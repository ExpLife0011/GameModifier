#pragma once
#include "stdafx.h"

// ThreadInject �Ի���

class ThreadInject 
{
public:
	void StartInject(std::wstring stExe, std::wstring strDll);
	void OnReleaseInject();
private:
	// ����ID
	DWORD m_dwPid;
	std::wstring m_strDllPath;
	std::wstring m_strExePath;

};
