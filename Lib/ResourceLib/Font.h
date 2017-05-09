#pragma once

#include <ExtTypes.h>

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

#pragma pack(pop)

typedef struct _SECTION_NODE {

	struct _SECTION_NODE	*Next;
	
	FL_SECTION_INF			SectionInfo;
	
	UFL_CHAR_INDEX			*CharList;

} SECTION_NODE, *PSECTION_NODE;

/* ��������� */

typedef struct _FONT_LIST_NODE{
	
	LIST_ENTRY FontList;

	CHAR16* FontName;

	PBYTE FontFile;

	PFL_Header FontHeader;

	PFL_SECTION_INF FirstSection;

}FONT_LIST_NODE, *PFONT_LIST_NODE;