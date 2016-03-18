#include <windows.h>

int APIENTRY wWinMain(HINSTANCE, HINSTANCE, LPWSTR lpCmdLine, int)
{
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi = { 0 };

	int answer;
	answer = MessageBox(NULL, lpCmdLine, L"fakeSuRun", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) ;
	if (answer != IDYES)
		return 255;

	if (!CreateProcess(NULL, lpCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		return 255;
	return 0;
}
