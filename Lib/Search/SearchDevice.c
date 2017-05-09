
#include <EdkClass/Common.h>
#include <EdkClass/MediaAccess.h>
#include <EdkClass/Helper.h>

#include <Lib/SearchLib.h>
#include <Lib/DeviceHelperLib.h>

EFI_HANDLE
EFIAPI
SearchLibGetFirstFileSystem(
) {

	/*
	EFI_HANDLE* Handles, Handle;
	UINTN Count;

	gBS->LocateHandle(ByProtocol, 
		&gEfiSimpleFileSystemProtocolGuid,
		NULL,
		&Count, 
		&Handles
	);

	if (Count == 0)
		return NULL;
	else
	{
		Handle = Handles[0];
		FreePool(Handles);
		return Handle;
	}
	?*/
	return DhRetrieveDevice(L"fs0");

}