#pragma once

#include <windows.h>
// HookInject �Ի���

class HookInject 
{
public:
	// ����ID
	DWORD m_dwPid;
	std::string m_strDllPath;
	 void OnBnClickedButton2();
	 void StartInject(std::wstring stExe, std::string strDll);
	 void OnBnClickedUnhook();
};
