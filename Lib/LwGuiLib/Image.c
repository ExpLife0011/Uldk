#pragma once

#include <EdkClass/Common.h>
#include <LwGUI/Component.h>
#include <LwGUI/LwGuiLib.h>
#include <LwGUI/Image.h>
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
Gui_Image_Draw(VOID* vSelf,
	PGUI_RECT ScopeRect
) {

	EFI_STATUS Status = EFI_SUCCESS;
	PGUI_IMAGE Self = (PGUI_IMAGE)vSelf;
	UINT16 StartX = 0;

	if (Self->Attr.NeedRedraw == FALSE)
		return;

	GUI_RECT AbsRect;
	LwGuiGetAbsoluteRect((PCOMPONENT_COMMON_HEADER)Self, &AbsRect);

	PGUI_FORM FormObject = (PGUI_FORM)LwGuiGetComponentForm((PCOMPONENT_COMMON_HEADER)Self);

	/* 把文字填充到交换缓冲区 */
	VideoLibBitblt(Self->Attr.BitmapObject,
		FormObject->SwapObject,
		0,
		0,
		AbsRect.x + StartX,
		AbsRect.y + 5,
		VideoLibGetImageWidth(Self->Attr.BitmapObject),
		VideoLibGetImageHeight(Self->Attr.BitmapObject)
	);

}

VOID
EFIAPI
Gui_Image_SetBackground(
	VOID* vSelf,
	CHAR16* FileName
) {

	PGUI_IMAGE Self = (PGUI_IMAGE)vSelf;

	if (Self->Attr.BitmapObject != NULL) {
		VideoLibDestoryBitmapObject(Self->Attr.BitmapObject);
		Self->Attr.BitmapObject = NULL;
	}

	Self->Attr.BitmapObject = VideoLibCreateImage(
		FileName,
		Self->Attr.Rect.Width,
		Self->Attr.Rect.Height
	);

}

/* 不接受焦点设置 */

static
BOOLEAN
EFIAPI
Gui_Image_SetFocus(
	IN		VOID* vSelf,
	IN		BOOLEAN Focus)
{

	return FALSE;

}

/* 直接返回没有焦点 */

static
BOOLEAN
EFIAPI
Gui_Image_GetFocus(
	IN		VOID* vSelf
) {

	return FALSE;

}

/* 设置文本并即时生成文本的位图信息 */

VOID
EFIAPI
Gui_Image_Destory(
	IN VOID* vSelf
) {

	PGUI_IMAGE Self = (PGUI_IMAGE)vSelf;

	if (Self->Attr.BitmapObject != 0)
		VideoLibDestoryBitmapObject(Self->Attr.BitmapObject);

	FreePool(Self);

}

PGUI_IMAGE
EFIAPI
CreateImage(
	IN		CHAR16*	Name
) {

	PGUI_IMAGE Image = AllocateZeroPool(sizeof(GUI_LABEL));

	Image->Ops.Draw = Gui_Image_Draw;
	Image->Ops.GetBound = Gui_Normal_GetBound;
	Image->Ops.SetBound = Gui_Normal_SetBound;
	Image->Ops.SetBackground = Gui_Image_SetBackground;
	
	Image->Ops.Destory = Gui_Image_Destory;

	Image->Ops.GetFocus = Gui_Image_GetFocus;
	Image->Ops.SetFocus = Gui_Image_SetFocus;

	StrCpyS(Image->Attr.Name, 32, Name);
	Image->Attr.NeedRedraw = FALSE;
	Image->Attr.Visible = TRUE;
	Image->Type = GUI_TYPE_IMAGE;

	return Image;

}

//XML解析

static
PCOMPONENT_COMMON_HEADER
Pharse_Image_XML(
	XML_SECTION* Xml,
	PCOMPONENT_COMMON_HEADER ParentNode
) {

	PGUI_IMAGE CurrentNode;

	if (StrCmp(Xml->SectionName, L"Label") == 0)
	{

		CHAR16* NameStr;

		PKV_PAIR NamePair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Name");
		PKV_PAIR WidthPair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Width");
		PKV_PAIR HeightPair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Height");
		PKV_PAIR LeftPair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Left");
		PKV_PAIR TopPair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Top");

		if (NamePair == NULL)
			NameStr = LwGuiGenerateName(L"Image");
		else
			NameStr = StrDup((CHAR16*)NamePair->Value);

		CurrentNode = CreateImage(NameStr);
		if (CurrentNode == NULL)
			return NULL;

		FreePool(NameStr);

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

		PKV_PAIR BgPair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Background");
		if (BgPair)
			CurrentNode->Ops.SetBackground(CurrentNode, (CHAR16*)BgPair->Value);

		//不存储额外信息，子节点忽略
		CurrentNode->Attr.Parent = ParentNode;

		return (PCOMPONENT_COMMON_HEADER)CurrentNode;

	}

	return NULL;

}

//static
VOID
RegisterImage()
{

	LwGuiRegisterComponentType(L"Image", CreateImage, Pharse_Image_XML);

}
