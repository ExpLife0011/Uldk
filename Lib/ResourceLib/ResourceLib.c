
#include <EdkClass/Common.h>
#include <EdkClass/Image.h>
#include <Lib/ResourceLib.h>
#include <PeStruct.h>

#include "ResourceMeta.h"

EFI_HANDLE
EFIAPI
ResLibLocateResourceSection(
	IN EFI_HANDLE ImageHandle
) {

	EFI_LOADED_IMAGE_PROTOCOL* ImageProt;
	BYTE* ImageBase;
	PIMAGE_DOS_HEADER DosHeader;
	PIMAGE_NT_HEADERS64 NtHeader;
	PIMAGE_SECTION_HEADER SectionHeaders;
	UINT16 i;

	EFI_STATUS Status = gBS->HandleProtocol(
		ImageHandle,
		&gEfiLoadedImageProtocolGuid,
		&ImageProt
	);
	if (EFI_ERROR(Status))
		return NULL;

	ImageBase = (BYTE*)ImageProt->ImageBase;
	DosHeader = (PIMAGE_DOS_HEADER)ImageBase;
	NtHeader = (PIMAGE_NT_HEADERS64)(ImageBase + DosHeader->e_lfanew);

	SectionHeaders = (PIMAGE_SECTION_HEADER)(ImageBase +
		DosHeader->e_lfanew +
		OFFSET_OF(IMAGE_NT_HEADERS64, OptionalHeader) +
		NtHeader->FileHeader.SizeOfOptionalHeader);

	for (i = 0;i < NtHeader->FileHeader.NumberOfSections;i++)
	{
		if (AsciiStrnCmp(SectionHeaders[i].Name, "RESPACK", 7) == 0)
		{
			return (EFI_HANDLE)(ImageBase + SectionHeaders[i].VirtualAddress);
		}
	}

	return NULL;

}

EFI_HANDLE
EFIAPI
ResLibCreateResource(
	IN EFI_HANDLE ImageHandle
) {

	PRESOURCE_INSTANCE ResInst = (PRESOURCE_INSTANCE)AllocateZeroPool(sizeof(*ResInst));
	if (ResInst == NULL)
		return NULL;

	InitializeListHead(&ResInst->FontList);
	InitializeListHead(&ResInst->ImageList);
	InitializeListHead(&ResInst->StringList);

	ResInst->ImageHandle = ImageHandle;

	return(EFI_HANDLE)ResInst;

}