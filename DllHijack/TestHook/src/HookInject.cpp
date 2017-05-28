// HookInject.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyInjectTool.h"
#include "HookInject.h"
#include "afxdialogex.h"


// HookInject �Ի���

IMPLEMENT_DYNAMIC(HookInject, CDialogEx)

HookInject::HookInject(CWnd* pParent /*=NULL*/)
	: CDialogEx(HookInject::IDD, pParent)
	, m_dwPid(0)
	, m_strDllPath(_T(""))
{

}

HookInject::~HookInject()
{
}

void HookInject::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_dwPid);
	DDX_Text(pDX, IDC_EDIT4, m_strDllPath);
}


BEGIN_MESSAGE_MAP(HookInject, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &HookInject::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_INJECT, &HookInject::OnBnClickedInject)
	ON_BN_CLICKED(IDC_UNHOOK, &HookInject::OnBnClickedUnhook)
END_MESSAGE_MAP()


// HookInject ��Ϣ�������


void HookInject::OnBnClickedButton2()
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



typedef void(*LPFUN)();
typedef void(*LPFUN2)(DWORD dwPid, char * szDllPath);
HINSTANCE g_hDll;
LPFUN2 g_pfnSetHook = NULL;
LPFUN g_pfnUnHook = NULL;

void HookInject::OnBnClickedInject()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	g_hDll = LoadLibrary("InjectDll.dll");

	if (g_hDll != NULL)
	{
		g_pfnSetHook = (LPFUN2)GetProcAddress(g_hDll, "SetHook");
		g_pfnUnHook = (LPFUN)GetProcAddress(g_hDll, "UnHook");
	}
	else
	{
		MessageBox("����DLLʧ�ܣ�");
		return;
	}


	//��װ���Ӻ���
	if (g_pfnSetHook != NULL)
	{
		g_pfnSetHook(m_dwPid, m_strDllPath.GetBuffer(0));
	}
	else
	{
		MessageBox("��װ����ʧ�ܣ�");
		return;
	}
}


void HookInject::OnBnClickedUnhook()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (g_hDll != NULL)
	{
		//ж�ع��Ӻ���
		g_pfnUnHook();
		//Ĩ��DLL
		FreeLibrary(g_hDll);
	}
}
