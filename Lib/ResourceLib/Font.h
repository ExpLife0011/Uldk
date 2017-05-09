#pragma once

#include <ExtTypes.h>

#pragma pack(push,1)

typedef struct tagFontLibHeader {
	BYTE		magic[4]; //'U'('S', 'M'), 'F', 'L', X---Unicode(Simple or MBCS), X: Version
	DWORD		dwFileSize; /* File total size */
	BYTE		nSection; /* total sections */
	BYTE		YSize; /* height of font */
	WORD		wCpFlag; /* codepageflag 每个bit位表示一个字符集。即最多可表示16个字符集。*/
	WORD		nTotalChars; /* 总的有效字符数 */
	char		reserved[2];
} FL_Header, *PFL_Header;

typedef struct tagFlSectionInfo {
	WORD		First; /* first character */
	WORD		Last; /* last character */
	DWORD		OffAddr; /* 指向的是当前SECTION包含的 UFL_CHAR_INFO第一个字符信息的起始地址 */
} FL_SECTION_INF, *PFL_SECTION_INF;

typedef struct tagUflCharInfo {
	DWORD		OffAddr : 26; // 当前字符点阵数据的起始地址
	DWORD		Width : 6; // 字符点阵的像素的宽度( 目前最大支持点阵 < 64)
} UFL_CHAR_INDEX;

#pragma pack(pop)

typedef struct _SECTION_NODE {

	struct _SECTION_NODE	*Next;
	
	FL_SECTION_INF			SectionInfo;
	
	UFL_CHAR_INDEX			*CharList;

} SECTION_NODE, *PSECTION_NODE;

/* 字体库链表 */

typedef struct _FONT_LIST_NODE{
	
	LIST_ENTRY FontList;

	CHAR16* FontName;

	PBYTE FontFile;

	PFL_Header FontHeader;

	PFL_SECTION_INF FirstSection;

}FONT_LIST_NODE, *PFONT_LIST_NODE;