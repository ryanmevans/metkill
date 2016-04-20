#include "stdafx.h"
#include "windows.h"
#include "stdio.h"
#include "stdlib.h"
#include "psapi.h"

int main()
{
	MEMORY_BASIC_INFORMATION mbi;
	int mypid, metflag = 0, killflag = 0;
	HANDLE myProcess;
	unsigned char* p = NULL;
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
		return 1;
	}
	cProcesses = cbNeeded / sizeof(DWORD);
	for (i = 0; i < cProcesses; i++) {
		if (aProcesses[i] != 0) {
			mypid = aProcesses[i];
			metflag = 0;
			myProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_QUERY_INFORMATION, false, mypid);
			for (p = NULL; VirtualQueryEx(myProcess, p, &mbi, sizeof(mbi)) == sizeof(mbi); p += mbi.RegionSize) {
				if (mbi.Protect == PAGE_EXECUTE_READWRITE) {
					if (mbi.RegionSize > 880000 && mbi.RegionSize < 920000) {
						metflag++;
					}
				}
			}
			if (metflag == 2) {
				printf("Process with PID=%d has meterpreter attached, killing process now\n", mypid);
				TerminateProcess(myProcess, 0);
				killflag = 1;
			}
		}
	}

	if (killflag == 0) {
		printf("No meterpreter sessions found\n");
	}
	scanf_s("%d", &i);
	return 0;
}
