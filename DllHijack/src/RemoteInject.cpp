// RemoteInject.cpp : ʵ���ļ�
//

#include "RemoteInject.h"



void RemoteInject::OnBnClickedInject(std::string path)
{
	m_strDllPath = path;
	m_dwPID = GetCurrentProcessId();
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	HANDLE hThread2 = NULL;
	char* pszRemoteBuffer = NULL;
	DWORD * pDwTidRemote = NULL;



	//UpdateData(TRUE);


	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_dwPID);

	if (hProcess == NULL)
	{
		TipBox("�򿪽���ʧ��!!!!");
		return;
	}


	//1.��Զ�̽����з����ڴ�
	pszRemoteBuffer = (char *)VirtualAllocEx(hProcess, NULL, m_strDllPath.size(), MEM_COMMIT, PAGE_READWRITE);

	if (pszRemoteBuffer == NULL)
	{
		TipBox("����Զ�̿ռ�ʧ��");
		return;
	}
	//2.��Զ������ĵ�ַ����д��DLL��·��
	SIZE_T dwWriten;
	if (!WriteProcessMemory(hProcess, pszRemoteBuffer, (LPVOID)m_strDllPath.c_str(), m_strDllPath.length(), &dwWriten))
	{
		TipBox("д���ڴ�ʧ��");
	}

	//3.��ȡԶ�̽�����LaodLibry�ĵ�ַ,�������õ��ɺ���ÿ�������е�kernel32�ĵ�ַ�Ķ�һ����Զ����Ҳһ����
	HMODULE hMouDle = GetModuleHandleA("Kernel32");
	PTHREAD_START_ROUTINE pfnLoadLibrary = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32"), "LoadLibraryA");

	if (pfnLoadLibrary == NULL)
	{
		TipBox("��ȡLoadLibrary��ַʧ�ܣ�����");
		return;
	}

	//4.����Զ���߳�
	hThread = CreateRemoteThread(hProcess, NULL, 0, pfnLoadLibrary, pszRemoteBuffer, 0, NULL);

	DWORD dwErrCode = GetLastError();
	if (hThread == NULL)
	{
		TipBox("����Զ���߳�ʧ��");
		return;
	}

	WaitForSingleObject(hThread, 2000);
}
