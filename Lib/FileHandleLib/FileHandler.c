
#include <EdkClass/Common.h>
#include <EdkClass/MediaAccess.h>
#include <EdkClass/Helper.h>

#include "HandleSup.h"

/* ��д�ļ�ʱС��1M���ļ�ֱ�Ӷ����ڴ� */

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

/* �����ļ����� */

EFI_STATUS
EFIAPI
File_Create(
	FILE_HANDLE* FileObj,
	UINT64 OpenMode,
	UINT64 Attribute
) {

	EFI_STATUS Status;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FatFs;

	/* ���ļ�ϵͳ */
	Status = gBS->HandleProtocol(FileObj->Device, &gEfiSimpleFileSystemProtocolGuid, &FatFs);
	if (EFI_ERROR(Status))
		return Status;

	EFI_FILE* Root;
	FatFs->OpenVolume(FatFs, &Root);

	/* ���ļ� */
	EFI_FILE* TargetFile;
	Status = Root->Open(Root, &TargetFile, FileObj->RemainPath, OpenMode, Attribute);
	if (EFI_ERROR(Status))
		return Status;

	/* ��ȡ��С�����С��1Mֱ�Ӷ����ڴ� */
	FileObj->File = TargetFile;
	FileHandleGetSize(TargetFile, &FileObj->Size);

	if (FileObj->Size <= 1024 * 1024) {

		FileObj->Buffer = (PBYTE)AllocatePool(FileObj->Size);
		FileHandleRead(TargetFile, &FileObj->Size, FileObj->Buffer);

	}

	/* ���ö�ȡ���� */
	FileObj->Read = File_Read;
	FileObj->Write = File_Write;
	FileObj->Seek = File_Seek;

	return Status;

}
