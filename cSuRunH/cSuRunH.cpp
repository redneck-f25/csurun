#include <windows.h>
#include <strsafe.h>

#include "../cSuRun/strdefs.h"

int APIENTRY wWinMain(HINSTANCE, HINSTANCE, LPWSTR lpCmdLine, int)
{
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES) };
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi = { 0 };
	int cmdOffset;
	wchar_t pipename[PIPENAME_LEN];
	DWORD csurun_pid;
	DWORD exitcode;

	if (swscanf_s(lpCmdLine, CMDLINE_PARSE_FMT, &csurun_pid, &cmdOffset) != 1 || !AttachConsole(csurun_pid))
		return 1;

	sa.bInheritHandle = TRUE;

	StringCchPrintf(pipename, PIPENAME_LEN, PIPENAME_FMT, csurun_pid);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = CreateFile(pipename, GENERIC_READ, 0, &sa,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	si.hStdOutput = CreateFile(pipename, GENERIC_WRITE, 0, &sa,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	si.hStdError = CreateFile(pipename, GENERIC_WRITE, 0, &sa,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	HANDLE hcomm = CreateFile(pipename, GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	CreateProcess(NULL, lpCmdLine + cmdOffset, NULL, NULL,
			TRUE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);

	ResumeThread(pi.hThread);
	WaitForSingleObject(pi.hProcess, INFINITE);
	GetExitCodeProcess(pi.hProcess, &exitcode);
	WriteFile(hcomm, &exitcode, sizeof(exitcode), NULL, NULL);

	return 0;
}
