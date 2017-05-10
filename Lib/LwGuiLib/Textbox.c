#pragma once

#include <EdkClass/Common.h>

#include <LwGUI/Component.h>
#include <LwGUI/LwGuiLib.h>
#include <LwGUI/TextBox.h>
#include <LwGui/Form.h>

#include <Lib/XmlParserLib.h>
#include <Lib/VideoLib.h>
#include <Color.h>

extern GUI_HANDLE*				GuiHandle;

extern COMPONENT_TYPE_LIST		TypeList[128];
extern UINTN					TypeListCount;


VOID
EFIAPI
Gui_Text_Destory(
	IN VOID* vSelf
) {

	PGUI_TEXT Self = 
		(PGUI_TEXT)vSelf;

	if (Self->Attr.BitmapObject != 0)
		VideoLibDestoryBitmapObject(Self->Attr.BitmapObject);

	FreePool(Self);

}

static
BOOLEAN
EFIAPI
Gui_Text_SetFocus(
	IN		VOID* vSelf,
	IN		BOOLEAN Focus
) {

	PGUI_TEXT Self = (PGUI_TEXT)vSelf;
	Self->Attr.Focus = Focus;

	Self->Attr.NeedRedraw = TRUE;

	return TRUE;

}

static
CHAR16*
EFIAPI
Gui_Text_GetText(
	IN		VOID* vSelf
) {

	PGUI_TEXT Self = (PGUI_TEXT)vSelf;
	
	return Self->Text;

}

static
VOID
EFIAPI
Gui_Text_SetText(
	IN	VOID*		vSelf,
	IN	CHAR16*		Text
) {

	PGUI_TEXT Self = (PGUI_TEXT)vSelf;
	StrCpy(Self->Text, Text);

	if (Self->Attr.BitmapObject) {
		VideoLibDestoryBitmapObject(Self->Attr.BitmapObject);
		Self->Attr.BitmapObject = NULL;
	}

	UINT16 Width = VideoLibGetStringWidth(Text);
	if (Width == 0)
		return;

	Self->Attr.BitmapObject = VideoLibCreateBitmapObject(BITMAP_FORMAT_RGB,
		Width,
		16
	);

	Self->Attr.FgColor.Reserved = 255;
	VideoLibRenderFont(Self->Attr.BitmapObject, 
		Text, 
		*((UINT32*)&Self->Attr.FgColor)
	);

}

static
VOID
EFIAPI
Gui_Text_KeyPress(
	IN		VOID* vSelf,
	IN		EFI_INPUT_KEY* Key
) {

	PGUI_TEXT Self = (PGUI_TEXT)vSelf;
	UINTN Len = StrLen(Self->Text);
	CHAR16 Temp[128];
	BOOLEAN TextChanged = FALSE;
	SetMem(Temp, 128 * 2, 0);

	switch (Key->ScanCode)
	{
	case 0:
		break;
	case SCAN_LEFT:
		if (Self->TextPos != 0)
			Self->TextPos--;
		break;
	case SCAN_RIGHT:
		if (Self->TextPos != StrLen(Self->Text))
			Self->TextPos++;
		break;
	case SCAN_DELETE:
		if (Self->TextPos != 0)
		{
			CopyMem(Temp, &Self->Text[Self->TextPos], (Len - Self->TextPos) * 2);
			CopyMem(&Self->Text[Self->TextPos - 1], Temp, (Len - Self->TextPos) * 2 + 2);
			Self->TextPos--;
		}
		break;
	default:
		break;
	}

	if (Key->UnicodeChar != 0)
	{
		CHAR16 Char = Key->UnicodeChar;

		if (Char == CHAR_BACKSPACE)
		{

			if (Self->TextPos != 0) {
				CopyMem(Temp, &Self->Text[Self->TextPos], (Len - Self->TextPos) * 2);
				CopyMem(&Self->Text[Self->TextPos - 1], Temp, (Len - Self->TextPos) * 2 + 2);
				Self->TextPos--;
			}
		}
		else if ((Char >= 'A'&&Char <= 'Z') ||
			(Char >= 'a'&&Char <= 'z') ||
			(Char >= '0'&&Char <= '9'))
		{
			if (Len >= 128)
				return;
			else
			{

				CopyMem(Temp, &Self->Text[Self->TextPos], (Len - Self->TextPos) * 2);
				Self->Text[Self->TextPos] = Key->UnicodeChar;
				if (Len - Self->TextPos == 0)
					Self->Text[Self->TextPos + 1] = 0;
				else
					CopyMem(&Self->Text[Self->TextPos + 1], Temp, (Len - Self->TextPos) * 2);
				Self->TextPos++;
			}
		}

	}

	Self->Attr.NeedRedraw = TRUE;

	if (Self->Attr.BitmapObject) {
		VideoLibDestoryBitmapObject(Self->Attr.BitmapObject);
		Self->Attr.BitmapObject = 0;
	}
	
	UINT16 Width = VideoLibGetStringWidth(Self->Text);
	if (Width == 0)
		return;

	Self->Attr.BitmapObject = VideoLibCreateBitmapObject(BITMAP_FORMAT_RGBA,
		Width,
		16
	);

	Self->Attr.FgColor.Reserved = 255;
	VideoLibRenderFont(Self->Attr.BitmapObject, 
		Self->Text, 
		*((UINT32*)&Self->Attr.FgColor)
	);

}

/* 绘制函数 */

static
VOID
EFIAPI
Gui_Text_Draw(VOID* vSelf,
	PGUI_RECT ScopeRect
) {

	EFI_STATUS Status = EFI_SUCCESS;
	PGUI_TEXT Self = (PGUI_TEXT)vSelf;

	/* 不需要重绘或者文本的位图信息没有 */

	if (!Self->Attr.NeedRedraw)
		return;

	/* 先计算出相对于父容器的位置 */

	GUI_RECT Rect;
	LwGuiGetAbsoluteRect(vSelf, &Rect);

	PGUI_FORM FormObject = (PGUI_FORM)LwGuiGetComponentForm((PCOMPONENT_COMMON_HEADER)Self);

	/* 创建渲染图像对象 */

	EFI_HANDLE TempBitmapObject = VideoLibCreateBitmapObject(
		BITMAP_FORMAT_RGB,
		Self->Ops.GetBound(Self)->Width,
		Self->Ops.GetBound(Self)->Height
	);

	/* 渲染背景色 */

	//Self->Attr.BgColor.Reserved = Self->Attr.Alpha;
	Self->Attr.BgColor.Reserved = 255;

	Status = VideoLibFillRect(TempBitmapObject,
		0,
		0,
		Rect.Width,
		Rect.Height,
		*((UINT32*)&Self->Attr.BgColor)
	);

	if (EFI_ERROR(Status)) {

		goto EXIT;

	}

	/* 渲染之前生成的字体位图 */

	Status = VideoLibBitblt(
		Self->Attr.BitmapObject,
		TempBitmapObject,
		0,
		0,
		5,
		5,
		VideoLibGetImageWidth(Self->Attr.BitmapObject),
		VideoLibGetImageWidth(Self->Attr.BitmapObject)
	);
	if (EFI_ERROR(Status)) {

		goto EXIT;
	
	}
	
	/* 如果拥有焦点就绘制光标 */

	if (Self->Attr.Focus)
	{

		/* 计算出光标的位置信息并且渲染 */

		GUI_RECT CursorRect;
		CursorRect.x = (UINT16)Self->TextPos * 16 / 2 + 5;
		CursorRect.y = 5;
		CursorRect.Width = 1;
		CursorRect.Height = 16;

		VideoLibFillRect(TempBitmapObject,
			CursorRect.x,
			CursorRect.y,
			CursorRect.Width,
			CursorRect.Height,
			0);

	}

	Status = VideoLibBitblt(TempBitmapObject,
		FormObject->SwapObject,
		0,
		0,
		Rect.x,
		Rect.y,
		Rect.Width,
		Rect.Height
	);

	Self->Attr.NeedRedraw = FALSE;

	return;

EXIT:

	VideoLibDestoryBitmapObject(TempBitmapObject);

	return;

}

/* 创建一个文本框对象 */

PGUI_TEXT
EFIAPI
CreateTextBox(
	IN		CHAR16*	Name
) {

	PGUI_TEXT Text = AllocatePool(sizeof(GUI_TEXT));
	SetMem(Text, sizeof(GUI_TEXT), 0);

	Text->Ops.GetBound =	Gui_Normal_GetBound;
	Text->Ops.SetBound =	Gui_Normal_SetBound;
	Text->Ops.SetFocus =	Gui_Text_SetFocus;
	Text->Ops.GetFocus =	Gui_Normal_GetFocus;
	Text->Ops.SetColor =	Gui_Normal_SetColor;
	Text->Ops.KeyPress =	Gui_Text_KeyPress;
	Text->Ops.SetAlpha =	Gui_Normal_SetAlpha;
	Text->Ops.Destory =		Gui_Text_Destory;

	Text->GetText =			Gui_Text_GetText;
	Text->SetText =			Gui_Text_SetText;
	Text->Ops.Draw =		Gui_Text_Draw;

	Text->Ops.SetColor(Text, &WhiteColor, &BlackColor);

	StrCpyS(Text->Attr.Name, 32, Name);
	Text->Attr.Visible = TRUE;
	Text->Attr.NeedRedraw = TRUE;
	Text->Attr.Alpha = 255;

	Text->Type = GUI_TYPE_TEXTBOX;

	return Text;

}

static
PCOMPONENT_COMMON_HEADER
Pharse_TextBox_XML(XML_SECTION* Xml, PCOMPONENT_COMMON_HEADER ParentNode)
{

	PGUI_TEXT CurrentNode;

	if (StrCmp(Xml->SectionName, L"TextBox") == 0)
	{

		CHAR16* NameStr;
		PKV_PAIR NamePair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Name");
		PKV_PAIR WidthPair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Width");
		PKV_PAIR HeightPair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Height");
		PKV_PAIR LeftPair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Left");
		PKV_PAIR TopPair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Top");
		PKV_PAIR Alpha = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Alpha");

		if (NamePair == NULL)
			NameStr = LwGuiGenerateName(L"TextBox");
		else
			NameStr = StrDup((CHAR16*)NamePair->Value);

		CurrentNode = CreateTextBox(NameStr);
		if (CurrentNode == NULL)
			return NULL;

		FreePool(NameStr);

		if (Alpha&&CurrentNode->Ops.SetAlpha)
			CurrentNode->Ops.SetAlpha(CurrentNode, (UINT8)StrDecimalToUintn((CHAR16*)Alpha->Value));

		CurrentNode->Attr.Parent = ParentNode;

		GUI_RECT Rect;
		Rect.x = LeftPair == NULL ? 0 : AttrReadValue(ParentNode->Ops.GetBound(ParentNode)->Width,
			(CHAR16*)LeftPair->Value);
		Rect.y = TopPair == NULL ? 0 : AttrReadValue(ParentNode->Ops.GetBound(ParentNode)->Height,
			(CHAR16*)TopPair->Value);
		Rect.Width = WidthPair == NULL ? 0 : AttrReadValue(ParentNode->Ops.GetBound(ParentNode)->Width,
			(CHAR16*)WidthPair->Value);
		Rect.Height = HeightPair == NULL ? 0 : AttrReadValue(ParentNode->Ops.GetBound(ParentNode)->Height,
			(CHAR16*)HeightPair->Value);
		CurrentNode->Ops.SetBound(CurrentNode, &Rect);

		if (Xml->SectionContent != NULL)
			CurrentNode->SetText(CurrentNode, 
				Xml->SectionContent
			);

		CurrentNode->Attr.Parent = ParentNode;
		//LwGuiRegisterComponent((PCOMPONENT_COMMON_HEADER)CurrentNode);
		return (PCOMPONENT_COMMON_HEADER)CurrentNode;

	}

	return NULL;

}

//static
VOID
RegisterTextBox()
{

	LwGuiRegisterComponentType(L"TextBox", CreateTextBox, Pharse_TextBox_XML);

}