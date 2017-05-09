#pragma once

#include <Config.h>
#include <ExtTypes.h>

#include <EdkClass/Common.h>
#include <EdkClass/Helper.h>
#include <EdkClass/MediaAccess.h>

typedef
UINT32
(*_FILE_READ_OP)(
	EFI_HANDLE FileHandle,
	CHAR8* Buffer,
	UINT32 Length
	);

typedef
UINT32
(*_FILE_WRITE_OP)(
	EFI_HANDLE FileHandle,
	CHAR8* Buffer,
	UINT32 Length
	);

typedef
UINT64
(*_FILE_SEEK_OP)(
	EFI_HANDLE FileHandle,
	UINT64 Offset
	);

typedef
VOID
(*_FILE_CLOSE_OP)(
	EFI_HANDLE FileHandle
	);

typedef enum {
	TFTP,
	FILE,
	BIO
}FILE_TYPE;

typedef struct _FILE_HANDLE {

	EFI_HANDLE	Device;
	CHAR16*		RemainPath;
	EFI_HANDLE	File;

	FILE_TYPE	FileType;

	/* 缓存的数据 */
	PBYTE		Buffer;
	/* 缓存的数据位于文件的偏移 */
	UINT64		BufferOffset;
	UINT64		Offset;
	UINT64		Size;

	UINT64		OpenMode;
	UINT64		Attribute;

	_FILE_READ_OP	Read;
	_FILE_WRITE_OP	Write;
	_FILE_SEEK_OP	Seek;
	_FILE_CLOSE_OP	Close;

}FILE_HANDLE,*PFILE_HANDLE;

EFI_STATUS
EFIAPI
File_Create(
	EFI_HANDLE FileHandle,
	UINT64 OpenMode,
	UINT64 Attribute
);

EFI_STATUS
EFIAPI
Mtftp_Create(
	EFI_HANDLE FileHandle
);