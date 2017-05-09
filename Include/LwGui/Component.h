
#pragma once

#include <EdkClass/Common.h>
#include <EdkClass/Video.h>

#include <Lib/XmlParserLib.h>

//标识屏幕一个区域
typedef struct _GUI_RECT
{
	UINT16 x;
	UINT16 y;
	UINT16 Width;
	UINT16 Height;
}GUI_RECT, *PGUI_RECT;

typedef struct _COMPONENT_ATTR
{

	/* 是否包含焦点 */
	BOOLEAN							Focus;

	/* 是否可见 */
	BOOLEAN							Visible;

	/* 是否重绘 */
	BOOLEAN							NeedRedraw;
	BOOLEAN							Reserve;

	/* 名字 */
	CHAR16							Name[32];

	/* Id值 */
	UINTN							Id;

	/* 绘图区域 */
	GUI_RECT						Rect;

	/* 透明度 */
	UINT8							Alpha;

	/* 父组件 */
	VOID*							Parent;

	/* 链表 */
	union {
		LIST_ENTRY					ChildList;
		LIST_ENTRY					FormList;
	};

	/* 窗体背景 */
	EFI_HANDLE						FormBuffer;

	/* 背景，前景，背景图 */
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL	BgColor;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL	FgColor;

	EFI_HANDLE						BitmapObject;

}COMPONENT_ATTR, *PCOMPONENT_ATTR;

typedef struct _COMPONENT_OPS
{

	VOID
	(EFIAPI *SetBound)(
		VOID* vSelf,
		PGUI_RECT Rect
		);

	PGUI_RECT
	(EFIAPI *GetBound)(
		VOID* vSelf
		);

	/*
	渲染过程由两个部分组成
	第一部分是自绘，即根据控件自身信息生成一个区域的BITMAP
	第二部分是叠加，即将这个BITMAP与下面的所有组件的对应区域进行叠加渲染
	叠加时分两部分，如果自身是不透明的，那么简单的计算相对于下一层的区域
	否则需要根据下一层的绘制信息（背景色或者背景图）进行ALPHA通道计算
	*/

	VOID
	(EFIAPI *Draw)(
		VOID* vSelf,
		PGUI_RECT Rect
		);

	/* 设置和获取焦点信息 */

	BOOLEAN
	(EFIAPI *SetFocus)(
		VOID* vSelf,
		BOOLEAN Focus
		);

	BOOLEAN
	(EFIAPI *GetFocus)(
		VOID* vSelf
		);

	/* 设置背景色，背景图和透明通道 */

	VOID
	(EFIAPI *SetColor)(
		VOID* vSelf,
		EFI_GRAPHICS_OUTPUT_BLT_PIXEL* BgColor,
		EFI_GRAPHICS_OUTPUT_BLT_PIXEL* FgColor
		);

	VOID
	(EFIAPI *SetBackground)(
		VOID* vSelf,
		CHAR16* FileName
		);

	VOID
	(EFIAPI * SetAlpha)(
		VOID* vSelf,
		UINT8 Alpha
		);


	VOID
	(EFIAPI *KeyPress)(
		VOID* vSelf,
		EFI_INPUT_KEY* Key
		);

	VOID
	(EFIAPI* Destory)(
		IN VOID* vSelf
		);

}COMPONENT_OPS, *PCOMPONENT_OPS;

typedef struct _COMPONENT_COMMON_HEADER
{

	UINT16				Type;
	UINT16				Version;
	COMPONENT_ATTR		Attr;
	COMPONENT_OPS		Ops;

}COMPONENT_COMMON_HEADER, *PCOMPONENT_COMMON_HEADER;

/* GUI的通用方法 */

VOID
EFIAPI
Gui_Normal_SetBound(
	IN		VOID* vSelf,
	IN		PGUI_RECT Rect
);

PGUI_RECT
EFIAPI
Gui_Normal_GetBound(
	IN		VOID* vSelf
);

VOID
EFIAPI
Gui_Normal_SetColor(
	IN		VOID* vSelf,
	IN		EFI_GRAPHICS_OUTPUT_BLT_PIXEL* BgColor,
	IN		EFI_GRAPHICS_OUTPUT_BLT_PIXEL* FgColor
);

BOOLEAN
EFIAPI
Gui_Normal_SetFocus(
	IN		VOID* vSelf,
	IN		BOOLEAN Focus
);

BOOLEAN
EFIAPI
Gui_Normal_GetFocus(
	IN		VOID* vSelf
);

VOID
EFIAPI
Gui_Normal_SetBackground(
	VOID* vSelf,
	CHAR16* FileName
);

VOID
EFIAPI
Gui_Normal_SetAlpha(
	IN	VOID* vSelf,
	IN	UINT8 Alpha
);