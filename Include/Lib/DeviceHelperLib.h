
#include <EdkClass/Common.h>
#include <EdkClass/Helper.h>
#include <EdkClass/MediaAccess.h>
#include <ExtTypes.h>

VOID
EFIAPI
DhInitDeviceHelperLib(
);

EFI_HANDLE
EFIAPI
DhRetrieveDevice(
	IN CHAR16* SymbolName
);

/* ���·���ڵ�����,�����������ڵ����� */

UINTN
EFIAPI
DhGetDevicePathLength(
	IN EFI_DEVICE_PATH_PROTOCOL* FullPath
);

/* ���·���е����һ���ڵ� */

EFI_DEVICE_PATH_PROTOCOL*
EFIAPI
DhGetLastPathNode(
	IN EFI_DEVICE_PATH_PROTOCOL* FullPath
);

EFI_HANDLE
EFIAPI
DhGetRootDevice(
);