
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

/* 获得路径节点数量,不包括结束节点数量 */

UINTN
EFIAPI
DhGetDevicePathLength(
	IN EFI_DEVICE_PATH_PROTOCOL* FullPath
);

/* 获得路径中的最后一个节点 */

EFI_DEVICE_PATH_PROTOCOL*
EFIAPI
DhGetLastPathNode(
	IN EFI_DEVICE_PATH_PROTOCOL* FullPath
);

EFI_HANDLE
EFIAPI
DhGetRootDevice(
);