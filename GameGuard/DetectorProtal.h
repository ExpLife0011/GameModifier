//����̵߳���ڣ������߼��������������
#pragma  once
#include <thread>
#include <windows.h>
class DetectorPortal
{
public:
	DetectorPortal();
	~DetectorPortal();
	bool Init();
	bool DeInit();
	//����߳��Ƿ���
	bool IsLive(DWORD iTime);
private:
	void DetectProcess();

	DWORD  _detectorThreadId;
	HANDLE  _detectorThreadHandle;
	DWORD   iCurrTime;
	std::thread thread;
};