
#include <EdkClass/Common.h>
#include <EdkClass/NetProtocolStack.h>

#include <Lib/MtftpLib.h>

//利用TFTP下载文件
EFI_STATUS
EFIAPI
MtftpLibDownloadFile(
	IN TFTP_IO*				TftpIo,
	IN CHAR8*				FileName,
	OUT CHAR8**				Buffer,
	OUT UINTN*				FileSize
) {

	UINT32 RetSize;
	EFI_STATUS status;
	EFI_MTFTP4_PACKET* RetPacket;

	//tsize=0
	//blksize=1468
	EFI_MTFTP4_OPTION Option[2] = {
		{ "tsize", "0" },
		{ "blksize", "1468" }
	};
	
	status = TftpIo->TftpProtocol->GetInfo(TftpIo->TftpProtocol,
		NULL,
		FileName,
		NULL,
		2,
		Option,
		&RetSize,
		&RetPacket
	);
	if (EFI_ERROR(status))
	{
		return FALSE;
	}

	UINTN tsize = AsciiStrDecimalToUintn(RetPacket->Oack.Data + 6);
	CHAR8* FileBuffer = (CHAR8*)AllocatePool(tsize);

	EFI_MTFTP4_TOKEN MtftpToken;
	MtftpToken.Filename = FileName;
	MtftpToken.OverrideData = NULL;
	MtftpToken.Event = NULL;
	MtftpToken.Buffer = FileBuffer;
	MtftpToken.BufferSize = tsize;
	MtftpToken.TimeoutCallback = NULL;
	MtftpToken.Context = NULL;
	MtftpToken.OptionCount = 1;
	MtftpToken.OptionList = &Option[1];
	MtftpToken.PacketNeeded = NULL;
	MtftpToken.CheckPacket = NULL;
	MtftpToken.OverrideData = NULL;
	MtftpToken.ModeStr = NULL;

	status = TftpIo->TftpProtocol->ReadFile(TftpIo->TftpProtocol,
		&MtftpToken
	);
	if (EFI_ERROR(status))
	{
		FreePool(FileBuffer);
		*Buffer = NULL;
		*FileSize = 0;
		return FALSE;
	}

	*Buffer = FileBuffer;
	*FileSize = tsize;

	return TRUE;

}

//根据DHCP配置TFTP
EFI_STATUS
EFIAPI
MtftpLibConfigure(
	IN TFTP_IO					*TftpIo,
	IN EFI_MTFTP4_CONFIG_DATA*	Config,
	IN BOOLEAN					UseDhcp
) {

	EFI_STATUS status;
	EFI_DHCP4_MODE_DATA ModeData;
	EFI_DHCP4_PROTOCOL* Dhcp;

	status = gBS->LocateProtocol(&gEfiDhcp4ProtocolGuid,
		NULL,
		(void**)&Dhcp
	);
	Dhcp->GetModeData(Dhcp, &ModeData);

	EFI_MTFTP4_CONFIG_DATA MtftpConfigData = { 0 };

	if (UseDhcp)
	{

		MtftpConfigData.UseDefaultSetting = FALSE;
		CopyMem(MtftpConfigData.SubnetMask.Addr, ModeData.SubnetMask.Addr, 4);
		CopyMem(MtftpConfigData.ServerIp.Addr, ModeData.ServerAddress.Addr, 4);
		CopyMem(MtftpConfigData.StationIp.Addr, ModeData.ClientAddress.Addr, 4);

	}
	else
	{

		CopyMem(&MtftpConfigData, Config, sizeof(*Config));

	}

	return TftpIo->TftpProtocol->Configure(TftpIo->TftpProtocol,
		&MtftpConfigData
	);

}

UINTN
EFIAPI
MtftpQueryFileSize(
	IN TFTP_IO*					TftpIo,
	IN CHAR8*					FileName
) {

	UINT32 RetSize;
	EFI_STATUS status;
	EFI_MTFTP4_PACKET* RetPacket;
	EFI_MTFTP4_OPTION Option[1] = {
		{ "tsize", "0" }
	};
	status = TftpIo->TftpProtocol->GetInfo(TftpIo->TftpProtocol,
		NULL,
		FileName,
		NULL,
		1,
		Option,
		&RetSize,
		&RetPacket
	);
	if (EFI_ERROR(status))
	{
		return 0;
	}

	return AsciiStrDecimalToUintn(RetPacket->Oack.Data + 6);

}

/* 销毁TFTP实例 */

VOID
EFIAPI
MtftpLibDestoryInstance(
	IN TFTP_IO*					TftpIo
) {

	EFI_STATUS						Status;
	EFI_SERVICE_BINDING_PROTOCOL	*MtftpBinding;
	EFI_HANDLE MtftpChildHandle = NULL;

	Status = gBS->LocateProtocol(&gEfiMtftp4ServiceBindingProtocolGuid,
		NULL,
		(void**)&MtftpBinding
	);
	if (Status != EFI_SUCCESS)
	{
		return;
	}

	Status = MtftpBinding->DestroyChild(MtftpBinding,
		TftpIo->TftpInstance
	);
	if (Status != EFI_SUCCESS)
	{
		return;
	}

	/*
	gBS->CloseProtocol(TftpIo->TftpInstance,
	&gEfiMtftp4ProtocolGuid,
	gImageHandle,
	NULL
	);
	*/
}

/*创建TFTP实例 */

TFTP_IO*
EFIAPI
MtftpLibCreateInstance(
	IN UINT16	BlkSize
) {

	EFI_STATUS Status;
	EFI_MTFTP4_PROTOCOL* Mtftp = NULL;
	EFI_SERVICE_BINDING_PROTOCOL* MtftpBinding = NULL;
	EFI_HANDLE MtftpChildHandle = NULL;
	TFTP_IO* TftpIo = (TFTP_IO*)AllocateZeroPool(sizeof(TFTP_IO));

	/* 创建Tftp实例 */
	Status = gBS->LocateProtocol(&gEfiMtftp4ServiceBindingProtocolGuid,
		NULL,
		(void**)&MtftpBinding
	);
	if (Status != EFI_SUCCESS)
	{
		return NULL;
	}

	Status = MtftpBinding->CreateChild(MtftpBinding,
		&MtftpChildHandle
	);
	if (Status != EFI_SUCCESS)
	{
		goto EXIT2;
	}

	Status = gBS->HandleProtocol(MtftpChildHandle,
		&gEfiMtftp4ProtocolGuid, (void**)&Mtftp
	);
	if (Status != EFI_SUCCESS)
	{
		goto EXIT1;
	}

	TftpIo->TftpInstance = MtftpChildHandle;
	TftpIo->TftpProtocol = Mtftp;

	if (BlkSize > 0)
		TftpIo->BlkSize = BlkSize;
	else
		TftpIo->BlkSize = 1468;

	goto EXIT3;

EXIT1:

	MtftpBinding->DestroyChild(MtftpBinding, MtftpChildHandle);

EXIT2:

	gBS->CloseProtocol(
		gImageHandle,
		&gEfiMtftp4ServiceBindingProtocolGuid,
		gImageHandle,
		NULL
	);

EXIT3:

	if (EFI_ERROR(Status))
	{
		FreePool(TftpIo);
		return NULL;
	}
	else
		return TftpIo;

}