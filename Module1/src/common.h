#pragma once

#define MAX_NUM_ARGVS 50

extern size_t com_argc;
extern char* com_argv[MAX_NUM_ARGVS + 1];

void COM_ParseCmdLine(char* lpCmdLine);
size_t COM_CheckParm(const char* parm);
