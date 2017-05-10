
#pragma once

#include <Lib/XmlParserLib.h>
#include <LwGUI/Component.h>

#define GUI_CLASS_COMPONENT			0x01
#define GUI_CLASS_CONTAINER			0x02

#define GUI_FLAG_OWN_BUFFER			0x01
#define GUI_FLAG_CANT_FOCUS			0x02

//目前所有组件类型
#define GUI_TYPE_FORM				((GUI_CLASS_CONTAINER<<8)|0x1)
#define GUI_TYPE_TEXTBOX			((GUI_CLASS_COMPONENT<<8)|0x2)
#define GUI_TYPE_LABEL				((GUI_CLASS_COMPONENT<<8)|0x3)
#define GUI_TYPE_LISTVIEW			((GUI_CLASS_CONTAINER<<8)|0x4)
#define GUI_TYPE_HBOX				((GUI_CLASS_CONTAINER<<8)|0X5)
#define GUI_TYPE_IMAGE				((GUI_CLASS_CONTAINER<<8)|0X5)

/* 对齐类型 */
#define GUI_ALIGN_LEFT				0x1
#define GUI_ALIGN_CENTER			0x2
#define GUI_ALIGN_RIGHT				0x3

//组件类型表
typedef struct _COMPONENT_TYPE_LIST
{

	CHAR16* Name;

	//创建回调
	VOID*(*CP_CREATE)(
		CHAR16* Name
		);

	//XML识别解析回调
	VOID*(*Pharse_COMPONENT_XML)(
		XML_SECTION* Xml,
		VOID* ParentNode
		);

}COMPONENT_TYPE_LIST;

CHAR16 *
EFIAPI
StrDup(
	IN      CONST CHAR16              *Source
);

//GUI全局管理对象
typedef struct _GUI_HANDLE
{
	GUI_RECT						ScreenRect;
	EFI_GRAPHICS_OUTPUT_PROTOCOL	*GOP;
	EFI_HANDLE						ResourceHandle;
	VOID*							FormArray[8];
	VOID*							ActiveForm;
}GUI_HANDLE, *PGUI_HANDLE;

EFI_STATUS
EFIAPI
LwGuiInitialize(
);

EFI_GRAPHICS_OUTPUT_BLT_PIXEL*
EFIAPI
LwGuiInitializePixelBuffer(
	IN PGUI_RECT		Rect
);

VOID
EFIAPI
LwGuiFillRect(
	IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL* Vector,
	IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL* Color,
	IN GUI_RECT* AllArea,
	IN GUI_RECT* FillArea
);

EFI_STATUS
EFIAPI
LwGuiGetAbsoluteRect(
	IN PCOMPONENT_COMMON_HEADER Component,
	OUT PGUI_RECT Absolute
);

EFI_STATUS
EFIAPI
LwGuiGetRelativeRect(
	IN PCOMPONENT_COMMON_HEADER Component,
	OUT PGUI_RECT Absolute
);

CHAR16*
EFIAPI
LwGuiGenerateName(
	IN CHAR16* BaseName
);

EFI_STATUS
EFIAPI
LwGuiClearScreen(
);

PCOMPONENT_COMMON_HEADER
EFIAPI
LwGuiGetComponentForm(
	IN PCOMPONENT_COMMON_HEADER Component
);

VOID
EFIAPI
LwGuiLoadFont(
	IN CHAR8* FontModule
);

XML_SECTION*
EFIAPI
LwGuiParserFormSection(
	IN WCHAR* FormXML
);

COMPONENT_COMMON_HEADER*
EFIAPI
LwGuiGetComponent(
	IN WCHAR* Name
);

UINT16
EFIAPI
AttrReadAlignment(
	IN	CHAR16*		Value
);

VOID
EFIAPI
LwGuiRegisterComponentType(
	IN CHAR16*	Name,
	IN VOID*	CreateProc,
	IN VOID*	ParserProc
);

BOOLEAN
EFIAPI
AttrReadColor(
	IN	CHAR16*		Value,
	IN	EFI_GRAPHICS_OUTPUT_BLT_PIXEL* Color
);

UINT16
EFIAPI
AttrReadAlignment(
	IN	CHAR16*		Value
);

UINT16
EFIAPI
AttrReadValue(
	UINT16 OriValue,
	CHAR16* Value
);
