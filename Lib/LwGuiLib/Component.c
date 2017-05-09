
#include <EdkClass/Common.h>
#include <LwGUI/LwGuiLib.h>
#include <LwGUI/Component.h>
#include <Lib/VideoLib.h>
#include <intrin.h>

extern GUI_HANDLE*				GuiHandle;

VOID
EFIAPI
Gui_Normal_SetBound(
	IN		VOID* vSelf,
	IN		PGUI_RECT Rect
) {

	PCOMPONENT_COMMON_HEADER Self = 
		(PCOMPONENT_COMMON_HEADER)vSelf;

	CopyMem(&Self->Attr.Rect, Rect, sizeof(GUI_RECT));
	Self->Attr.NeedRedraw = TRUE;

}

PGUI_RECT
EFIAPI
Gui_Normal_GetBound(
	IN		VOID* vSelf
) {

	PCOMPONENT_COMMON_HEADER Self = 
		(PCOMPONENT_COMMON_HEADER)vSelf;
	return &Self->Attr.Rect;

}

VOID
EFIAPI
Gui_Normal_SetColor(
	IN		VOID* vSelf,
	IN		EFI_GRAPHICS_OUTPUT_BLT_PIXEL* BgColor,
	IN		EFI_GRAPHICS_OUTPUT_BLT_PIXEL* FgColor
) {

	PCOMPONENT_COMMON_HEADER Self = 
		(PCOMPONENT_COMMON_HEADER)vSelf;

	if (BgColor != NULL)
	{

		CopyMem(&Self->Attr.BgColor, 
			BgColor, 
			sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
		);
		Self->Attr.BgColor.Reserved = 255;

	}

	if (FgColor != NULL)
	{

		CopyMem(&Self->Attr.FgColor, 
			FgColor, 
			sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
		);
		Self->Attr.FgColor.Reserved = 255;

	}

	Self->Attr.NeedRedraw = TRUE;

}

BOOLEAN
EFIAPI
Gui_Normal_SetFocus(
	IN		VOID* vSelf,
	IN		BOOLEAN Focus
) {

	PCOMPONENT_COMMON_HEADER Self = 
		(PCOMPONENT_COMMON_HEADER)vSelf;

	Self->Attr.Focus = Focus;
	Self->Attr.NeedRedraw = TRUE;

	return TRUE;

}

BOOLEAN
EFIAPI
Gui_Normal_GetFocus(
	IN		VOID* vSelf
) {

	PCOMPONENT_COMMON_HEADER Self = 
		(PCOMPONENT_COMMON_HEADER)vSelf;

	return Self->Attr.Focus;

}


VOID
EFIAPI
Gui_Normal_SetBackground(
	VOID* vSelf,
	CHAR16* FileName
) {

	PCOMPONENT_COMMON_HEADER Self = 
		(PCOMPONENT_COMMON_HEADER)vSelf;

	Self->Attr.BitmapObject = VideoLibCreateImage(
		FileName,
		Self->Attr.Rect.Width,
		Self->Attr.Rect.Height
	);
	Self->Attr.NeedRedraw = TRUE;

}

VOID
EFIAPI
Gui_Normal_SetAlpha(
	IN	VOID* vSelf,
	IN	UINT8 Alpha
) {

	PCOMPONENT_COMMON_HEADER Self = 
		(PCOMPONENT_COMMON_HEADER)vSelf;

	Self->Attr.Alpha = Alpha;
	Self->Attr.BgColor.Reserved = Alpha;
	Self->Attr.NeedRedraw = TRUE;

}