
#include <EdkClass/Common.h>
#include <EdkClass/Video.h>

BOOLEAN
ReadPng(EFI_IMAGE_OUTPUT* Output,
	const CHAR8* filebuf, UINT64 filesize
);

UINT16
EFIAPI
GetBitmapWidth(
	IN EFI_HANDLE bitmap
);

UINT16
EFIAPI
GetBitmapHeight(
	IN EFI_HANDLE bitmap
);

UINT16
EFIAPI
GetBitmapDepth(
	IN EFI_HANDLE bitmap
);

VOID*
EFIAPI
GetBitmapData(
	IN EFI_HANDLE bitmap
);