#pragma once

#include <EdkClass/Common.h>
#include <LwGUI/Component.h>
#include <LwGUI/LwGuiLib.h>
#include <LwGUI/Label.h>
#include <LwGui/Form.h>

#include <Lib/XmlParserLib.h>
#include <Lib/VideoLib.h>
#include <Lib/ResourceLib.h>

#include <Color.h>

extern GUI_HANDLE*				GuiHandle;

/* 标签绘制函数 */

static
VOID
EFIAPI
Gui_Label_Draw(VOID* vSelf,
	PGUI_RECT ScopeRect
) {

	EFI_STATUS Status = EFI_SUCCESS;
	PGUI_LABEL Self = (PGUI_LABEL)vSelf;

	if (Self->Attr.NeedRedraw == FALSE)
		return;

	GUI_RECT AbsRect;
	LwGuiGetAbsoluteRect((PCOMPONENT_COMMON_HEADER)Self, &AbsRect);

	/* 临时缓冲区 */
	EFI_HANDLE TmpCanvas = VideoLibCreateBitmapObject(BITMAP_FORMAT_RGB, AbsRect.Width, AbsRect.Height);
	if (TmpCanvas == NULL)
		return;

	/* 背景色缓冲区 */
	EFI_HANDLE FillLevel = VideoLibCreateBitmapObject(BITMAP_FORMAT_RGBA, AbsRect.Width, AbsRect.Height);
	if (FillLevel == NULL) {
		VideoLibDestoryBitmapObject(TmpCanvas);
		return;
	}

	PGUI_FORM FormObject = (PGUI_FORM)LwGuiGetComponentForm((PCOMPONENT_COMMON_HEADER)Self);

	/* 把窗体对应区域的位图拷贝到临时缓冲区 */
	Status = VideoLibBitblt(FormObject->Attr.BitmapObject,
		TmpCanvas,
		AbsRect.x,
		AbsRect.y,
		0,
		0,
		AbsRect.Width,
		AbsRect.Height
	);
	if (EFI_ERROR(Status))
		goto EXIT;

	Self->Attr.BgColor.Reserved = Self->Attr.Alpha;

	/* 填充背景色临时缓冲区 */
	Status = VideoLibFillRect(FillLevel,
		0, 
		0,
		AbsRect.Width,
		AbsRect.Height,
		*((UINT32*)&Self->Attr.BgColor)
	);
	if (EFI_ERROR(Status))
		goto EXIT;

	/* 把背景色缓冲区覆盖到窗体缓冲区 */
	Status = VideoLibBitblt(FillLevel,
		TmpCanvas,
		0,
		0,
		0,
		0,
		AbsRect.Width,
		AbsRect.Height);
	if (EFI_ERROR(Status))
		goto EXIT;

	/* 把文字填充到交换缓冲区 */
	VideoLibBitblt(Self->Attr.BitmapObject,
		TmpCanvas,
		0,
		0,
		0,
		0,
		AbsRect.Width,
		AbsRect.Height
	);

	GuiHandle->GOP->Blt(
		GuiHandle->GOP,
		VideoLibGetImageBitmapData(TmpCanvas),
		EfiBltBufferToVideo,
		0,
		0,
		0,
		0,
		AbsRect.Width,
		AbsRect.Height,
		10 * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
	);

	while (1);

	/* 把临时缓冲区填充到交换缓冲区 */
	Status = VideoLibBitblt(TmpCanvas,
		FormObject->SwapObject, 
		0, 
		0,
		0,
		0, 
		AbsRect.Width,
		AbsRect.Height
	);
	if (EFI_ERROR(Status))
		goto EXIT;

	Print(L"ccc\n");

	/* 把文字填充到交换缓冲区 */
	VideoLibBitblt(Self->Attr.BitmapObject,
		FormObject->SwapObject, 
		0,
		0,
		AbsRect.x,
		AbsRect.y,
		AbsRect.Width,
		AbsRect.Height
	);

	Print(L"ccc\n");

EXIT:

	FreePool(TmpCanvas);

	FreePool(FillLevel);

}

/* 不接受焦点设置 */

static
BOOLEAN
EFIAPI
Gui_Label_SetFocus(
	IN		VOID* vSelf,
	IN		BOOLEAN Focus)
{

	return FALSE;

}

/* 直接返回没有焦点 */

static
BOOLEAN
EFIAPI
Gui_Label_GetFocus(
	IN		VOID* vSelf
) {

	return FALSE;

}

/* 设置文本并即时生成文本的位图信息 */

static
VOID
EFIAPI
Gui_Label_SetText(
	IN	VOID*		vSelf,
	IN	CHAR16*		Text
) {

	PGUI_LABEL Self = (PGUI_LABEL)vSelf;
	Self->Text = StrDup(Text);

	if (Self->Attr.BitmapObject) {

		VideoLibDestoryBitmapObject(Self->Attr.BitmapObject);
		Self->Attr.BitmapObject = NULL;
	
	}

	Self->Attr.NeedRedraw = TRUE;
	
	UINT16 Width = VideoLibGetStringWidth(Self->Text);
	if (Width == 0) {
		return;
	}

	Self->Attr.BitmapObject = VideoLibCreateBitmapObject(BITMAP_FORMAT_RGB,
		Width,
		16
	);

	Self->Attr.FgColor.Reserved = 255;
	VideoLibRenderFont(Self->Attr.BitmapObject, Text, *((UINT32*)&Self->Attr.FgColor));

	Print(L"Label-SetText\n");

}

/* 设置标签透明度 */

static
VOID
EFIAPI
Gui_Label_SetTransparent(
	IN	VOID*		vSelf,
	IN	BOOLEAN		Transparent
) {

	PGUI_LABEL Self = (PGUI_LABEL)vSelf;
	if (Self->Transparent == Transparent)
		return;

	Self->Transparent = Transparent;

	if (Transparent)
		Self->Attr.BgColor.Reserved = 0;
	else
		Self->Attr.BgColor.Reserved = 255;

	Self->Attr.NeedRedraw = TRUE;

}

VOID
EFIAPI
Gui_Label_Destory(
	IN VOID* vSelf
) {

	PGUI_LABEL Self = (PGUI_LABEL)vSelf;

	if (Self->Text)
		FreePool(Self->Text);

	if (Self->Attr.BitmapObject != 0)
		VideoLibDestoryBitmapObject(Self->Attr.BitmapObject);

	FreePool(Self);

}

PGUI_LABEL
EFIAPI
CreateLabel(
	IN		CHAR16*	Name
) {

	PGUI_LABEL Label = AllocateZeroPool(sizeof(GUI_LABEL));

	Label->Ops.Draw =		Gui_Label_Draw;
	Label->Ops.GetBound =	Gui_Normal_GetBound;
	Label->Ops.SetBound =	Gui_Normal_SetBound;
	Label->Ops.SetColor =	Gui_Normal_SetColor;
	Label->Ops.SetAlpha =	Gui_Normal_SetAlpha;
	Label->Ops.Destory =	Gui_Label_Destory;

	Label->Ops.GetFocus =	Gui_Label_GetFocus;
	Label->Ops.SetFocus =	Gui_Label_SetFocus;
	Label->SetText =		Gui_Label_SetText;
	Label->SetTransparent = Gui_Label_SetTransparent;

	Label->Ops.SetColor(Label, &RedColor, &WhiteColor);

	StrCpyS(Label->Attr.Name, 32, Name);
	Label->Attr.NeedRedraw = FALSE;
	Label->Attr.Visible = TRUE;
	Label->Attr.Alpha = 50;
	Label->Alignment = GUI_ALIGN_LEFT;

	return Label;

}

//XML解析

static
PCOMPONENT_COMMON_HEADER
Pharse_Label_XML(
	XML_SECTION* Xml, 
	PCOMPONENT_COMMON_HEADER ParentNode
) {

	PGUI_LABEL CurrentNode;

	if (StrCmp(Xml->SectionName, L"Label") == 0)
	{

		CHAR16* NameStr;

		PKV_PAIR NamePair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Name");
		PKV_PAIR WidthPair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Width");
		PKV_PAIR HeightPair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Height");
		PKV_PAIR LeftPair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Left");
		PKV_PAIR TopPair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Top");
		PKV_PAIR Alpha = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Alpha");
		PKV_PAIR AlignmentPair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Alignment");

		if (NamePair == NULL)
			NameStr = LwGuiGenerateName(L"Label");
		else
			NameStr = StrDup((CHAR16*)NamePair->Value);

		CurrentNode = CreateLabel(NameStr);
		if (CurrentNode == NULL)
			return NULL;

		FreePool(NameStr);

		if (Alpha&&CurrentNode->Ops.SetAlpha)
			CurrentNode->Ops.SetAlpha(CurrentNode, (UINT8)StrDecimalToUintn((CHAR16*)Alpha->Value));

		CurrentNode->Attr.Parent = ParentNode;

		GUI_RECT Rect = { 0 };
		Rect.x = (LeftPair == NULL) ? 0 : AttrReadValue(ParentNode->Ops.GetBound(ParentNode)->Width,
			(CHAR16*)LeftPair->Value);
		Rect.y = (TopPair == NULL) ? 0 : AttrReadValue(ParentNode->Ops.GetBound(ParentNode)->Height,
			(CHAR16*)TopPair->Value);
		Rect.Width = (WidthPair == NULL) ? 0 : AttrReadValue(ParentNode->Ops.GetBound(ParentNode)->Width,
			(CHAR16*)WidthPair->Value);
		Rect.Height = (HeightPair == NULL) ? 0 : AttrReadValue(ParentNode->Ops.GetBound(ParentNode)->Height,
			(CHAR16*)HeightPair->Value);
		CurrentNode->Ops.SetBound(CurrentNode, &Rect);

		if (AlignmentPair) {
			CurrentNode->Alignment = AttrReadAlignment((CHAR16*)AlignmentPair->Value);
		}

		if (Xml->SectionContent != NULL)
			CurrentNode->SetText(CurrentNode, Xml->SectionContent);

		//不存储额外信息，子节点忽略
		CurrentNode->Attr.Parent = ParentNode;

		//LwGuiRegisterComponent((PCOMPONENT_COMMON_HEADER)CurrentNode);
		return (PCOMPONENT_COMMON_HEADER)CurrentNode;

	}

	return NULL;

}

//static
VOID
RegisterLabel()
{

	LwGuiRegisterComponentType(L"Label", CreateLabel, Pharse_Label_XML);

}
