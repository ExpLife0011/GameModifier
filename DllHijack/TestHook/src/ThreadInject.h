#pragma once


// ThreadInject �Ի���

class ThreadInject : public CDialogEx
{
	DECLARE_DYNAMIC(ThreadInject)

public:
	ThreadInject(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ThreadInject();

// �Ի�������
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	// ����ID
	DWORD m_dwPid;
	CString m_strDllPath;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
};
