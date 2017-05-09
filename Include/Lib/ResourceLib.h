
#include <EdkClass/Common.h>
#include <ExtTypes.h>

//EFI_HANDLE
//EFIAPI
//ResLibLoadResource(
//	IN EFI_HANDLE	ResourceHandle,
//	IN UINT16		ResourceId
//);

PBYTE
EFIAPI
ResLibRetrieveCharMatrix(
	IN EFI_HANDLE	ResourceHandle,
	IN EFI_HANDLE	CharHandle
);

UINT16
EFIAPI
ResLibGetStringWidth(
	IN EFI_HANDLE	ResourceHandle,
	IN CHAR16*		String
);

UINT16
EFIAPI
ResLibGetCharHeight(
	IN EFI_HANDLE	ResourceHandle,
	IN EFI_HANDLE	CharHandle
);

EFI_HANDLE
EFIAPI
ResLibFindCharCode(
	IN EFI_HANDLE	ResourceHandle,
	IN CHAR16		CharCode
);

UINT16
EFIAPI
ResLibGetCharWidth(
	IN EFI_HANDLE	ResourceHandle,
	IN EFI_HANDLE	CharHandle
);

EFI_STATUS
EFIAPI
ResLibLoadFont(
	IN EFI_HANDLE	ResourceHandle,
	IN VOID*		FontResource
);

EFI_HANDLE
EFIAPI
ResLibCreateResource(
	IN EFI_HANDLE ImageHandle
);