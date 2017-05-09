
#include <Config.h>
#include <EdkClass/Common.h>
#include <EdkClass/MediaAccess.h>
#include <ExtTypes.h>

#include <Lib/DeviceHelperLib.h>
#include <Lib/MtftpLib.h>
#include <Lib/FileLib.h>
#include <Lib/EnvMgrLib.h>
#include <Lib/SearchLib.h>

#include "HandleSup.h"

static
CHAR16 *
StrDup(
	IN      CONST CHAR16              *Source
) {

	UINTN Size = StrLen(Source);
	CHAR16* Dest = AllocatePool((Size + 1) * sizeof(CHAR16));
	StrCpyS(Dest, (Size + 1) * sizeof(CHAR16), Source);
	return Dest;

}

/* 获得路径开头的符号名 */

static
CHAR16*
GetSymbolName(
	IN CHAR16* PathName
) {

	if (PathName[0] != L'(')
		return NULL;

	CHAR16* p = &PathName[1];
	CHAR16* Buf = (CHAR16*)AllocateZeroPool(64);

	while (*p != L')') {
		Buf[p - &PathName[1]] = *p;
		p++;
	}
	Buf[p - &PathName[1]] = 0;

	return Buf;

}

static
EFI_STATUS
PathNameParserToHandle(
	IN FILE_HANDLE* FileHandle,
	IN CHAR16* PathName
) {

	EFI_STATUS Status = EFI_SUCCESS;
	CHAR16* SymbolName = NULL;
	CHAR16* RemainPath = NULL;

	/* 没有设备符号名，则尝试选择一个 */
	if (PathName[0] != L'(')
	{

		FileHandle->RemainPath = StrDup(PathName);

		if (EmIsPxeBoot()) {
			
			FileHandle->Device = MtftpLibCreateInstance(1024);
			if (FileHandle->Device == NULL)
				EXIT_WITH_STATUS(EFI_NO_MEDIA);
			FileHandle->FileType = TFTP;

		}
		else {

			FileHandle->Device = SearchLibGetFirstFileSystem();
			if (FileHandle->Device == NULL)
				EXIT_WITH_STATUS(EFI_NO_MEDIA);
			FileHandle->FileType = FILE;

		}
	
	}
	else {

		/* 获取设备符号名和路径名 */
		SymbolName = GetSymbolName(PathName);

		RemainPath = AllocateZeroPool((StrLen(PathName) - StrLen(SymbolName) - 2 + 1) * 2);
		CopyMem(RemainPath, &PathName[StrLen(SymbolName) + 2], (StrLen(PathName) - StrLen(SymbolName) - 2) * 2);
		FileHandle->RemainPath = StrDup(RemainPath);

		EFI_HANDLE HddDevice;
		if (StrCmp(SymbolName, L"tftp") == 0) {

			FileHandle->Device = MtftpLibCreateInstance(1024);
			if (FileHandle->Device == NULL)
				EXIT_WITH_STATUS(EFI_NO_MEDIA);

			FileHandle->FileType = TFTP;
			EXIT_WITH_STATUS(EFI_SUCCESS);

		}
		else if ((HddDevice = DhRetrieveDevice(SymbolName)) != NULL) {

			FileHandle->Device = HddDevice;
			FileHandle->FileType = FILE;

			EXIT_WITH_STATUS(EFI_SUCCESS);

		}
		else {

			EXIT_WITH_STATUS(EFI_UNSUPPORTED);

		}

	}

EXIT:

	if(SymbolName!=NULL)
		FreePool(SymbolName);
	
	if(RemainPath!=NULL)
		FreePool(RemainPath);

	return Status;

}

static
EFI_STATUS
OpenFileInHandle(
	IN FILE_HANDLE* FileObj
) {

	EFI_STATUS Status = EFI_SUCCESS;

	if (FileObj->Device == NULL)
		return EFI_NO_MEDIA;

	switch (FileObj->FileType)
	{
	case TFTP:

		Status = MtftpLibConfigure(FileObj->Device, NULL, TRUE);
		if (EFI_ERROR(Status)) {
			return Status;
		}

		Status = Mtftp_Create(FileObj);
		if (EFI_ERROR(Status)) {
			return Status;
		}
		break;
		
	case FILE:

		Status = File_Create(FileObj, FileObj->OpenMode, FileObj->Attribute);
		if (EFI_ERROR(Status)) {
			return Status;
		}
		break;

	default:

		return EFI_UNSUPPORTED;
		break;
	}

	return Status;

}

static
VOID 
DestoryFileObject(
	IN FILE_HANDLE* FileObj
) {

	if (FileObj->Buffer) {
		FreePool(FileObj->Buffer);
		FileObj->Buffer = NULL;
	}

	if (FileObj->RemainPath) {
		FreePool(FileObj->RemainPath);
		FileObj->RemainPath = NULL;
	}

	if (FileObj->Device != NULL) {

		if (FileObj->FileType == TFTP) {
			MtftpLibDestoryInstance(FileObj->Device);
		}
		else if ((FileObj->FileType == FILE) &&
			(FileObj->File != NULL)) {
			EFI_FILE_PROTOCOL* FileProt = (EFI_FILE_PROTOCOL*)FileObj->File;
			FileProt->Close(FileProt);
			FileObj->File = NULL;
		}

		FileObj->Device = NULL;

	}

}

/* 打开文件或设备 */

EFI_HANDLE
EFIAPI
FileLibCreateFile(
	IN CHAR16* PathName,
	IN UINT64 OpenMode,
	IN UINT64 Attribute
) {

	if ((PathName == NULL) ||
		StrLen(PathName) == 0)
		return NULL;

	FILE_HANDLE* FileObj = (FILE_HANDLE*)AllocateZeroPool(sizeof(*FileObj));
	FileObj->Attribute = Attribute;
	FileObj->OpenMode = OpenMode;

	EFI_STATUS Status = PathNameParserToHandle((EFI_HANDLE)FileObj, PathName);

	if (EFI_ERROR(Status)) {
		DestoryFileObject(FileObj);
		return NULL;
	}

	Status = OpenFileInHandle(FileObj);
	if (EFI_ERROR(Status)) {
		DestoryFileObject(FileObj);
		return NULL;
	}

	return (EFI_HANDLE)FileObj;

}

UINT32
EFIAPI
FileLibWriteFile(
	IN EFI_HANDLE File,
	IN VOID* Buffer,
	IN UINT32 Size
) {

	FILE_HANDLE* FileObj = (FILE_HANDLE*)File;
	return FileObj->Write(File, Buffer, Size);

}

UINT64
EFIAPI
FileLibGetFileSize(
	IN EFI_HANDLE File
) {

	FILE_HANDLE* FileObj = (FILE_HANDLE*)File;
	return FileObj->Size;

}

UINT64
EFIAPI
FileLibGetFilePos(
	IN EFI_HANDLE File
) {

	FILE_HANDLE* FileObj = (FILE_HANDLE*)File;
	return FileObj->Offset;

}

UINT64
EFIAPI
FileLibSetFilePos(
	IN EFI_HANDLE File,
	IN UINT64 NewPos
) {

	FILE_HANDLE* FileObj = (FILE_HANDLE*)File;
	return FileObj->Seek(File, NewPos);

}

UINT32
EFIAPI
FileLibReadFile(
	IN EFI_HANDLE File,
	IN VOID* Buffer,
	IN UINT32 Size
) {

	FILE_HANDLE* FileObj = (FILE_HANDLE*)File;
	return FileObj->Read(File, Buffer, Size);

}


VOID
EFIAPI
FileLibCloseFile(
	IN EFI_HANDLE FileHandle
) {

	FILE_HANDLE* FileObj = (FILE_HANDLE*)FileHandle;

	DestoryFileObject(FileHandle);
	FreePool(FileObj);

	return;

}