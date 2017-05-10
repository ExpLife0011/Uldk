
#include <EdkClass/Common.h>
#include <LwGui/LwGuiLib.h>
#include <LwGUI/Component.h>
#include <LwGUI/Form.h>

#include <Lib/XmlParserLib.h>
#include <Lib/VideoLib.h>

extern GUI_HANDLE*				GuiHandle;

extern COMPONENT_TYPE_LIST		TypeList[128];
extern UINTN					TypeListCount;

extern LIST_ENTRY				FormListHead;

//�ػ�����ص�
//������������ϴ�������¼��ַ���������Լ������ػ�
//ֻ���ڷ�������ƶ�ʱ�������ػ�

BOOLEAN
RedrawAll_IterChildCallback(
	VOID* Parent,
	VOID* Child
) {

	PGUI_FORM Form = (PGUI_FORM)Parent;

	/* ������Լ����� */
	PCOMPONENT_COMMON_HEADER Component = (PCOMPONENT_COMMON_HEADER)Child;

	//����ɼ�����ȷʵ��Ҫ�ػ�
	if ((Component->Attr.Visible) &&
		(Component->Attr.NeedRedraw))
	{

		Component->Ops.Draw(Component, NULL);
		Form->Attr.NeedRedraw = FALSE;

	}

	return TRUE;

}

BOOLEAN
Destory_IterChildCallback(
	VOID* Parent,
	VOID* Child
) {

	PGUI_FORM Form = (PGUI_FORM)Parent;

	//������Լ�����
	PCOMPONENT_COMMON_HEADER Component = (PCOMPONENT_COMMON_HEADER)Child;
	//Print(L"%s\n", Component->Attr.Name);

	if (Component->Ops.Destory)
		Component->Ops.Destory(Component);

	return TRUE;

}

/* ���ý�������ص� */

BOOLEAN
SetFocus_IterChildCallback(
	VOID* Parent,
	VOID* Child
) {

	PGUI_FORM Form = (PGUI_FORM)Parent;

	//������Լ�����
	PCOMPONENT_COMMON_HEADER Component = (PCOMPONENT_COMMON_HEADER)Child;

	if (Component->Ops.SetFocus(Component, TRUE))
	{
		Form->CurrentFocus = Component;
		return FALSE;
	}

	return TRUE;

}

/* Form��Ȼռ��������Ļ */

static
VOID
EFIAPI
Gui_Form_SetBound(
	IN VOID* vSelf,
	IN PGUI_RECT Rect
) {

	return;

}

static
PGUI_RECT
EFIAPI
Gui_Form_GetBound(
	IN VOID* vSelf
) {

	PGUI_FORM Self = (PGUI_FORM)vSelf;
	return &Self->Attr.Rect;

}

/* ���һ������� */

VOID
EFIAPI
Gui_Form_AddChild(
	IN VOID*	vSelf,
	IN PCOMPONENT_COMMON_HEADER	Component
) {

	PGUI_FORM Self = (PGUI_FORM)vSelf;

	InsertTailList(&Self->ChildrenList, &Component->Attr.ChildList);
	Self->ChildrenCount++;

	Component->Attr.Parent = vSelf;
	
	/* ���û��ƶ���Ϊ���彻������ */
	Component->Attr.FormBuffer = Self->SwapObject;

	/* û�н������������ý��� */
	if (Self->CurrentFocus == NULL)
	{

		if (Component->Ops.SetFocus(Component, TRUE))
			Self->CurrentFocus = Component;
	
	}

	return;

}

/* ɾ��һ������� */

VOID
EFIAPI
Gui_Form_DeleteChild(
	IN VOID* vSelf,
	IN PCOMPONENT_COMMON_HEADER	Component
) {

	PGUI_FORM Form = (PGUI_FORM)vSelf;
	PGUI_RECT Rect = Form->Ops.GetBound(Form);
	EFI_HANDLE Slice = VideoLibCreateBitmapObject(
		BITMAP_FORMAT_RGB,
		Rect->Width,
		Rect->Height
	);

	Component->Attr.ChildList.BackLink->ForwardLink = Component->Attr.ChildList.ForwardLink;
	Component->Attr.ChildList.ForwardLink->BackLink = Component->Attr.ChildList.BackLink;
	Component->Ops.Destory(Component);

	return;

}

/* ö����� */

VOID
EFIAPI
Gui_Form_IterChildren(
	IN	VOID*				vSelf,
	IN	IterChildCallback	IterCallback
) {

	PGUI_FORM Self = (PGUI_FORM)vSelf;

	if ((IterCallback == NULL) || (Self->ChildrenCount == 0))
		return;

	PCOMPONENT_COMMON_HEADER FirstChild = BASE_CR(Self->ChildrenList.ForwardLink, COMPONENT_COMMON_HEADER, Attr.ChildList);
	do {

		/* ����û�����false��ֹͣö�� */
		if (!IterCallback(vSelf, FirstChild))
			break;

		FirstChild = BASE_CR(FirstChild->Attr.ChildList.ForwardLink, COMPONENT_COMMON_HEADER, Attr.ChildList);

	} while (&FirstChild->Attr.ChildList != &Self->ChildrenList);

}

/* �����¼��ص� */

static
VOID
EFIAPI
Gui_Form_KeyPress(
	IN		VOID* vSelf,
	IN		EFI_INPUT_KEY* Key
) {

	PGUI_FORM Self = (PGUI_FORM)vSelf;

	//�����û��¼��ص�
	//if (Self->OnKeyPress)
	//	if (Self->OnKeyPress(vSelf, Key))
	//		return;

	//û�н����������ý���
	if (Self->CurrentFocus == NULL)
		Self->IterChildren(Self, SetFocus_IterChildCallback);
	if (Self->CurrentFocus == NULL)
		return;

	/* TAB�л����㣬�������ǰ���㸳ֵ */
	if (Key->UnicodeChar == CHAR_TAB)
	{

		PCOMPONENT_COMMON_HEADER Current = (PCOMPONENT_COMMON_HEADER)Self->CurrentFocus;
		PCOMPONENT_COMMON_HEADER FirstChild;

		/* �ҵ����ʵ���һ���ڵ� */

		if (Current->Attr.ChildList.ForwardLink == &Self->ChildrenList)
			FirstChild = BASE_CR(Self->ChildrenList.ForwardLink,
				COMPONENT_COMMON_HEADER,
				Attr.ChildList);
		else
			FirstChild = BASE_CR(Current->Attr.ChildList.ForwardLink,
				COMPONENT_COMMON_HEADER,
				Attr.ChildList);

		do
		{

			if (FirstChild->Ops.SetFocus(FirstChild, TRUE))
			{
				Self->CurrentFocus->Ops.SetFocus(Self->CurrentFocus, FALSE);
				Self->CurrentFocus = FirstChild;
				break;
			}

			if (FirstChild->Attr.ChildList.ForwardLink == &Self->ChildrenList)
				FirstChild = BASE_CR(Self->ChildrenList.ForwardLink,
					COMPONENT_COMMON_HEADER,
					Attr.ChildList);
			else
				FirstChild = BASE_CR(FirstChild->Attr.ChildList.ForwardLink,
					COMPONENT_COMMON_HEADER,
					Attr.ChildList);

		} while (&FirstChild->Attr.ChildList != (LIST_ENTRY*)Self->CurrentFocus);

	}
	else
	{

		if (Self->CurrentFocus->Ops.KeyPress)
			Self->CurrentFocus->Ops.KeyPress(
				Self->CurrentFocus,
				Key
			);

	}

	return;

}

static
PCOMPONENT_COMMON_HEADER
EFIAPI
Gui_Form_NextForm(
	IN		VOID* vSelf,
	IN		CHAR16* FormName
) {

	for (UINTN i = 0;i < 8;i++)
	{

		if (StrCmp(((COMPONENT_COMMON_HEADER*)GuiHandle->FormArray[i])->Attr.Name, FormName) == 0)
		{

			LwGuiClearScreen();
			PCOMPONENT_COMMON_HEADER ActiveForm = GuiHandle->FormArray[i];
			ActiveForm->Ops.Draw(ActiveForm, NULL);
			GuiHandle->ActiveForm = ActiveForm;
			return ActiveForm;

		}

	}

	return NULL;

}

static
VOID
EFIAPI
Gui_Form_Destory(
	IN VOID* vSelf
) {

	PGUI_FORM Self = (PGUI_FORM)vSelf;

	Self->IterChildren(vSelf, Destory_IterChildCallback);

	if (Self->Attr.BitmapObject != 0)
		VideoLibDestoryBitmapObject(Self->Attr.BitmapObject);

	if (Self->SwapObject != 0)
		VideoLibDestoryBitmapObject(Self->SwapObject);

	FreePool(Self);

}

VOID
EFIAPI
Gui_Form_SetBackground(
	VOID* vSelf,
	CHAR16* FileName
) {

	PGUI_FORM Self = (PGUI_FORM)vSelf;

	if (Self->Attr.BitmapObject != NULL)
		VideoLibDestoryBitmapObject(Self->Attr.BitmapObject);

	Self->Attr.BitmapObject = VideoLibCreateImage(
		FileName,
		Self->Attr.Rect.Width,
		Self->Attr.Rect.Height
	);

	if (Self->Attr.BitmapObject != NULL) {
		VideoLibBitblt(Self->Attr.BitmapObject,
			Self->SwapObject,
			0, 0, 0, 0,
			VideoLibGetImageWidth(Self->Attr.BitmapObject),
			VideoLibGetImageHeight(Self->Attr.BitmapObject)
		);
	}

}

static
VOID
EFIAPI
Gui_Form_SetColor(
	IN		VOID* vSelf,
	IN		EFI_GRAPHICS_OUTPUT_BLT_PIXEL* BgColor,
	IN		EFI_GRAPHICS_OUTPUT_BLT_PIXEL* FgColor
) {

	PGUI_FORM Self = (PGUI_FORM)vSelf;

	CopyMem(&Self->BgColor, BgColor, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
	
	if (Self->Attr.BitmapObject != NULL)
		VideoLibDestoryBitmapObject(Self->Attr.BitmapObject);

	Self->Attr.BitmapObject = VideoLibCreateBitmapObject(
		BITMAP_FORMAT_RGB,
		Self->Attr.Rect.Width,
		Self->Attr.Rect.Height
	);

	if (Self->Attr.BitmapObject != NULL) {
		VideoLibFillRect(
			Self->Attr.BitmapObject,
			0, 0,
			Self->Attr.Rect.Width,
			Self->Attr.Rect.Height,
			*((UINT32*)&Self->Attr.BgColor)
		);
	}

}

/* �ڴ����в������ */

PCOMPONENT_COMMON_HEADER
EFIAPI
Gui_Form_FindComponent(
	IN VOID*		vSelf,
	IN CHAR16*		Name
) {


	PGUI_FORM Self = (PGUI_FORM)vSelf;
	if ((Self->ChildrenCount == 0))
		return NULL;

	PCOMPONENT_COMMON_HEADER FirstChild = BASE_CR(Self->ChildrenList.ForwardLink, COMPONENT_COMMON_HEADER, Attr.ChildList);
	do {

		/* ����û�����false��ֹͣö�� */
		if (StrCmp(Name, FirstChild->Attr.Name) == 0)
			return FirstChild;

		FirstChild = BASE_CR(FirstChild->Attr.ChildList.ForwardLink, COMPONENT_COMMON_HEADER, Attr.ChildList);

	} while (&FirstChild->Attr.ChildList != &Self->ChildrenList);

	return NULL;

}

static
VOID
EFIAPI
Gui_Form_Draw(
	IN		VOID* vSelf,
	IN		PGUI_RECT Rect
) {

	PGUI_FORM Self = (PGUI_FORM)vSelf;
	PVIDEO_BITMAP BitmapObject = (PVIDEO_BITMAP)Self->SwapObject;

	//if (Self->Attr.BitmapObject != NULL) {
	//	VideoLibBitblt(Self->Attr.BitmapObject,
	//		Self->SwapObject,
	//		0, 0, 0, 0,
	//		VideoLibGetImageWidth(Self->Attr.BitmapObject),
	//		VideoLibGetImageHeight(Self->Attr.BitmapObject)
	//	);
	//}

	//�ȳ����ػ�������Ԫ��

	Self->IterChildren(vSelf, RedrawAll_IterChildCallback);

	GuiHandle->GOP->Blt(
		GuiHandle->GOP,
		BitmapObject->BitmapData,
		EfiBltBufferToVideo,
		0,
		0,
		0,
		0,
		BitmapObject->Width,
		BitmapObject->Height,
		BitmapObject->Width * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
	);

	Self->Attr.NeedRedraw = FALSE;

}

PGUI_FORM
EFIAPI
CreateForm(
	IN CHAR16* Name
)
{

	PGUI_FORM Form = AllocatePool(sizeof(GUI_FORM));
	SetMem(Form, sizeof(GUI_FORM), 0);

	Form->Ops.GetBound =		Gui_Form_GetBound;
	Form->Ops.SetBound =		Gui_Form_SetBound;
	Form->Ops.Draw =			Gui_Form_Draw;
	Form->Ops.KeyPress =		Gui_Form_KeyPress;
	Form->Ops.SetBackground =	Gui_Form_SetBackground;

	Form->SetColor = Gui_Form_SetColor;
	Form->NextForm = Gui_Form_NextForm;
	Form->AddChild = Gui_Form_AddChild;
	Form->DeleteChild = Gui_Form_DeleteChild;
	Form->IterChildren = Gui_Form_IterChildren;
	Form->FindComponent = Gui_Form_FindComponent;

	Form->Attr.Visible = TRUE;
	Form->Type = GUI_TYPE_FORM;

	/* ����ֱ��ռ��ȫ�� */

	GUI_RECT Rect;
	Rect.x = 0;
	Rect.y = 0;
	Rect.Width = GuiHandle->ScreenRect.Width;
	Rect.Height = GuiHandle->ScreenRect.Height;
	CopyMem(&Form->Attr.Rect, &Rect, sizeof(GUI_RECT));
	
	Form->SwapObject = VideoLibCreateBitmapObject(
		BITMAP_FORMAT_RGB,
		Rect.Width,
		Rect.Height
	);
	Form->Attr.FormBuffer = Form->Attr.BitmapObject;

	StrCpyS(Form->Attr.Name, 32, Name);
	/*
	Form Don't hold its own output buffer
	Form->Attr.BitmapBuffer.Width = Rect.Width;
	Form->Attr.BitmapBuffer.Height = Rect.Height;
	Form->Attr.BitmapBuffer.Image.Bitmap = GuiInitializePixelBuffer(&Rect);
	*/

	InitializeListHead(&Form->ChildrenList);

	Form->Attr.FormList.BackLink = &FormListHead;
	Form->Attr.FormList.ForwardLink = FormListHead.ForwardLink;
	FormListHead.ForwardLink = &Form->Attr.FormList;
	Form->Timeout = -1;

	return Form;

}


static
PCOMPONENT_COMMON_HEADER
Pharse_Form_XML(XML_SECTION* Xml, PCOMPONENT_COMMON_HEADER ParentNode)
{

	PGUI_FORM CurrentNode;

	if (StrCmp(Xml->SectionName, L"Form") == 0)
	{

		PKV_PAIR NamePair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Name");
		PKV_PAIR BgPair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Background");
		PKV_PAIR ToPair = HkFindPairInHashKv(&Xml->AttrHashKV, (BYTE*)L"Timeout");

		if (NamePair == NULL)
			return NULL;

		CurrentNode = CreateForm((CHAR16*)NamePair->Value);
		if (CurrentNode == NULL)
			return NULL;

		/* ���ñ������� */
		if (BgPair)
			CurrentNode->Ops.SetBackground(CurrentNode, (CHAR16*)BgPair->Value);

		/* ���ó�ʱ���� */
		if (ToPair)
			CurrentNode->Timeout = StrDecimalToUintn((CHAR16*)ToPair->Value);

		for (UINTN i = 0;i < Xml->ChildrenCount;i++)
		{

			XML_SECTION* Child = Xml->Children[i];

			/* ���洢������Ϣ��δʶ��ڵ���� */

			for (UINTN j = 0;j < TypeListCount;j++)
			{

				if (StrCmp(TypeList[j].Name, Child->SectionName) == 0)
				{

					PCOMPONENT_COMMON_HEADER ChildNode = TypeList[j].Pharse_COMPONENT_XML(Child, CurrentNode);
					CurrentNode->AddChild(CurrentNode, ChildNode);

					break;

				}
			}

		}

		return (PCOMPONENT_COMMON_HEADER)CurrentNode;

	}

	return NULL;

}

PGUI_FORM
EFIAPI
CreateFormFromXML(
	IN XML_SECTION* XML
)
{

	return (PGUI_FORM)Pharse_Form_XML(XML, NULL);

}

//static
VOID
RegisterForm()
{

	LwGuiRegisterComponentType(L"Form", CreateForm, Pharse_Form_XML);

}