#pragma once

#include <EdkClass/Common.h>
#include <LwGUI/LwGuiLib.h>
#include <LwGUI/Component.h>

#define GUI_TYPE_STRING 0x1001

typedef struct _GUI_STRING
{
	UINT16				Type;
	UINT16				Version;
	CHAR16*				String;
}GUI_STRING, *PGUI_STRING;

typedef struct _GUI_LISTVIEW_NODE
{

	UINT16				Type;
	UINT16				Version;
	COMPONENT_ATTR		Attr;
	COMPONENT_OPS		Ops;

	UINTN*				Content;

}GUI_LISTVIEW_NODE, *PGUI_LISTVIEW_NODE;

typedef struct _GUI_LISTVIEW_ITEM
{

	CHAR16*			ItemName;
	VOID*			ItemContent[8];
	VOID*			ItemContext;
	UINT16			ItemWidth;
	UINT16			ItemHeight;
	BOOLEAN			ItemSizeAutoSense;

}GUI_LISTVIEW_ITEM, *PGUI_LISTVIEW_ITEM;

typedef struct _GUI_LISTVIEW
{

	UINT16				Type;
	UINT16				Version;
	COMPONENT_ATTR		Attr;
	COMPONENT_OPS		Ops;

	EFI_GRAPHICS_OUTPUT_BLT_PIXEL ItemFgColor;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL HeaderFgColor;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL ItemBgColor;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL HeaderBgColor;

	CHAR16* Header[8];
	UINT16 HeaderWidth[8];
	UINT8 HeaderCount;

	GUI_LISTVIEW_ITEM Items[8];
	UINT8 ItemCount;

	UINT8 SelectItem;

	VOID(*SetColor)(
		IN VOID* vSelf,
		IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL* BgColor,
		IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL* FgColor
		);

	BOOLEAN(*AddColumnHeader)(
		IN VOID* vSelf,
		IN CHAR16*	HeaderName,
		IN UINT16	Width
		);

	BOOLEAN(*AddItem)(
		IN VOID* vSelf,
		IN CHAR16* Name,
		IN UINT16 Height,
		IN VOID* Context,
		IN BOOLEAN Auto
		);

	BOOLEAN(*SetItemContent)(
		IN VOID* vSelf,
		IN UINT16	ColumnIndex,
		IN UINT16	RowIndex,
		IN UINTN*	Content
		);

	VOID(*SetItemContext)(
		IN VOID*	vSelf,
		IN UINT16	RowIndex,
		IN UINTN*	Context
		);

}GUI_LISTVIEW, *PGUI_LISTVIEW;

PGUI_LISTVIEW
CreateListview(
	IN CHAR16* Name
);