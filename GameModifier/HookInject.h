#pragma once

#include <windows.h>
// HookInject �Ի���

class HookInject 
{
public:
	// ����ID
	DWORD m_dwPid;
	std::wstring m_strDllPath;
	 void OnBnClickedButton2();
	 void StartInject(std::wstring stExe, std::wstring strDll);
	 void OnBnClickedUnhook();
};
