#pragma once


// ProcessInject �Ի���

class ProcessInject : public CDialogEx
{
	DECLARE_DYNAMIC(ProcessInject)

public:
	ProcessInject(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ProcessInject();

// �Ի�������
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_strExePath;
	CString m_strDllPath;
	afx_msg void OnBnClickedInject();
	afx_msg void OnBnClickedFreemem();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
};
