
#include <EdkClass/Common.h>
#include <LwGUI/Component.h>
#include <LwGUI/LwGuiLib.h>
#include <Lib/XmlParserLib.h>
#include <Lib/VideoLib.h>
#include <Color.h>

GUI_HANDLE*				GuiHandle = NULL;

COMPONENT_TYPE_LIST		TypeList[128];
UINTN					TypeListCount = 0;

LIST_ENTRY				FormListHead = INITIALIZE_LIST_HEAD_VARIABLE(FormListHead);

/* ��ʼ��LwGUILib */

EFI_STATUS
EFIAPI
LwGuiInitialize(
) {

	EFI_STATUS Status = EFI_SUCCESS;

	GuiHandle = AllocateZeroPool(sizeof(*GuiHandle));

	Status = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, &GuiHandle->GOP);
	if (EFI_ERROR(Status))
	{
		return Status;
	}

	GuiHandle->ScreenRect.x = 0;
	GuiHandle->ScreenRect.y = 0;
	GuiHandle->ScreenRect.Width = GuiHandle->GOP->Mode->Info->HorizontalResolution;
	GuiHandle->ScreenRect.Height = GuiHandle->GOP->Mode->Info->VerticalResolution;

	//gBS->CloseProtocol(gImageHandle, &gEfiGraphicsOutputProtocolGuid, gImageHandle, NULL);

	//RegisterForm();
	//RegisterLabel();
	//RegisterTextBox();
	//RegisterHBox();
	//RegisterListview();

	return EFI_SUCCESS;

}

/* ���������ӳ�䵽��Ļ���� */

EFI_STATUS
EFIAPI
LwGuiGetAbsoluteRect(
	IN PCOMPONENT_COMMON_HEADER Component,
	OUT PGUI_RECT Absolute
) {

	GUI_RECT RRect;

	/* ���Ȼ�ȡ������� */
	PCOMPONENT_COMMON_HEADER Parent = Component->Attr.Parent;
	CopyMem(&RRect,
		Component->Ops.GetBound(Component),
		sizeof(GUI_RECT)
	);

	/* �ݹ�������λ��ֱ���������� */
	do {

		PGUI_RECT PRect = Parent->Ops.GetBound(Parent);
		RRect.x += PRect->x;
		RRect.y += PRect->y;
		Parent = Parent->Attr.Parent;
		if (Parent == NULL)
			break;

	} while (Parent->Type != GUI_TYPE_FORM);

	CopyMem(Absolute, &RRect, sizeof(GUI_RECT));

	return EFI_SUCCESS;

}

/* ����һ����־����һ��������� */

CHAR16*
EFIAPI
LwGuiGenerateName(
	IN CHAR16* BaseName
) {

	static c = 0;
	CHAR16* RName = (CHAR16*)AllocatePool(64);

	//��ôAsmRdRand64������ѽ
	//UINT64 Rand;
	//AsmRdRand64(&Rand);
	//Print(L"456\n");

	UnicodeSPrint(RName, 64, L"%s-%d", BaseName, c++);

	return RName;

}

/* ��ȡ��������Ĵ��� */

PCOMPONENT_COMMON_HEADER
EFIAPI
LwGuiGetComponentForm(
	IN PCOMPONENT_COMMON_HEADER Component
) {

	if (Component->Type == GUI_TYPE_FORM)
		return Component;

	PCOMPONENT_COMMON_HEADER Parent = Component->Attr.Parent;

	while (Parent->Type != GUI_TYPE_FORM)
	{

		Parent = Parent->Attr.Parent;
	}

	return Parent;

}

/* ���� */

EFI_STATUS
EFIAPI
LwGuiClearScreen(
) {

	return GuiHandle->GOP->Blt(
		GuiHandle->GOP,
		&BlackColor,
		EfiBltVideoFill,
		0,
		0,
		0,
		0,
		GuiHandle->ScreenRect.Width,
		GuiHandle->ScreenRect.Height,
		0
	);

}

VOID
EFIAPI
LwGuiRegisterComponentType(
	IN CHAR16*	Name,
	IN VOID*	CreateProc,
	IN VOID*	ParserProc
) {

	TypeList[TypeListCount].Name = StrDup(Name);
	TypeList[TypeListCount].CP_CREATE = CreateProc;
	TypeList[TypeListCount].Pharse_COMPONENT_XML = ParserProc;
	TypeListCount++;

}

CHAR16 *
EFIAPI
StrDup(
	IN      CONST CHAR16              *Source
) {

	UINTN Size = StrLen(Source);
	CHAR16* Dest = AllocatePool((Size + 1) * sizeof(CHAR16));
	StrCpyS(Dest, (Size + 1) * sizeof(CHAR16), Source);
	return Dest;

}