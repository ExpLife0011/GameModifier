// BlookHook.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyInjectTool.h"
#include "BlookHook.h"
#include "afxdialogex.h"


// BlookHook �Ի���

IMPLEMENT_DYNAMIC(BlookHook, CDialogEx)

BlookHook::BlookHook(CWnd* pParent /*=NULL*/)
	: CDialogEx(BlookHook::IDD, pParent)
	, m_dwPID(0)
{

}

BlookHook::~BlookHook()
{
}

void BlookHook::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_dwPID);
}


BEGIN_MESSAGE_MAP(BlookHook, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &BlookHook::OnBnClickedButton1)
END_MESSAGE_MAP()


// BlookHook ��Ϣ�������
typedef void(*LPFUN)();
typedef void(*LPFUN2)(DWORD dwPid);
HINSTANCE g_hBlookDll;
LPFUN2 g_pfnBlookSetHook = NULL;
LPFUN g_pfnBlookUnHook = NULL;

void BlookHook::OnBnClickedButton1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	g_hBlookDll = LoadLibrary("BlockHook.dll");

	if (g_hBlookDll != NULL)
	{
		g_pfnBlookSetHook = (LPFUN2)GetProcAddress(g_hBlookDll, "SetHook");
		g_pfnBlookUnHook = (LPFUN)GetProcAddress(g_hBlookDll, "UnHook");
	}
	else
	{
		MessageBox("����DLLʧ�ܣ�");
		return;
	}


	//��װ���Ӻ���
	if (g_pfnBlookSetHook != NULL)
	{
		g_pfnBlookSetHook(m_dwPID);
	}
	else
	{
		MessageBox("��װ����ʧ�ܣ�");
		return;
	}

}
