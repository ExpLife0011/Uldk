
#include  <EdkClass/Common.h>

#include <LwGUI/LwGuiLib.h>
#include <LwGUI/Component.h>
#include <LwGUI/ListView.h>
#include <Lib/XmlParserLib.h>
#include <Lib/VideoLib.h>
#include <Color.h>

extern PGUI_HANDLE GuiHandle;

extern COMPONENT_TYPE_LIST TypeList[128];
extern UINTN TypeListCount;

static
VOID
Gui_ListView_SetBound(
	IN		VOID* vSelf,
	IN		PGUI_RECT Rect
) {

	PCOMPONENT_COMMON_HEADER Self = (PCOMPONENT_COMMON_HEADER)vSelf;
	Self->Attr.NeedRedraw = TRUE;
	CopyMem(&Self->Attr.Rect, Rect, sizeof(GUI_RECT));

}

static
PGUI_RECT
Gui_ListView_GetBound(
	IN		VOID* vSelf
) {

	PCOMPONENT_COMMON_HEADER Self = (PCOMPONENT_COMMON_HEADER)vSelf;
	return &Self->Attr.Rect;

}

VOID
EFIAPI
Gui_ListView_SetAlpha(
	IN	VOID* vSelf,
	IN	UINT8 Alpha
) {

	PGUI_LISTVIEW Self = (PGUI_LISTVIEW)vSelf;
	Self->Attr.NeedRedraw = TRUE;

	if (Alpha == Self->Attr.Alpha)
		return;

	Self->Attr.Alpha = Alpha;

	for (UINT16 i = 0;i < Self->HeaderCount;i++)
	{
		for (UINT16 j = 0;j < Self->ItemCount;j++)
		{
			if (Self->Items[j].ItemContent[i])
			{

				PCOMPONENT_COMMON_HEADER Cop = (PCOMPONENT_COMMON_HEADER)Self->Items[j].ItemContent[i];
				Cop->Ops.SetAlpha(Cop, Alpha);

			}
		}
	}

}

static
BOOLEAN
Gui_ListView_AddColumnHeader(
	IN VOID* vSelf,
	IN CHAR16*	HeaderName,
	IN UINT16	Width
) {

	PGUI_LISTVIEW ListView = (PGUI_LISTVIEW)vSelf;
	if (ListView->HeaderCount == 8)
		return FALSE;

	if (Width == 0)
		Width = 80;

	ListView->Header[ListView->HeaderCount] = StrDup(HeaderName);
	ListView->HeaderWidth[ListView->HeaderCount] = Width;
	ListView->HeaderCount++;
	ListView->Attr.NeedRedraw = TRUE;
	return TRUE;

}

static
VOID
Gui_ListView_KeyPress(
	IN		VOID* vSelf,
	IN		EFI_INPUT_KEY* Key
) {

	PGUI_LISTVIEW ListView = (PGUI_LISTVIEW)vSelf;
	if (Key->ScanCode == 0)
		return;

	switch (Key->ScanCode)
	{
	case SCAN_UP:

		if (ListView->SelectItem != 0)
		{
			ListView->SelectItem--;
			ListView->Attr.NeedRedraw = TRUE;
		}
		break;

	case SCAN_DOWN:

		if (ListView->SelectItem != ListView->ItemCount - 1)
		{
			ListView->SelectItem++;
			ListView->Attr.NeedRedraw = TRUE;
		}
		break;

	default:
		break;
	}

	return;

}

/* 在listview中添加行项 */

static
BOOLEAN
Gui_ListView_AddItem(
	IN VOID* vSelf,
	IN CHAR16* Name,
	IN UINT16 Height,
	IN VOID* Context,
	IN BOOLEAN Auto
) {

	PGUI_LISTVIEW ListView = (PGUI_LISTVIEW)vSelf;
	if (ListView->ItemCount > 16)
		return FALSE;

	ListView->Items[ListView->ItemCount].ItemName = StrDup(Name);
	ListView->Items[ListView->ItemCount].ItemContext = Context;

	if (Height == 0)
		Height = 30;

	ListView->Items[ListView->ItemCount].ItemHeight = Height;
	ListView->Items[ListView->ItemCount].ItemSizeAutoSense = Auto;

	ListView->ItemCount++;

	ListView->Attr.NeedRedraw = TRUE;
	return TRUE;

}

/* 获得节点内容 */

static
BOOLEAN
Gui_ListView_GetItemContent(
	IN VOID*				vSelf,
	IN UINT16				RowIndex,
	OUT GUI_LISTVIEW_ITEM** Item
) {

	PGUI_LISTVIEW ListView = (PGUI_LISTVIEW)vSelf;
	if (RowIndex >= ListView->ItemCount)
		return FALSE;

	*Item = &ListView->Items[RowIndex];
	return TRUE;

}

static
VOID
Gui_ListView_SetItemContext(
	IN VOID*	vSelf,
	IN UINT16	RowIndex,
	IN VOID*	Context
) {

	PGUI_LISTVIEW ListView = (PGUI_LISTVIEW)vSelf;
	ListView->Items[RowIndex].ItemContext = Context;
	return;

}

static
BOOLEAN
Gui_ListView_SetItemContent(
	IN VOID*	vSelf,
	IN UINT16	ColumnIndex,
	IN UINT16	RowIndex,
	IN UINTN*	Content
) {

	PGUI_LISTVIEW ListView = (PGUI_LISTVIEW)vSelf;
	if ((ColumnIndex >= ListView->HeaderCount) || (RowIndex >= ListView->ItemCount))
		return FALSE;

	PCOMPONENT_COMMON_HEADER Component = (PCOMPONENT_COMMON_HEADER)Content;
	BOOLEAN Auto = ListView->Items[RowIndex].ItemSizeAutoSense;

	ListView->Items[RowIndex].ItemContent[ColumnIndex] = (VOID*)Content;
	UINT16 FrontHeigth = 0, FrontWidth = 0;
	for (UINTN i = 0;i < RowIndex;i++)
		FrontHeigth += ListView->Items[i].ItemHeight;
	for (UINTN i = 0;i < ColumnIndex;i++)
		FrontWidth += ListView->HeaderWidth[i];

	Component->Attr.Parent = vSelf;
	Component->Ops.SetColor(Component,
		&ListView->Attr.BgColor,
		&ListView->Attr.FgColor
	);

	Component->Ops.SetAlpha(Component,
		ListView->Attr.Alpha);
	Component->Attr.FormBuffer = ListView->Attr.FormBuffer;

	GUI_RECT Rect;
	if (!Auto)
	{
		Rect.x = FrontWidth;
		Rect.y = FrontHeigth;
		Rect.Width = ListView->HeaderWidth[ColumnIndex];
		Rect.Height = ListView->Items[RowIndex].ItemHeight;
		Component->Ops.SetBound(Component, &Rect);
	}
	else
	{
		/*not implement*/
	}

	ListView->Attr.NeedRedraw = TRUE;
	return TRUE;

}

static
UINT16
Gui_ListView_GetSelectedItem(
	IN VOID*		vSelf
) {

	PGUI_LISTVIEW ListView = (PGUI_LISTVIEW)vSelf;

	return ListView->SelectItem;

}

static
VOID
EFIAPI
Gui_ListView_Destory(
	IN VOID* vSelf
) {

	PGUI_LISTVIEW Self = (PGUI_LISTVIEW)vSelf;

	for (UINTN i = 0;i < Self->HeaderCount;i++)
	{
		for (UINTN j = 0;j < Self->ItemCount;j++) {

			PCOMPONENT_COMMON_HEADER CopObj =
				(PCOMPONENT_COMMON_HEADER)Self->Items[j].ItemContent[i];

			if (CopObj->Ops.Destory)
				CopObj->Ops.Destory(CopObj);
		}
	}

	if (Self->Attr.BitmapObject != 0)
		VideoLibDestoryBitmapObject(Self->Attr.BitmapObject);

	FreePool(Self);

}

static
VOID
Gui_ListView_Draw(
	IN VOID*		vSelf,
	IN PGUI_RECT	Rect
) {

	PGUI_LISTVIEW ListView = (PGUI_LISTVIEW)vSelf;

	if (!ListView->Attr.NeedRedraw)
		return;

	/* 遍历所有行列 */

	for (UINTN i = 0;i < ListView->ItemCount;i++)
	{

		for (UINTN j = 0;j < ListView->HeaderCount;j++)
		{

			PCOMPONENT_COMMON_HEADER Component = ListView->Items[i].ItemContent[j];

			EFI_GRAPHICS_OUTPUT_BLT_PIXEL OriBgColor;

			/* 对于选中的项，我们设置背景为亮灰色 */

			if (i == ListView->SelectItem)
			{

				CopyMem(&OriBgColor, &Component->Attr.BgColor, sizeof(OriBgColor));
				Component->Ops.SetColor(Component, &LightGrayColor, NULL);

			}

			Component->Ops.Draw(Component, NULL);

			if (i == ListView->SelectItem)
			{

				Component->Ops.SetColor(Component, &OriBgColor, NULL);

			}

		}

	}

	ListView->Attr.NeedRedraw = FALSE;
	return;

}

PGUI_LISTVIEW
CreateListview(
	IN CHAR16* Name
) {

	PGUI_LISTVIEW ListView = AllocatePool(sizeof(GUI_LISTVIEW));
	SetMem(ListView, sizeof(GUI_LISTVIEW), 0);

	ListView->Ops.GetBound =	Gui_ListView_GetBound;
	ListView->Ops.SetBound =	Gui_ListView_SetBound;
	ListView->Ops.SetFocus =	Gui_Normal_SetFocus;
	ListView->Ops.GetFocus =	Gui_Normal_GetFocus;
	ListView->Ops.SetColor =	Gui_Normal_SetColor;
	ListView->Ops.KeyPress =	Gui_ListView_KeyPress;
	ListView->Ops.SetAlpha =	Gui_ListView_SetAlpha;
	ListView->Ops.Destory =		Gui_ListView_Destory;

	ListView->AddItem = Gui_ListView_AddItem;
	ListView->AddColumnHeader = Gui_ListView_AddColumnHeader;
	ListView->SetItemContent =	Gui_ListView_SetItemContent;
	ListView->SetItemContext =	Gui_ListView_SetItemContext;
	ListView->Ops.Draw =		Gui_ListView_Draw;

	StrCpyS(ListView->Attr.Name, 32, Name);

	ListView->Ops.SetColor(ListView, &BlackColor, &WhiteColor);
	ListView->Attr.Visible = TRUE;
	ListView->Attr.Alpha = 255;
	ListView->Attr.NeedRedraw = TRUE;

	ListView->Type = GUI_TYPE_LISTVIEW;

	return ListView;

}

/* 通过XML生成组件 */

static
PCOMPONENT_COMMON_HEADER
Pharse_Listview_XML(XML_SECTION* Xml, PCOMPONENT_COMMON_HEADER ParentNode)
{

	PGUI_LISTVIEW CurrentNode;

	if (StrCmp(Xml->SectionName, L"Listview") == 0)
	{

		CHAR16* NameStr;

		PKV_PAIR NamePair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Name");
		PKV_PAIR WidthPair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Width");
		PKV_PAIR HeightPair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Height");
		PKV_PAIR LeftPair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Left");
		PKV_PAIR TopPair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Top");
		PKV_PAIR Alpha = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Alpha");

		if (NamePair == NULL)
			NameStr = LwGuiGenerateName(L"Listview");
		else
			NameStr = StrDup((CHAR16*)NamePair->Value);

		CurrentNode = CreateListview(NameStr);
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

		for (UINTN i = 0;i < Xml->ChildrenCount;i++)
		{

			//先遍历添加所有列头
			if (StrCmp(Xml->Children[i]->SectionName, L"ColumnHeaders") == 0)
			{

				XML_SECTION* HeaderSection = Xml->Children[i];
				for (UINTN j = 0;j < HeaderSection->ChildrenCount;j++)
				{

					PKV_PAIR WidthPair = HkFindPairInHashKv(&HeaderSection->Children[j]->AttrHashKV, (BYTE*)L"Width");
					UINT16 Width = 0;
					if (WidthPair != NULL)
						Width = AttrReadValue(CurrentNode->Ops.GetBound(CurrentNode)->Width,
						(CHAR16*)WidthPair->Value);

					CurrentNode->AddColumnHeader(CurrentNode, HeaderSection->Children[j]->SectionContent, Width);

				}
			}

			//添加行项
			if (StrCmp(Xml->Children[i]->SectionName, L"RowItems") == 0)
			{

				XML_SECTION* RowItemSection = Xml->Children[i];
				UINT16 RowIndex = 0;
				//遍历添加所有行
				for (UINTN j = 0;j < RowItemSection->ChildrenCount;j++)
				{

					if (StrCmp(RowItemSection->Children[j]->SectionName, L"Row") != 0)
						continue;

					XML_SECTION* Row = RowItemSection->Children[j];

					PKV_PAIR HeightPair = HkFindPairInHashKv(&Row->AttrHashKV, (BYTE*)L"Height");
					UINT16 Height = 0;
					if (HeightPair != NULL)
						Height = AttrReadValue(CurrentNode->Ops.GetBound(CurrentNode)->Height,
							(CHAR16*)HeightPair->Value);
					CurrentNode->AddItem(CurrentNode, L"Row", Height, NULL, FALSE);

					WCHAR* Context = NULL;
					UINT16 HeaderIndex = 0;

					for (UINTN k = 0;k < Row->ChildrenCount;k++)
					{

						if (StrCmp(Row->Children[k]->SectionName, L"Context") == 0)
							Context = StrDup(Row->Children[k]->SectionContent);
						else
						{

							for (UINTN m = 0;m < TypeListCount;m++)
							{

								PCOMPONENT_COMMON_HEADER Component;
								if (StrCmp(TypeList[m].Name, Row->Children[k]->SectionName) == 0)
								{

									Component = TypeList[m].Pharse_COMPONENT_XML(Row->Children[k], CurrentNode);
									CurrentNode->SetItemContent(CurrentNode, HeaderIndex, RowIndex, (VOID*)Component);
									HeaderIndex++;
								}

							}

						}
					}

					CurrentNode->SetItemContext(CurrentNode, RowIndex, (UINTN*)Context);
					RowIndex++;

				}

			}

		}

		//LwGuiRegisterComponent((PCOMPONENT_COMMON_HEADER)CurrentNode);
		return (PCOMPONENT_COMMON_HEADER)CurrentNode;

	}

	return NULL;

}

//static
VOID
RegisterListview()
{

	LwGuiRegisterComponentType(L"Listview", CreateListview, Pharse_Listview_XML);

}