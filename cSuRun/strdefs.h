#ifndef __STRDEFS_H__
#define __STRDEFS_H__

#define PIPENAME(pid)  (L"\\\\.\\pipe\\cSuRun-" pid)
#define PIPENAME_FMT   PIPENAME("%d")
#define PIPENAME_MAX   PIPENAME("4294967296")
#define PIPENAME_LEN   (sizeof(PIPENAME_MAX) / sizeof(PIPENAME_MAX[0]))

#if defined(_DEBUG) && !defined(DEBUG_NO_FAKE_SURUN)
#define SURUN_CMD "fakesurun"
#else
#define SURUN_CMD "surun"
#endif

#define CMDLINE(pre, post)  (L"" pre "/attach:%08x " post)
#define CMDLINE_INVOKE_FMT  CMDLINE(SURUN_CMD " \"%s\" ", "\"%s\" %s")
#define CMDLINE_PARSE_FMT   CMDLINE("", "%n")

#endif //__STRDEFS_H__
