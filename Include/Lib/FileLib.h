#pragma once

#include <EdkClass/Common.h>
#include <EdkClass/MediaAccess.h>
#include <EdkClass/NetProtocolStack.h>

UINT32
EFIAPI
FileLibReadFile(
	IN EFI_HANDLE File,
	IN VOID* Buffer,
	IN UINT32 Size
);

UINT32
EFIAPI
FileLibWriteFile(
	IN EFI_HANDLE File,
	IN VOID* Buffer,
	IN UINT32 Size
);

UINT64
EFIAPI
FileLibGetFileSize(
	IN EFI_HANDLE File
);

UINT64
EFIAPI
FileLibGetFilePos(
	IN EFI_HANDLE File
);

UINT64
EFIAPI
FileLibSetFilePos(
	IN EFI_HANDLE File,
	IN UINT64 NewPos
);

EFI_STATUS
EFIAPI
FileLibDeleteIfExist(
	IN CHAR16* PathName
);

EFI_HANDLE
EFIAPI
FileLibCreateFile(
	IN CHAR16* PathName,
	IN UINT64 OpenMode,
	IN UINT64 Attribute
);

VOID
EFIAPI
FileLibCloseFile(
	IN EFI_HANDLE FileHandle
);