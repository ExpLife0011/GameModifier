#pragma once
#include "stdafx.h"
#include <vector>
// ImportInject �Ի���
#include "PEFuncs.h"
class ImportInject 
{
public:
	std::wstring m_strFile;
	std::string m_strDll;
	std::wstring m_strFun;
	std::wstring m_strTempPath;

	std::vector<PIMAGE_SECTION_HEADER> vct_SectionHeader;//����ڱ���Ϣ
	MAP_FILE_STRUCT m_stMapFile;
	PIMAGE_DOS_HEADER m_pDosHeader;
	PIMAGE_NT_HEADERS m_pNtHeader;


	void OnBnClickedButton1(std::wstring peFile);
	void OnBnClickedButton2();
	std::vector<std::string> m_strFunList;
	void OnBnClickedButton3();
};
