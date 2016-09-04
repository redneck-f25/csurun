// cmd /e:on /v:on /c "for /l %n in (0,0,1) do @(echo !time! & >nul timeout /t 1)"
// cmd /e:on /v:on /c "for /l %n in (0,0,1) do @(echo !time! & >nul ping -n 2 localhost)"

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <Psapi.h>
#include <strsafe.h>

#include "../cSuRun/strdefs.h"

#define NUM_INSTANCES (4)
HANDLE pipes[NUM_INSTANCES][2] = {
	{ GetStdHandle(STD_INPUT_HANDLE), NULL },
	{ NULL, GetStdHandle(STD_OUTPUT_HANDLE) },
	{ NULL, GetStdHandle(STD_ERROR_HANDLE) }
};

DWORD WINAPI RedirStdBytesThread(int i)
{
	char ch;
	DWORD cch;

	while (1)
	{
		if (!ReadFile(pipes[i][0], &ch, sizeof(ch), &cch, NULL))
			break;
		if (!cch)
			break;
		if (!WriteFile(pipes[i][1], &ch, sizeof(ch), &cch, NULL))
			break;
		if (!cch)
			break;
	}
	return 0;
}

wchar_t* strip_part(wchar_t **pcmdline)
{
	wchar_t *start = *pcmdline;
	wchar_t *end;

	if (*start == '"')
	{
		if (!(end = wcschr(start + 1, '"')))
			return 0;
		end++;
	}
	else
	{
		if (!(end = wcschr(start, ' ')))
			end = wcschr(start, 0);
	}
	if (*end != 0)
	{
		if (*end != ' ')
			return 0;
		*end = 0;
		do end++; while (*end != 0 && *end == ' ');
	}
	*pcmdline = end;
	return start;
}

int main()
{
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi = { 0 };
	wchar_t *pch;
	wchar_t *commandLine = GetCommandLine();
	wchar_t *application;
	wchar_t *helper_bin = (LPWSTR)malloc(4096);
	wchar_t *surun_cmdline = (LPWSTR)malloc(4096);
	wchar_t *application_full = (LPWSTR)malloc(4096);
	wchar_t pipename[PIPENAME_LEN];
	DWORD exitCode;
	int nh = 0;
	DWORD res;

	GetModuleFileName(NULL, helper_bin, 2047);
	pch = wcsrchr(helper_bin, L'.');
	wcscpy(pch, L"H.bin");

	if (!(strip_part(&commandLine) && (application = strip_part(&commandLine))))
	{
		return 1;
	}
	if (*application == 0 || !wcscmp(application, L"\"\""))
	{
		return 1;
	}
	res = SearchPath(NULL, application, L".exe", 2048, application_full, NULL);
	if (res == 0 || res >= 2048) {
		return 1;
	}

	StringCchPrintf(surun_cmdline, 2048, CMDLINE_INVOKE_FMT,
			helper_bin, GetCurrentProcessId(), application_full, commandLine);
	free(helper_bin);
	free(application_full);
	//wprintf(L"%s\n", surun_cmdline);
	if (!CreateProcess(NULL, surun_cmdline, NULL, NULL,
			FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi))
	{
		return 1;
	}

	free(surun_cmdline);

	StringCchPrintf(pipename, PIPENAME_LEN, PIPENAME_FMT, GetCurrentProcessId());
	//wprintf(L"%s\n", pipe_name);
	pipes[0][1] = CreateNamedPipe(pipename, PIPE_ACCESS_DUPLEX, 0, NUM_INSTANCES, 1, 1, 0, NULL);
	pipes[1][0] = CreateNamedPipe(pipename, PIPE_ACCESS_DUPLEX, 0, NUM_INSTANCES, 1, 1, 0, NULL);
	pipes[2][0] = CreateNamedPipe(pipename, PIPE_ACCESS_DUPLEX, 0, NUM_INSTANCES, 1, 1, 0, NULL);
	pipes[3][0] = CreateNamedPipe(pipename, PIPE_ACCESS_DUPLEX, 0, NUM_INSTANCES, 1, 1, 0, NULL);
	//DWORD pid;
	//HANDLE hprocess;

	ResumeThread(pi.hThread);
	WaitForSingleObject(pi.hProcess, INFINITE);

	GetExitCodeProcess(pi.hProcess, &exitCode);
	if (exitCode)
		return exitCode;

	ConnectNamedPipe(pipes[0][1], NULL);
	ConnectNamedPipe(pipes[1][0], NULL);
	ConnectNamedPipe(pipes[2][0], NULL);
	ConnectNamedPipe(pipes[3][0], NULL);

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RedirStdBytesThread, (LPVOID)0, 0, NULL);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RedirStdBytesThread, (LPVOID)1, 0, NULL);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RedirStdBytesThread, (LPVOID)2, 0, NULL);

	ReadFile(pipes[3][0], &exitCode, sizeof(exitCode), NULL, NULL);
	return exitCode;
}
