
// MyInjectTool.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once
#include "PEFuncs.h"
#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif
#include <vector>
#include "resource.h"		// ������
using namespace std;

// CMyInjectToolApp: 
// �йش����ʵ�֣������ MyInjectTool.cpp
//

class CMyInjectToolApp : public CWinApp
{
public:
	CMyInjectToolApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
public:
	vector<PIMAGE_SECTION_HEADER> vct_SectionHeader;//����ڱ���Ϣ
	CFile* m_pFile;
	MAP_FILE_STRUCT m_stMapFile;
	PIMAGE_DOS_HEADER m_pDosHeader;
	PIMAGE_NT_HEADERS m_pNtHeader;
};

extern CMyInjectToolApp theApp;