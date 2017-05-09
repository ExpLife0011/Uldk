
#include <EdkClass/Common.h>
#include <EdkClass/MediaAccess.h>
#include <Lib/DeviceHelperLib.h>
#include <Lib/FileLib.h>

EFI_HANDLE
EFIAPI 
SearchLibSearchFile(
	IN CHAR16* FileName
) {

	CHAR16 FsName[100];
	UINTN i = 0;

	EFI_HANDLE Handle = NULL;
	for (i = 0;i < 10;i++) {

		UnicodeSPrint(FsName, 4, L"(fs%d)%s", i, FileName);

		Handle = FileLibCreateFile(FsName, EFI_FILE_MODE_READ, 0);
		if (Handle != NULL) {
			FileLibCloseFile(Handle);
			UnicodeSPrint(FsName, 4, L"fs%d", i);
			return DhRetrieveDevice(FsName);
		}

	}

	return NULL;

}