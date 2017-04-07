
#include <EdkClass/Common.h>
#include <EdkClass/NetProtocolStack.h>

typedef struct _TFTP_IO
{

	EFI_HANDLE				TftpInstance;
	EFI_MTFTP4_PROTOCOL*	TftpProtocol;

	UINT16					BlkSize;

}TFTP_IO, *PTFTP_IO;

TFTP_IO*
EFIAPI
MtftpLibCreateInstance(
	IN UINT16	BlkSize
);

VOID
EFIAPI
MtftpLibDestoryInstance(
	IN TFTP_IO*					TftpIo
);

EFI_STATUS
EFIAPI
MtftpLibConfigure(
	IN TFTP_IO					*TftpIo,
	IN EFI_MTFTP4_CONFIG_DATA*	Config,
	IN BOOLEAN					UseDhcp
);

UINTN
EFIAPI
MtftpQueryFileSize(
	IN TFTP_IO*					TftpIo,
	IN CHAR16*					FileName
);

EFI_STATUS
EFIAPI
MtftpLibDownloadFile(
	IN TFTP_IO					*TftpIo,
	IN CHAR8					*FileName,
	OUT CHAR8					**Buffer,
	OUT UINTN					*FileSize
);