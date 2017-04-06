#pragma once

#include <windows.h>

#pragma once

/* align help function */

#define ALIGN_DOWN_BY(length, alignment) \
    ((ULONG_PTR)(length) & ~(alignment - 1))

#define ALIGN_UP_BY(length, alignment) \
    (ALIGN_DOWN_BY(((ULONG_PTR)(length) + alignment - 1), alignment))

#define ALIGN_DOWN_POINTER_BY(address, alignment) \
    ((PVOID)((ULONG_PTR)(address) & ~((ULONG_PTR)alignment - 1)))

#define ALIGN_UP_POINTER_BY(address, alignment) \
    (ALIGN_DOWN_POINTER_BY(((ULONG_PTR)(address) + alignment - 1), alignment))

#define ALIGN_DOWN(length, type) \
    ALIGN_DOWN_BY(length, sizeof(type))

#define ALIGN_UP(length, type) \
    ALIGN_UP_BY(length, sizeof(type))

#define ALIGN_DOWN_POINTER(address, type) \
    ALIGN_DOWN_POINTER_BY(address, sizeof(type))

#define ALIGN_UP_POINTER(address, type) \
    ALIGN_UP_POINTER_BY(address, sizeof(type))

#pragma pack(push,1)

typedef struct tagFontLibHeader {
	BYTE		magic[4]; //'U'('S', 'M'), 'F', 'L', X---Unicode(Simple or MBCS), X: Version
	DWORD		dwFileSize; /* File total size */
	BYTE		nSection; /* total sections */
	BYTE		YSize; /* height of font */
	WORD		wCpFlag; /* codepageflag ÿ��bitλ��ʾһ���ַ����������ɱ�ʾ16���ַ�����*/
	WORD		nTotalChars; /* �ܵ���Ч�ַ��� */
	char		reserved[2];
} FL_Header, *PFL_Header;

typedef struct tagFlSectionInfo {
	WORD		First; /* first character */
	WORD		Last; /* last character */
	DWORD		OffAddr; /* ָ����ǵ�ǰSECTION������ UFL_CHAR_INFO��һ���ַ���Ϣ����ʼ��ַ */
} FL_SECTION_INF, *PFL_SECTION_INF;

typedef struct tagUflCharInfo {
	DWORD		OffAddr : 26; // ��ǰ�ַ��������ݵ���ʼ��ַ
	DWORD		Width : 6; // �ַ���������صĿ��( Ŀǰ���֧�ֵ��� < 64)
} UFL_CHAR_INDEX;

typedef struct _SECTION_NODE{
	struct _SECTION_NODE	*Next;
	FL_SECTION_INF	SectionInfo;
	UFL_CHAR_INDEX	*CharList;
} SECTION_NODE, *PSECTION_NODE;

#pragma pack(pop)

