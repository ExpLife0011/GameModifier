#pragma once
#include <windows.h>
#include <string>
// APCInject �Ի���
class APCInject 
{
public:
	void StartInject(std::wstring stExe, std::wstring strDll);
private:
	
	std::wstring m_strExePath;
	std::wstring m_strDllPath;

};
