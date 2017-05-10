
#include <EdkClass/Common.h>
#include <EdkClass/MediaAccess.h>
#include <EdkClass/Helper.h>

#include "HandleSup.h"

/* 读写文件时小于1M的文件直接读进内存 */

static
UINT32
File_Read(
	EFI_HANDLE FileHandle,
	CHAR8* Buffer,
	UINT32 Length
) {

	EFI_STATUS Status;
	FILE_HANDLE* FileObj = (FILE_HANDLE*)FileHandle;
	UINT64 RealLength;

	if (FileObj->Size <= 1024 * 1024) {	

		if (FileObj->Offset == FileObj->Size)
			return 0;

		if (FileObj->Offset + Length > FileObj->Size)
			RealLength = FileObj->Size - FileObj->Offset;
		else
			RealLength = Length;

		CopyMem(Buffer, FileObj->Buffer, RealLength);
		FileObj->Offset += RealLength;

		return (UINT32)RealLength;

	}
	else {

		EFI_FILE_PROTOCOL* FileProt;
		FileProt = (EFI_FILE_PROTOCOL*)FileObj->File;

		RealLength = Length;
		Status=FileProt->Read(FileProt, &RealLength, Buffer);
		if (EFI_ERROR(Status))
			return 0;

		return (UINT32)RealLength;

	}

}

static
UINT32
File_Write(
	EFI_HANDLE FileHandle,
	CHAR8* Buffer,
	UINT32 Length
) {

	EFI_STATUS Status;
	FILE_HANDLE* FileObj = (FILE_HANDLE*)FileHandle;
	UINT64 RealLength;

	if (FileObj->Size <= 1024 * 1024) {

		if (FileObj->Offset == FileObj->Size)
			return 0;

		if (FileObj->Offset + Length > FileObj->Size)
			RealLength = FileObj->Size - FileObj->Offset;
		else
			RealLength = Length;

		CopyMem(FileObj->Buffer, Buffer, RealLength);
		FileObj->Offset += RealLength;

		return (UINT32)RealLength;

	}
	else {

		EFI_FILE_PROTOCOL* FileProt;
		FileProt = (EFI_FILE_PROTOCOL*)FileObj->File;

		RealLength = Length;
		Status = FileProt->Write(FileProt, &RealLength, Buffer);
		if (EFI_ERROR(Status))
			return 0;

		return (UINT32)RealLength;

	}

}

static
UINT64
File_Seek(
	EFI_HANDLE FileHandle,
	UINT64 Offset
) {

	FILE_HANDLE* FileObj = (FILE_HANDLE*)FileHandle;
	UINT64 OldOffset;

	if (FileObj->Size <= 1024 * 1024) {

		OldOffset = FileObj->Offset;
		FileObj->Offset = Offset;
		
		return OldOffset;
	
	}
	else {

		EFI_FILE_PROTOCOL* FileProt;
		FileProt = (EFI_FILE_PROTOCOL*)FileObj->File;
		FileProt->GetPosition(FileProt, &OldOffset);
		FileProt->SetPosition(FileProt, Offset);
	
		return OldOffset;

	}

}

/* 创建文件对象 */

EFI_STATUS
EFIAPI
File_Create(
	FILE_HANDLE* FileObj,
	UINT64 OpenMode,
	UINT64 Attribute
) {

	EFI_STATUS Status;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FatFs;

	/* 打开文件系统 */
	Status = gBS->HandleProtocol(FileObj->Device, &gEfiSimpleFileSystemProtocolGuid, &FatFs);
	if (EFI_ERROR(Status))
		return Status;

	EFI_FILE* Root;
	FatFs->OpenVolume(FatFs, &Root);

	/* 打开文件 */
	EFI_FILE* TargetFile;
	Status = Root->Open(Root, &TargetFile, FileObj->RemainPath, OpenMode, Attribute);
	if (EFI_ERROR(Status))
		return Status;

	/* 获取大小，如果小于1M直接读到内存 */
	FileObj->File = TargetFile;
	FileHandleGetSize(TargetFile, &FileObj->Size);

	if (FileObj->Size <= 1024 * 1024) {

		FileObj->Buffer = (PBYTE)AllocatePool(FileObj->Size);
		FileHandleRead(TargetFile, &FileObj->Size, FileObj->Buffer);

	}

	/* 设置读取函数 */
	FileObj->Read = File_Read;
	FileObj->Write = File_Write;
	FileObj->Seek = File_Seek;

	return Status;

}
