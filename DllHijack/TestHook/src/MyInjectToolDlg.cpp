
// MyInjectToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyInjectTool.h"
#include "MyInjectToolDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMyInjectToolDlg �Ի���



CMyInjectToolDlg::CMyInjectToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyInjectToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyInjectToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_listProcess);
	DDX_Control(pDX, IDC_LIST1, m_listDll);
}

BEGIN_MESSAGE_MAP(CMyInjectToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_SHOW, &CMyInjectToolDlg::OnShowProcess)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, &CMyInjectToolDlg::OnItemchangedList1)
	ON_COMMAND(ID_IMPORT, &CMyInjectToolDlg::OnImport)
	ON_COMMAND(ID_THREAD, &CMyInjectToolDlg::OnThread)
	ON_COMMAND(ID_PROCESS, &CMyInjectToolDlg::OnProcess)
	ON_COMMAND(ID_DEBUG, &CMyInjectToolDlg::OnDebug)
	ON_COMMAND(ID_APC, &CMyInjectToolDlg::OnApc)
	ON_COMMAND(ID_REG, &CMyInjectToolDlg::OnReg)
	ON_COMMAND(ID_HOOK, &CMyInjectToolDlg::OnHook)
	ON_COMMAND(ID_REMOTE, &CMyInjectToolDlg::OnRemote)
	ON_COMMAND(ID_INPUT, &CMyInjectToolDlg::OnInput)
	ON_COMMAND(ID_HIJACK, &CMyInjectToolDlg::OnHijack)
	ON_COMMAND(ID_ANTIHOOK, &CMyInjectToolDlg::OnAntihook)
END_MESSAGE_MAP()


// CMyInjectToolDlg ��Ϣ�������

BOOL CMyInjectToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	m_listProcess.InsertColumn(0,"��������",LVCFMT_LEFT,150,-1);
    m_listProcess.InsertColumn(1,"����ID",LVCFMT_LEFT,150,-1);
    m_listProcess.InsertColumn(2,"ӳ��·��",LVCFMT_LEFT,500,-1);
    
    m_listDll.InsertColumn(0,"ģ������",LVCFMT_LEFT,150,-1);
    m_listDll.InsertColumn(1,"ģ��·��",LVCFMT_LEFT,500,-1);
    ZeroMemory(m_dwID,sizeof(DWORD) * 1000);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

/*******************************************************
*��������:��Ȩ����
*��������:��
*��������:bool
*ע������:��
*����޸�ʱ��:2017/5/13
*******************************************************/
bool CMyInjectToolDlg::EnableDebugPriv() //��Ȩ����
{
	HANDLE hToken;

	LUID sedebugnameValue;

	TOKEN_PRIVILEGES tkp;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		return false;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	{
		CloseHandle(hToken);

		return false;
	}

	tkp.PrivilegeCount = 1;

	tkp.Privileges[0].Luid = sedebugnameValue;

	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
		CloseHandle(hToken);

		return false;
	}
	return true;
}

/*******************************************************
*��������:�������еĽ��̲�����ʾ
*��������:��
*��������:��
*ע������:��
*����޸�ʱ��:2017/5/13
*******************************************************/
void CMyInjectToolDlg::OnShowprocess()
{
	ZeroMemory(m_dwID, sizeof(DWORD)* 1000);
	m_listProcess.DeleteAllItems();
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		MessageBox("��ȡϵͳ���̿���ʧ�ܣ�����");
		return;
	}

	BOOL bResult = Process32First(hProcessSnap, &pe32);
	int i = 0;
	while (bResult)
	{

		CString myStr = pe32.szExeFile;
		CString myStrId;

		int nId = pe32.th32ProcessID;
		myStrId.Format("%d", nId);
		m_listProcess.InsertItem(i, myStr);
		m_listProcess.SetItemText(i, 1, myStrId);
		m_listProcess.SetItemText(i, 2, pe32.szExeFile);
		m_dwID[i] = pe32.th32ProcessID;
		i++;
		bResult = Process32Next(hProcessSnap, &pe32);
	}

	CloseHandle(hProcessSnap);
}

/*******************************************************
*��������:���ս���ID��ʾ����DLLģ��
*��������:����1����ǰ�б�����
*��������:��
*ע������:��
*����޸�ʱ��:2017/5/13
*******************************************************/
void CMyInjectToolDlg::OnShowmoudle(int nIndex)
{
	DWORD dwId = 0;

	EnableDebugPriv();
	dwId = m_dwID[nIndex];

	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	m_listDll.DeleteAllItems();

	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwId);
	int n = GetLastError();
	if (hModuleSnap == INVALID_HANDLE_VALUE){
		printf("CreateToolhelp32SnapshotError! \n");
		return;
	}

	MODULEENTRY32 module32;
	module32.dwSize = sizeof(module32);
	BOOL bResult = Module32First(hModuleSnap, &module32);

	int i = 0;
	while (bResult){
		CString strDllName = module32.szModule;
		CString strDllPath = module32.szExePath;

		m_listDll.InsertItem(i, strDllName);
		m_listDll.SetItemText(i, 1, strDllPath);
		bResult = Module32Next(hModuleSnap, &module32);
		i++;
	}

	CloseHandle(hModuleSnap);
}


void CMyInjectToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMyInjectToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMyInjectToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMyInjectToolDlg::OnShowProcess()
{
	// TODO:  �ڴ���������������
	m_listProcess.DeleteAllItems();
	OnShowprocess();
}


void CMyInjectToolDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO:  �ڴ˴������Ϣ����������
	CMenu popMenu;

	popMenu.LoadMenu(IDR_MENU1);         //����˵�

	CMenu *pPopup;

	pPopup = popMenu.GetSubMenu(0);     //����Ӳ˵�ָ��



	pPopup->EnableMenuItem(ID_SHOW, MF_BYCOMMAND | MF_ENABLED);  //����˵���ʹ��





	//ClientToScreen(&point);            //���ͻ�������ת������Ļ����

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);                //��ʾ�����˵�����������Ϊ(����ڲ˵����|�����Ҽ���x��y��this)

	pPopup->Detach();

	popMenu.DestroyMenu();
}


void CMyInjectToolDlg::OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->uNewState)
	{
		OnShowmoudle(pNMLV->iItem);
	}

	*pResult = 0;
}


void CMyInjectToolDlg::OnImport()
{
	// TODO:  �ڴ���������������
	m_pImportDlg = new ImportInject;
	m_pImportDlg->Create(IDD_DIALOG1);
	m_pImportDlg->ShowWindow(SW_SHOW);
}


void CMyInjectToolDlg::OnThread()
{
	// TODO:  �ڴ���������������
	m_pThreadDlg = new ThreadInject;
	m_pThreadDlg->Create(IDD_DIALOG2);
	m_pThreadDlg->ShowWindow(SW_SHOW);
}


void CMyInjectToolDlg::OnProcess()
{
	// TODO:  �ڴ���������������
	m_pProcessDlg = new ProcessInject;
	m_pProcessDlg->Create(IDD_DIALOG3);
	m_pProcessDlg->ShowWindow(SW_SHOW);
}


void CMyInjectToolDlg::OnDebug()
{
	// TODO:  �ڴ���������������
	m_pDebugDlg = new DebugInject;
	m_pDebugDlg->Create(IDD_DIALOG4);
	m_pDebugDlg->ShowWindow(SW_SHOW);
}


void CMyInjectToolDlg::OnApc()
{
	// TODO:  �ڴ���������������
	m_pApcDlg = new APCInject;
	m_pApcDlg->Create(IDD_DIALOG5);
	m_pApcDlg->ShowWindow(SW_SHOW);
}


void CMyInjectToolDlg::OnReg()
{
	// TODO:  �ڴ���������������
	m_RegDlg = new RegInject;
	m_RegDlg->Create(IDD_DIALOG6);
	m_RegDlg->ShowWindow(SW_SHOW);
}


void CMyInjectToolDlg::OnHook()
{
	// TODO:  �ڴ���������������
	m_HookDlg = new HookInject;
	m_HookDlg->Create(IDD_DIALOG7);
	m_HookDlg->ShowWindow(SW_SHOW);
}


void CMyInjectToolDlg::OnRemote()
{
	// TODO:  �ڴ���������������
	m_RemoteInjectDlg = new RemoteInject;
	m_RemoteInjectDlg->Create(IDD_DIALOG10);
	m_RemoteInjectDlg->ShowWindow(SW_SHOW);
}


void CMyInjectToolDlg::OnInput()
{
	// TODO:  �ڴ���������������
	m_InputInjectDlg = new InputInject;
	m_InputInjectDlg->Create(IDD_DIALOG9);
	m_InputInjectDlg->ShowWindow(SW_SHOW);
}


void CMyInjectToolDlg::OnHijack()
{
	// TODO:  �ڴ���������������
	m_HijackDlg = new Hijack;
	m_HijackDlg->Create(IDD_DIALOG8);
	m_HijackDlg->ShowWindow(SW_SHOW);
}


void CMyInjectToolDlg::OnAntihook()
{
	// TODO:  �ڴ���������������
	m_BlookHookDlg = new BlookHook;
	m_BlookHookDlg->Create(IDD_DIALOG11);
	m_BlookHookDlg->ShowWindow(SW_SHOW);
}
