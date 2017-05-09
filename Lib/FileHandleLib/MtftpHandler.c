
#include <EdkClass/Common.h>
#include <EdkClass/MediaAccess.h>
#include <EdkClass/NetProtocolStack.h>
#include <Lib/MtftpLib.h>

#include "HandleSup.h"

UINT32
Mtftp_Read(
	EFI_HANDLE FileHandle,
	CHAR8* Buffer,
	UINT32 Length
) {

	FILE_HANDLE* FileObj = (FILE_HANDLE*)FileHandle;
	UINT64 RealLength;

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

UINT32
Mtftp_Write(
	EFI_HANDLE FileHandle,
	CHAR8* Buffer,
	UINT32 Length
) {

	FILE_HANDLE* FileObj = (FILE_HANDLE*)FileHandle;
	UINT64 RealLength;

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

UINT64
Mtftp_Seek(
	EFI_HANDLE FileHandle,
	UINT64 Offset
	) {

	FILE_HANDLE* FileObj = (FILE_HANDLE*)FileHandle;
	UINT64 OldOffset = FileObj->Offset;

	FileObj->Offset = Offset;
	return OldOffset;

}

/* 创建文件对象 */

EFI_STATUS
EFIAPI
Mtftp_Create(
	FILE_HANDLE* FileObj
) {

	EFI_STATUS Status;

	CHAR8 ascFileName[128];
	UnicodeStrToAsciiStrS(FileObj->RemainPath, ascFileName, 128);
	
	Status = MtftpLibDownloadFile(FileObj->Device, ascFileName, (CHAR8**)&FileObj->Buffer, &FileObj->Size);
	if (EFI_ERROR(Status))
		return Status;

	/* 设置读取函数 */
	FileObj->Read = Mtftp_Read;
	FileObj->Write = Mtftp_Read;
	FileObj->Seek = Mtftp_Seek;

	return EFI_SUCCESS;

}
