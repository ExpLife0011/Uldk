
#include <EdkClass/Common.h>
#include <Lib/ResourceLib.h>

#include "ResourceMeta.h"
#include "Font.h"

/* 获得一行字符串的长度 */

UINT16
EFIAPI
ResLibGetStringWidth(
	IN EFI_HANDLE	ResourceHandle,
	IN CHAR16*		String
) {

	EFI_HANDLE CharHandle;
	UINT16 Width;

	Width = 0;

	for (UINTN i = 0;i < StrLen(String);i++) {

		CharHandle = ResLibFindCharCode(ResourceHandle, String[i]);
		Width += ResLibGetCharWidth(ResourceHandle, CharHandle);

	}

	return Width;

}

/* 获得字模点阵数据 */

PBYTE
EFIAPI
ResLibRetrieveCharMatrix(
	IN EFI_HANDLE	ResourceHandle,
	IN EFI_HANDLE	CharHandle
) {

	PRESOURCE_INSTANCE ResInst = (PRESOURCE_INSTANCE)ResourceHandle;
	/* 只认准第一个服务库 */
	UFL_CHAR_INDEX* CharIndex = (UFL_CHAR_INDEX*)CharHandle;

	return &((FONT_LIST_NODE*)ResInst->FontList.ForwardLink)->FontFile[CharIndex->OffAddr];

}

/* 获取字符高度 */

UINT16
EFIAPI
ResLibGetCharHeight(
	IN EFI_HANDLE	ResourceHandle,
	IN EFI_HANDLE	CharHandle
) {

	return 16;

}

/* 获取字符宽度 */

UINT16
EFIAPI
ResLibGetCharWidth(
	IN EFI_HANDLE	ResourceHandle,
	IN EFI_HANDLE	CharHandle
) {

	UFL_CHAR_INDEX* CharIndex = (UFL_CHAR_INDEX*)CharHandle;
	UINT16 Width = CharIndex->Width;

	if (Width > 0 && Width <= 8)
		Width = 8;
	else
		Width = 16;

	return Width;

}

/* 现在只认第一个字体库信息 */

EFI_HANDLE
EFIAPI
ResLibFindCharCode(
	IN EFI_HANDLE	ResourceHandle,
	IN CHAR16		CharCode
) {

	PRESOURCE_INSTANCE ResInst = (PRESOURCE_INSTANCE)ResourceHandle;
	PFONT_LIST_NODE FontNode = (PFONT_LIST_NODE)ResInst->FontList.ForwardLink;
	PFL_SECTION_INF Section;
	UFL_CHAR_INDEX* IndexList;

	for (UINTN i = 0;i < FontNode->FontHeader->nSection;i++) {

		Section = &FontNode->FirstSection[i];
		if ((CharCode >= Section->First) && (CharCode <= Section->Last)) {
			IndexList = (UFL_CHAR_INDEX*)&FontNode->FontFile[Section->OffAddr];
			return (EFI_HANDLE)&IndexList[CharCode - Section->First];
		}

	}

	return NULL;

}

EFI_STATUS
EFIAPI
ResLibLoadFont(
	IN EFI_HANDLE	ResourceHandle,
	IN VOID*		FontResource
) {

	PRESOURCE_INSTANCE ResInst = (PRESOURCE_INSTANCE)ResourceHandle;
	PFONT_LIST_NODE FontListNode = (PFONT_LIST_NODE)AllocateZeroPool(sizeof(*FontListNode));
	PFL_Header FlHeader;
	PFL_SECTION_INF FlSection;

	if (FontListNode == NULL)
		return EFI_UNSUPPORTED;

	FontListNode->FontFile = FontResource;
	FlHeader = (PFL_Header)FontResource;
	FlSection = (PFL_SECTION_INF)(FlHeader + 1);
	FontListNode->FontHeader = FlHeader;
	FontListNode->FirstSection = FlSection;

	InsertHeadList(&ResInst->FontList, (LIST_ENTRY*)&FontListNode->FontList);
	return EFI_SUCCESS;

}