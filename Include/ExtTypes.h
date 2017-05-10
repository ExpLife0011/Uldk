#pragma once

#include <EdkClass/Common.h>

typedef INT8		CHAR;
typedef UINT8		BYTE;
typedef UINT8*		PBYTE;

typedef UINT16		WORD;
typedef UINT32		DWORD;
typedef INT32		LONG;
typedef UINT64		QWORD;
typedef UINT64		ULONGLONG;

#define EXIT_WITH_STATUS(_status_) {Status=_status_;goto EXIT;}