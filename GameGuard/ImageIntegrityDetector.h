//�ڴ�ӳ���ļ�У��
#pragma once
#include <windows.h>
class ImageIntegrityDetector
{
public:
	ImageIntegrityDetector();
	~ImageIntegrityDetector();
	void Detect();




private:




	void CheckImageIsModified();
	void CalcImageSize(char* pDllname);

	bool AddCodeSectionCrc2File();
	bool CheckCodeSnipeCrc32();
	bool ImageCodeSectionCrc32(char* pModuleName, DWORD &crc32);
	bool PEFileCodeSectionCrc32(char* pFile , DWORD& crc32);
	bool PEFileCodeSectionCrc32_2(char* pFile , DWORD& crc32);
	BYTE * _pBuff;

};

