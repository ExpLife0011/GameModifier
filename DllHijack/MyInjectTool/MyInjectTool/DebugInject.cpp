// DebugInject.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyInjectTool.h"
#include "DebugInject.h"
#include "afxdialogex.h"

//�ṹ�����ֽڶ���!  
#pragma pack(1)  
typedef struct _INJECT_CODE
{
	BYTE  byMOV_EAX;          //mov eax, addr szDllpath
	DWORD dwMOV_EAX_VALUE;
	BYTE  byPUSH_EAX;         //push eax
	BYTE  byMOV_ECX;          //mov ecx, LoadLibrary
	DWORD dwMOV_ECX_VALUE;
	WORD  wCALL_ECX;          //call ecx
	BYTE  byINT3;             //int 3
	CHAR  szDllPath[MAX_PATH];
}INJECT_CODE, *PINJECT_CODE;
#pragma pack()
// DebugInject �Ի���

IMPLEMENT_DYNAMIC(DebugInject, CDialogEx)

DebugInject::DebugInject(CWnd* pParent /*=NULL*/)
	: CDialogEx(DebugInject::IDD, pParent)
	, m_strExePath(_T(""))
	, m_strDllPath(_T(""))
{

}

DebugInject::~DebugInject()
{
}

void DebugInject::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strExePath);
	DDX_Text(pDX, IDC_EDIT2, m_strDllPath);
}


BEGIN_MESSAGE_MAP(DebugInject, CDialogEx)
	ON_BN_CLICKED(IDC_INJECT, &DebugInject::OnBnClickedInject)
	ON_BN_CLICKED(IDC_BUTTON4, &DebugInject::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON3, &DebugInject::OnBnClickedButton3)
END_MESSAGE_MAP()


// DebugInject ��Ϣ�������


void DebugInject::OnBnClickedInject()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BOOL bRet;
	DWORD dwProcessId = 0;
	LPVOID lpBaseAddress = NULL;
	HANDLE hThread = NULL;
	HANDLE hProcess = NULL;
	DEBUG_EVENT dbgEvent = { 0 };
	CONTEXT ctxOld = { CONTEXT_FULL };
	CONTEXT ctxNew = { CONTEXT_FULL };
	INJECT_CODE ic = { 0 };
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi = { 0 };
	HMODULE hDll = NULL;
	BOOL bIsSystemBp = TRUE;
	DWORD dwOldEip = 0;
	bRet = CreateProcess(NULL,
		m_strExePath.GetBuffer(0),
		NULL,
		NULL,
		FALSE,
		DEBUG_ONLY_THIS_PROCESS,
		NULL,
		NULL,
		&si,
		&pi);
	if (!bRet)
	{
		MessageBox("CreateProcess ʧ��");
		return;
	}

	//��ֹ�����Խ��̺͵�����һ��ر�
	bRet = DebugSetProcessKillOnExit(FALSE);

	while (WaitForDebugEvent(&dbgEvent, INFINITE))
	{
		switch (dbgEvent.dwDebugEventCode)
		{
		case CREATE_PROCESS_DEBUG_EVENT:
			hProcess = dbgEvent.u.CreateProcessInfo.hProcess;
			hThread = dbgEvent.u.CreateProcessInfo.hThread;

			//�����ڴ�,���ע��ָ��  
			lpBaseAddress = VirtualAllocEx(hProcess,
				NULL,
				sizeof(INJECT_CODE),
				MEM_COMMIT | MEM_RESERVE,
				PAGE_EXECUTE_READWRITE);
			if (NULL == lpBaseAddress)
			{
				MessageBox("VirtualAllocEx ʧ��");
				return;
			}

			//��ShellCode�ṹ�帳ֵ
			ic.byMOV_EAX = 0xB8;
			ic.dwMOV_EAX_VALUE = (DWORD)lpBaseAddress + offsetof(INJECT_CODE, szDllPath);
			ic.byPUSH_EAX = 0x50;
			ic.byMOV_ECX = 0xB9;
			ic.dwMOV_ECX_VALUE = (DWORD)&LoadLibrary;
			ic.wCALL_ECX = 0xD1FF;
			ic.byINT3 = 0xCC;
			memcpy(ic.szDllPath, m_strDllPath.GetBuffer(0), m_strDllPath.GetLength());

			//д��ShellCode
			bRet = WriteProcessMemory(hProcess, lpBaseAddress, &ic, sizeof(ic), NULL);
			if (!bRet)
			{
				MessageBox("WriteProcessMemory ʧ��");
				return;
			}


			//��ȡ��ǰ�߳�������
			bRet = GetThreadContext(hThread, &ctxOld);
			if (!bRet)
			{
				MessageBox("GetThreadContext ʧ��");
				return;
			}

			ctxNew = ctxOld;


#ifdef _WIN64
			ctxNew.Rip = (DWORD)lpBaseAddress;
			dwOldEip = ctxNew.Rip;
#else 
			ctxNew.Eip = (DWORD)lpBaseAddress;
			dwOldEip = ctxNew.Eip;
#endif

			bRet = SetThreadContext(hThread, &ctxNew);
			if (!bRet)
			{
				MessageBox("SetThreadContext ʧ��");
				return;
			}

			break;
		case EXCEPTION_DEBUG_EVENT:
			if (dbgEvent.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_BREAKPOINT)
			{
				//���ε�ϵͳ�ϵ�
				if (bIsSystemBp)
				{
					bIsSystemBp = FALSE;
					break;
				}

				//�ͷ��ڴ�
				bRet = VirtualFreeEx(hProcess,
					lpBaseAddress,
					0,
					MEM_RELEASE
					);

				if (!bRet)
				{
					MessageBox("VirtualFreeEx ʧ��");
					return;
				}

				//�ָ������򴴽�ʱ��EIP
				bRet = SetThreadContext(hThread, &ctxOld);
				if (!bRet)
				{
					MessageBox("SetThreadContext ʧ��");
					return;
				}

				bRet = ContinueDebugEvent(dbgEvent.dwProcessId, dbgEvent.dwThreadId, DBG_CONTINUE);
				if (!bRet)
				{
					MessageBox("ContinueDebugEvent ʧ��!!");
					return;
				}
				//�˳������̣��ñ����Գ���������
				//ExitProcess(0);
				return;

			}
			break;
		}

		bRet = ContinueDebugEvent(dbgEvent.dwProcessId, dbgEvent.dwThreadId, DBG_EXCEPTION_NOT_HANDLED);
		if (!bRet)
		{
			MessageBox("ContinueDebugEvent ʧ��!!");
			return;
		}
	}
}


void DebugInject::OnBnClickedButton4()
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


void DebugInject::OnBnClickedButton3()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	char szFilter[] = "��ִ�г���|*.exe";
	CFileDialog fileDlg(TRUE, "exe", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	UpdateData(TRUE);
	if (fileDlg.DoModal() == IDOK)
	{
		m_strExePath = fileDlg.GetPathName();
	}
	UpdateData(FALSE);
}
