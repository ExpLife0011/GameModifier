#pragma once


// APCInject �Ի���

class APCInject : public CDialogEx
{
	DECLARE_DYNAMIC(APCInject)

public:
	APCInject(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~APCInject();

// �Ի�������
	enum { IDD = IDD_DIALOG5 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_strExePath;
	CString m_strDllPath;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedInject();
};
