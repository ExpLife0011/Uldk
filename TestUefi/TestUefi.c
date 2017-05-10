
#include <EdkClass/Common.h>

#include <EdkClass/MediaAccess.h>
#include <Lib/DeviceHelperLib.h>
#include <Lib/EnvMgrLib.h>

#include <Lib/FileLib.h>
#include <Lib/HashKvLib.h>
#include <Lib/XmlParserLib.h>

#include <Lib/ImageDecoderLib.h>
#include <Lib/ResourceLib.h>
#include <Lib/VideoLib.h>

#include <LwGui/LwGuiLib.h>
#include <LwGui/Form.h>
#include <LwGui/Component.h>
#include <LwGui/Label.h>

EFI_STATUS
TestMain(
	IN EFI_HANDLE			ImageHandle,
	IN EFI_SYSTEM_TABLE*	SystemTable
) {

	//Print(L"cccccc\n");
	DhInitDeviceHelperLib();
	
	EmInit();

	EFI_HANDLE GG = FileLibCreateFile(L"(tftp)/Font.bin", EFI_FILE_MODE_READ, 0);

	UINT64 Size = FileLibGetFileSize(GG);
	CHAR16* Buffer = (CHAR16*)AllocatePool(Size);
	FileLibReadFile(GG, Buffer, (UINT32)Size);

	Print(L"sss:%d\n", Size);

	//XML_SECTION* root;
	//XpReadSection(Buffer, &root, NULL);

	//XML_SECTION* Node = XpFindFirstNodeByName(root, L"Label");

	//Print(L"ddd:%s\n", Node->SectionName);

	//ReadPng(NULL, NULL, 0);

	//LwGuiInitialize();

	EFI_HANDLE ResHandle = ResLibCreateResource(ImageHandle);
	ResLibLoadFont(ResHandle, Buffer);

	VideoLibSetResourceHandle(ResHandle);
	Print(L"%d\n", VideoLibGetStringWidth(L"addfsmd"));

	LwGuiInitialize();

	GG = FileLibCreateFile(L"config.xml", EFI_FILE_MODE_READ, 0);

	Size = FileLibGetFileSize(GG);
	Buffer = (CHAR16*)AllocatePool(Size);
	FileLibReadFile(GG, Buffer, (UINT32)Size);

	PXML_SECTION FormXML;
	XpReadSection(Buffer, &FormXML, NULL);

	PGUI_FORM Form = CreateFormFromXML(FormXML);

	//Form->Ops.SetBackground(Form, L"Background.png");

	//PGUI_LABEL Label = CreateLabel(L"uuu");
	//GUI_RECT Rect = { 10,10,100,40 };
	//Label->Ops.SetBound(Label, &Rect);
	//Label->SetText(Label, L"dddd");

	//Form->AddChild(Form, (PCOMPONENT_COMMON_HEADER)Label);

	Form->Ops.Draw(Form, NULL);


	return EFI_SUCCESS;

}