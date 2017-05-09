
#pragma once

#include <EdkClass/Common.h>
#include <EdkClass/Video.h>

#include <Lib/XmlParserLib.h>

//��ʶ��Ļһ������
typedef struct _GUI_RECT
{
	UINT16 x;
	UINT16 y;
	UINT16 Width;
	UINT16 Height;
}GUI_RECT, *PGUI_RECT;

typedef struct _COMPONENT_ATTR
{

	/* �Ƿ�������� */
	BOOLEAN							Focus;

	/* �Ƿ�ɼ� */
	BOOLEAN							Visible;

	/* �Ƿ��ػ� */
	BOOLEAN							NeedRedraw;
	BOOLEAN							Reserve;

	/* ���� */
	CHAR16							Name[32];

	/* Idֵ */
	UINTN							Id;

	/* ��ͼ���� */
	GUI_RECT						Rect;

	/* ͸���� */
	UINT8							Alpha;

	/* ����� */
	VOID*							Parent;

	/* ���� */
	union {
		LIST_ENTRY					ChildList;
		LIST_ENTRY					FormList;
	};

	/* ���屳�� */
	EFI_HANDLE						FormBuffer;

	/* ������ǰ��������ͼ */
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
	��Ⱦ�����������������
	��һ�������Ի棬�����ݿؼ�������Ϣ����һ�������BITMAP
	�ڶ������ǵ��ӣ��������BITMAP���������������Ķ�Ӧ������е�����Ⱦ
	����ʱ�������֣���������ǲ�͸���ģ���ô�򵥵ļ����������һ�������
	������Ҫ������һ��Ļ�����Ϣ������ɫ���߱���ͼ������ALPHAͨ������
	*/

	VOID
	(EFIAPI *Draw)(
		VOID* vSelf,
		PGUI_RECT Rect
		);

	/* ���úͻ�ȡ������Ϣ */

	BOOLEAN
	(EFIAPI *SetFocus)(
		VOID* vSelf,
		BOOLEAN Focus
		);

	BOOLEAN
	(EFIAPI *GetFocus)(
		VOID* vSelf
		);

	/* ���ñ���ɫ������ͼ��͸��ͨ�� */

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

/* GUI��ͨ�÷��� */

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