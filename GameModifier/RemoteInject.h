#pragma once

#include "stdafx.h"
// RemoteInject �Ի���

class RemoteInject
{
public:
	DWORD m_dwPID;
	std::string m_strDllPath;
	void OnBnClickedInject(std::string path);
};
