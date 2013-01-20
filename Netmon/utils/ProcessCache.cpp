#include "stdafx.h"
#include "ProcessCache.h"

ProcessCache::ProcessCache()
{
	RtlZeroMemory(_nameTable, sizeof(_nameTable));
	RtlZeroMemory(_pathTable, sizeof(_pathTable));
}

TCHAR *ProcessCache::GetName(int pid)
{
	if (_nameTable[pid] == NULL)
	{
		rebuildTable();
	}
	return _nameTable[pid];
}

TCHAR *ProcessCache::GetFullPath(int pid)
{
	if (_pathTable[pid] == NULL)
	{
		rebuildTable();
	}
	return _pathTable[pid];
}

void ProcessCache::rebuildTable()
{
	// Clear Tables
	RtlZeroMemory(_nameTable, sizeof(_nameTable));
	RtlZeroMemory(_pathTable, sizeof(_pathTable));

	// Take a snapshot
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 p;
	p.dwSize = sizeof(PROCESSENTRY32);

	// Traverse Process List
	for(BOOL ret = Process32First(hSnapShot, &p); ret != 0; ret = Process32Next(hSnapShot, &p))
	{
		// Get pid and file name
		int pid = p.th32ProcessID;
		TCHAR *processName = p.szExeFile;

		// Get full path (if possible)
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, pid);
		if (hProcess == 0)
		{
			_tcscpy_s(_nameTable[pid], MAX_PATH, processName);
			_tcscpy_s(_pathTable[pid], MAX_PATH, TEXT("-"));
		}
		else
		{
			TCHAR fullPath[260];
			GetModuleFileNameEx(hProcess, 0, fullPath, 260);
			_tcscpy_s(_nameTable[pid], MAX_PATH, processName);
			_tcscpy_s(_pathTable[pid], MAX_PATH, fullPath);
		}
		CloseHandle(hProcess);
	}
	CloseHandle(hSnapShot);
}
