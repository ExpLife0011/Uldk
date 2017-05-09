#pragma once

#include <EdkClass/Common.h>

typedef UINT8		BYTE;
typedef UINT8*		PBYTE;

typedef UINT16		WORD;
typedef UINT32		DWORD;
typedef UINT64		QWORD;

#define EXIT_WITH_STATUS(_status_) {Status=_status_;goto EXIT;}