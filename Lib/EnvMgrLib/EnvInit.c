
#include <EdkClass/Common.h>
#include <EdkClass/Helper.h>
#include <EdkClass/MediaAccess.h>
#include <Protocol/LoadedImage.h>
#include <Config.h>
#include <ExtTypes.h>

#include <Lib/EnvMgrLib.h>

/* 判断当前程序是否是从网络启动的 */

BOOLEAN
EFIAPI
EmIsPxeBoot() {

	EFI_LOADED_IMAGE_PROTOCOL* LdrProt;
	gBS->HandleProtocol(gImageHandle, &gEfiLoadedImageProtocolGuid, &LdrProt);

	EFI_DEVICE_PATH_PROTOCOL* TargetPath = DuplicateDevicePath(DevicePathFromHandle(LdrProt->DeviceHandle));
	EFI_DEVICE_PATH_PROTOCOL* CurNode = TargetPath;

	while (!IsDevicePathEnd(CurNode)) {

		if (DevicePathSubType(CurNode) == MSG_MAC_ADDR_DP) {
			FreePool(TargetPath);
			EmSetEnv(L"IsPxeBoot", L"1", 4);
			return TRUE;
		}
		CurNode = NextDevicePathNode(CurNode);

	}

	FreePool(TargetPath);
	return FALSE;

}