#pragma once

#include "..\stdafx.h"
// ProcessInject �Ի���

class ProcessInject 
{
public:
	std::string m_strExePath;
	std::string m_strDllPath;
	void OnBnClickedInject(std::string exePath, std::string dllPath);
	 void OnBnClickedFreemem();

};
