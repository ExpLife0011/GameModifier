// InputInject.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyInjectTool.h"
#include "InputInject.h"
#include "afxdialogex.h"
#include <Imm.h>
#pragma comment(lib,"IMM32.lib")

// InputInject �Ի���

IMPLEMENT_DYNAMIC(InputInject, CDialogEx)

InputInject::InputInject(CWnd* pParent /*=NULL*/)
	: CDialogEx(InputInject::IDD, pParent)
	, m_dwPID(0)
	, m_strDllPath(_T(""))
{

}

InputInject::~InputInject()
{
}

void InputInject::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_dwPID);
	DDX_Text(pDX, IDC_EDIT4, m_strDllPath);
}


BEGIN_MESSAGE_MAP(InputInject, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &InputInject::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_INJECT, &InputInject::OnBnClickedInject)
END_MESSAGE_MAP()


// InputInject ��Ϣ�������

typedef struct
{
	HWND hWnd;
	DWORD dwPid;
}WNDINFO;

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	WNDINFO* pInfo = (WNDINFO*)lParam;
	DWORD dwProcessId = 0;
	GetWindowThreadProcessId(hWnd, &dwProcessId);

	if (dwProcessId == pInfo->dwPid)
	{
		pInfo->hWnd = hWnd;
		return FALSE;
	}
	return TRUE;
}

/*******************************************************
*��������:���ս���ID��ȡ�����ھ��
*��������:����1������ID
*��������:HWND
*ע������:��
*����޸�ʱ��:2017/5/13
*******************************************************/
HWND GetHwndByProcessId(DWORD dwProcessId)
{
	WNDINFO info = { 0 };
	info.hWnd = NULL;
	info.dwPid = dwProcessId;
	EnumWindows(EnumWindowsProc, (LPARAM)&info);
	return info.hWnd;
}
void InputInject::OnBnClickedButton2()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	char szFilter[] = "��̬���ӿ�|*.dll";
	CFileDialog fileDlg(TRUE, "dll", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	UpdateData(TRUE);
	if (fileDlg.DoModal() == IDOK)
	{
		m_strDllPath = fileDlg.GetPathName();
	}
	UpdateData(FALSE);
}


void InputInject::OnBnClickedInject()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	char ImeDllPath[MAX_PATH] = "E:\\InjectDll\\MyImeDll.dll";
	char InjectDllPath[MAX_PATH] = "E:\\InjectDll\\MyDll.dll";
	UpdateData(TRUE);
	//ԭʼ���뷨
	HKL HklOldInput = NULL;
	//�µ����뷨
	HKL HklNewInput = NULL;
	//���뷨��������
	TCHAR ImeSymbol[MAX_PATH];
	//��ע����̵Ĵ��ھ��
	HWND hWnd = NULL;
	HWND hTopWnd = NULL;

	HKEY phkResult;
	int i = 0;
	TCHAR ValueName[MAX_PATH];  //�洢�õ��ļ�ֵ��
	TCHAR lpData[MAX_PATH];     //�洢�õ��ļ�ֵ����
	DWORD lenValue = MAX_PATH;  //�洢��ֵ�����ݳ���
	DWORD lenData = MAX_PATH;   //�洢��ֵ�����ݳ���

	//1.����ԭʼ�ļ��̲��֣�������滹ԭ
	SystemParametersInfo(SPI_GETDEFAULTINPUTLANG, 0, &HklOldInput, 0);
	//2.�����뷨DLL�ʹ�ע��Dllһ��ŵ�C:\\WINDOWS\\SYSTEM32Ŀ¼��
	CopyFile(ImeDllPath, "C:\\WINDOWS\\SYSTEM32\\MyImeDll.ime", FALSE);
	CopyFile(m_strDllPath.GetBuffer(0), "C:\\WINDOWS\\SYSTEM32\\InjectDll.dll", FALSE);

	//3.�������뷨
	HklNewInput = ImmInstallIME("C:\\WINDOWS\\SYSTEM32\\MyImeDll.ime", "�ҵ����뷨");

	//4.�ж����뷨����Ƿ���Ч
	if (!ImmIsIME(HklNewInput))
	{
		MessageBox("���뷨����ʧ��!!!");
	}

	//5.���������뷨�ļ��̲���
	ActivateKeyboardLayout(HklNewInput, 0);
	//6.��ȡ����������뷨ע�����������ɾ��
	GetKeyboardLayoutName(ImeSymbol);

	//7.��ȡ���ھ��
	hWnd = GetHwndByProcessId(m_dwPID);

	//8.����ָ�����ڵ����뷨
	if (HklNewInput != NULL)
	{
		::PostMessage(hWnd, WM_INPUTLANGCHANGEREQUEST, 0x1, (LPARAM)HklNewInput);
	}

	Sleep(1000);

	//9.��ԭ���̲���
	SystemParametersInfo(SPI_SETDEFAULTINPUTLANG, 0, &HklOldInput, SPIF_SENDWININICHANGE);

	//10.���㴰�ڻ���ȥ��������ȥ����ж�ز�������Ϊime�ļ�ռ��
	do
	{
		hTopWnd = ::FindWindowEx(NULL, hTopWnd, NULL, NULL);
		if (hTopWnd != NULL)
		{
			::PostMessage(hTopWnd, WM_INPUTLANGCHANGEREQUEST, 0x1, (LPARAM)&HklOldInput);
		}
	} while (hTopWnd == NULL);

	//11.ж�����뷨
	if (HklNewInput != NULL)
	{
		UnloadKeyboardLayout(HklNewInput);
	}

	//12.ɾ��ע��������ۼ�
	//��ע�����Ŀ����ȡ���
	RegOpenKey(HKEY_CURRENT_USER, "Keyboard Layout\\Preload", &phkResult);
	//ö��ע�����Ŀ
	while (RegEnumValue(phkResult, i, ValueName, &lenValue, NULL, NULL, (LPBYTE)lpData, &lenData) != ERROR_NO_MORE_ITEMS)
	{
		if (lenData != 0)
		{
			if (strcmp(ImeSymbol, lpData) == 0)
			{
				//ɾ����Ŀ��ֵ
				RegDeleteValue(phkResult, ValueName);
				break;
			}
		}

		memset(lpData, 0, MAX_PATH);
		memset(ValueName, 0, MAX_PATH);
		lenValue = MAX_PATH;
		lenData = MAX_PATH;
		i++;
	}

	// ɾ�����뷨�ļ�
	DeleteFile("C:\\WINDOWS\\SYSTEM32\\MyImeDll.ime");
}
