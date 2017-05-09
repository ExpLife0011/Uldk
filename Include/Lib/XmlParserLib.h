#pragma once

#include <EdkClass/Common.h>
#include <Lib/HashKVLib.h>

#define ENABLE_HASHKV

typedef CHAR16			WCHAR;

#define SECTYPE_TEXT		1
#define SECTYPE_NODE		2

typedef struct _XML_SECTION
{
	CHAR16*					SectionName;
	struct _XML_SECTION*	Parent;

	UINT32					SectionType;
	UINT32					Padding;

	/* 属性列表 */
	CHAR16*					Attributes[10];
	CHAR16*					Value[10];
	UINTN					AttrCount;
	HASH_KV					AttrHashKV;

	/* 子节点列表 */
	VOID*					SectionContent;
	struct _XML_SECTION*	Children[32];
	UINTN					ChildrenCount;
	HASH_KV					ChildrenHashKV;

}XML_SECTION, *PXML_SECTION;

UINT16
EFIAPI
TranslateAttrToValue(
	UINT16 OriValue,
	CHAR16* Value
);

BOOLEAN
EFIAPI
XpReadSection(
	CHAR16* SectionStr,
	XML_SECTION** Section,
	CHAR16** Next
);

VOID
EFIAPI
XpFreeSection(
	XML_SECTION* CurSection
);

XML_SECTION*
EFIAPI
XpFindFirstNodeByName(
	XML_SECTION* Root,
	CHAR16* Name
);

XML_SECTION*
EFIAPI
XpFindNodeById(
	XML_SECTION* Root,
	CHAR16* Id);