#include "ProcessDetector.h"
#include "Framework.h"
#include <tlhelp32.h>  
#include <algorithm>
ProcessDetector::ProcessDetector()
{
	InitExeList();
}

ProcessDetector::~ProcessDetector()
{
}

bool ProcessDetector::FindProcess(std::wstring name)
{
	std::transform(name.begin(), name.end(), name.begin(), toupper);

	return std::find(_exeList.begin(), _exeList.end(), name) != _exeList.end();
}
// ö��ϵͳ��ǰ���н�����Ϣ  
// ������Ϣ���������Ŀ¼��EnumInfo_ToolHelp_process.txt  
bool ProcessDetector::EnumProcessInfo()
{
	// ���������Ϣ�ṹ  
	PROCESSENTRY32 pe32 = { sizeof(pe32) };

	// ����ϵͳ��ǰ���̿���  
	HANDLE hProcessShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessShot == INVALID_HANDLE_VALUE)
		return FALSE;

	
	if (Process32First(hProcessShot, &pe32))
	{
		do 
		{
			if (FindProcess(pe32.szExeFile))
			{
				wprintf(L" Find Processs: %s \n ", pe32.szExeFile);
			}

		} while (Process32Next(hProcessShot, &pe32));
	}

	CloseHandle(hProcessShot);
	return TRUE;
}

void ProcessDetector::InitExeList()
{
	//ö�����е�exe
	_exeList.push_back(L"OLLYICE.EXE");
	_exeList.push_back(L"IDAG.EXE");
	_exeList.push_back(L"OLLYDBG.EXE");
	_exeList.push_back(L"PEID.EXE");
	_exeList.push_back(L"LORDPE.EXE");
	_exeList.push_back(L"IMPORTREC.EXE");
	_exeList.push_back(L"W32DSM89.EXE");
	_exeList.push_back(L"WINDBG.EXE");
}

void ProcessDetector::Detect()
{
	EnumProcessInfo();
}
