#pragma once
#include "..\stdafx.h"

// InputInject �Ի���

class InputInject 
{
public:
	DWORD m_dwPID;
	std::string m_strDllPath;
	 void OnBnClickedInject(std::string dllPath);
};
