#pragma once

#include "stdafx.h"
// ProcessInject �Ի���

class ProcessInject 
{
public:
	void OnStartInject(std::string exePath, std::string dllPath);
	 void OnReleaseInject();
private:
	std::string m_strExePath;
	std::string m_strDllPath;

};
