// RegInject.cpp : ʵ���ļ�
//
#include "RegInject.h"





void RegInject::OnBnClickedButton2(std::string path)
{
	m_strDllPath = path;
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BOOL bRet = FALSE;
	HKEY hKey = NULL;
	LONG nReg;
	char szDllPath[MAX_PATH] = { 0 };

	//��HKEY_LOCAL_MACHINE/Software/Microsoft/WindowsNT/CurrentVersion/Windows
	nReg = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows",
		0,
		KEY_ALL_ACCESS,
		&hKey);

	if (nReg != ERROR_SUCCESS)
	{
		TipBox("��ע���ʧ��");
		RegCloseKey(hKey);
		return;
	}

	//����AppInit_DLLs�ļ�ֵΪ���ǵ�Dll
	nReg = RegSetValueExA(hKey,
		"AppInit_DLLs",
		0,
		REG_SZ,
		//(byte *)m_strDllPath.GetBuffer(0),
		NULL,
		m_strDllPath.size()
		);
	if (nReg != ERROR_SUCCESS)
	{
		TipBox("����ע���ʧ�ܣ�");
		RegCloseKey(hKey);
		return;
	}

	RegCloseKey(hKey);
	return;
}
