#pragma once

#include "stdafx.h"
// RemoteInject �Ի���

class RemoteInject
{
public:

	void StartInject(std::string stExe, std::string strDll);
private:
	DWORD GetProcessIdByName(std::string name);
private:
	DWORD m_dwPID;
	std::string m_strDllPath;
};
