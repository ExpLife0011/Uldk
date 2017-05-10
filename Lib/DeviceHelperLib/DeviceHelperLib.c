
#include <EdkClass/Common.h>
#include <EdkClass/Helper.h>
#include <EdkClass/MediaAccess.h>
#include <EdkClass/Image.h>
#include <ExtTypes.h>

static LIST_ENTRY HdDeviceList = INITIALIZE_LIST_HEAD_VARIABLE(HdDeviceList);
static LIST_ENTRY UsbDeviceList = INITIALIZE_LIST_HEAD_VARIABLE(HdDeviceList);

typedef struct {

	LIST_ENTRY	DevNode;
	EFI_HANDLE	HdDevHandle;
	CHAR16		SymbolName[32];

}DEVICE_NODE, *PDEVICE_NODE;

/* 获得路径节点数量,不包括结束节点数量 */

UINTN
EFIAPI
DhGetDevicePathLength(
	IN EFI_DEVICE_PATH_PROTOCOL* FullPath
) {

	UINTN Length = 0;
	EFI_DEVICE_PATH_PROTOCOL* TargetPath = DuplicateDevicePath(FullPath);
	EFI_DEVICE_PATH_PROTOCOL* CurNode = TargetPath;

	while (!IsDevicePathEnd(CurNode)) {

		CurNode = NextDevicePathNode(CurNode);
		Length++;

	}
	FreePool(TargetPath);

	return Length;

}

/* 获得路径中的最后一个节点 */

EFI_DEVICE_PATH_PROTOCOL*
EFIAPI
DhGetLastPathNode(
	IN EFI_DEVICE_PATH_PROTOCOL* FullPath
) {

	EFI_DEVICE_PATH_PROTOCOL* TargetPath = DuplicateDevicePath(FullPath);
	EFI_DEVICE_PATH_PROTOCOL* CurNode = TargetPath, *PrevNode = TargetPath;
	EFI_DEVICE_PATH_PROTOCOL* TargetNode;

	while (!IsDevicePathEnd(CurNode)) {

		PrevNode = CurNode;
		CurNode = NextDevicePathNode(CurNode);

	}

	TargetNode = (EFI_DEVICE_PATH_PROTOCOL*)((BYTE*)FullPath + ((BYTE*)PrevNode - (BYTE*)TargetPath));
	FreePool(TargetPath);

	return PrevNode;

}

/* 判断某个路径是否是另一个路径的子路径 */

BOOLEAN
EFIAPI 
DhIsChild(
	IN EFI_DEVICE_PATH_PROTOCOL* Parent,
	IN EFI_DEVICE_PATH_PROTOCOL* MaybeChild
) {

	EFI_DEVICE_PATH_PROTOCOL* TargetPath = DuplicateDevicePath(Parent);
	EFI_DEVICE_PATH_PROTOCOL* CurNode = TargetPath;

	while (!IsDevicePathEnd(CurNode)) 
		CurNode = NextDevicePathNode(CurNode);

	UINTN Size = (BYTE*)CurNode - (BYTE*)TargetPath;
	
	BOOLEAN Result = (CompareMem(MaybeChild, TargetPath, Size) == 0);
	FreePool(TargetPath);

	return Result == TRUE;

}

static
BOOLEAN
DhScanIfHaveFileSystem(
	IN EFI_HANDLE	PartDevice
) {

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FatFs = NULL;
	gBS->HandleProtocol(PartDevice, &gEfiSimpleFileSystemProtocolGuid, &FatFs);

	return FatFs != NULL;

}

/* 寻找设备列表里比磁盘设备路径大一的节点作为分区节点 */

static
VOID
DhScanHdPartMap(
	IN EFI_HANDLE	HdDevice,
	IN CHAR16*		Prefix,
	IN EFI_HANDLE*	BioDevices,
	IN UINTN		BioCount
) {

	EFI_DEVICE_PATH_PROTOCOL* DevPath = DevicePathFromHandle(HdDevice);
	UINTN Len = DhGetDevicePathLength(DevPath);
	static int FsIndex = 0;

	for (UINTN j = 0;j < BioCount;j++) {

		EFI_DEVICE_PATH_PROTOCOL* SubDevPath = DevicePathFromHandle(BioDevices[j]);
		UINTN SubLen = DhGetDevicePathLength(SubDevPath);

		if ((SubLen == Len + 1) && DhIsChild(DevPath, SubDevPath)) {

			EFI_DEVICE_PATH_PROTOCOL* LastPathNode = DhGetLastPathNode(SubDevPath);

			/* 构造HDD分区设备节点 */
			DEVICE_NODE* Node = (DEVICE_NODE*)AllocatePool(sizeof(DEVICE_NODE));
			Node->HdDevHandle = BioDevices[j];
			UnicodeSPrint(Node->SymbolName, 32, L"%s,%d", Prefix, ((HARDDRIVE_DEVICE_PATH*)LastPathNode)->PartitionNumber);

			//Print(L"%s\n", Node->SymbolName);

			InsertTailList(&HdDeviceList, &Node->DevNode);

			/* 如果含有文件系统则建立文件系统符号 */
			if (DhScanIfHaveFileSystem(Node->HdDevHandle)) {

				DEVICE_NODE* Node = (DEVICE_NODE*)AllocatePool(sizeof(DEVICE_NODE));
				Node->HdDevHandle = BioDevices[j];
				UnicodeSPrint(Node->SymbolName, 32, L"fs%d", FsIndex);

				InsertTailList(&HdDeviceList, &Node->DevNode);
				FsIndex++;

			}

		}

	}

}

/* 建立硬盘设备映射 */

static
VOID
DhScanHdDeviceMap(
) {

	EFI_HANDLE* BioDevices;
	UINTN BioCount;

	gBS->LocateHandleBuffer(ByProtocol,
		&gEfiBlockIoProtocolGuid,
		NULL,
		&BioCount,
		&BioDevices);

	if (BioCount > 0) {

		for (UINTN i = 0;i < BioCount;i++) {

			EFI_DEVICE_PATH_PROTOCOL* DevPath = DevicePathFromHandle(BioDevices[i]);
			EFI_DEVICE_PATH_PROTOCOL* LastNode = DhGetLastPathNode(DevPath);

			if (DevicePathSubType(LastNode) == MSG_SATA_DP ||
				DevicePathSubType(LastNode) == MSG_SCSI_DP) {

				/* 构造HDD设备节点 */
				DEVICE_NODE* Node = (DEVICE_NODE*)AllocatePool(sizeof(DEVICE_NODE));
				Node->HdDevHandle = BioDevices[i];
				UnicodeSPrint(Node->SymbolName, 32, L"hd%d", i);

				InsertTailList(&HdDeviceList, &Node->DevNode);

				/* 寻找所有分区节点 */
				DhScanHdPartMap(BioDevices[i], Node->SymbolName, BioDevices, BioCount);

			}

		}

		FreePool(BioDevices);

	}

}

/* 根据符号名获取对应的设备句柄 */

EFI_HANDLE
EFIAPI
DhRetrieveDevice(
	IN CHAR16* SymbolName
) {

	LIST_ENTRY* Cur = HdDeviceList.ForwardLink;
	while (Cur != &HdDeviceList) {

		DEVICE_NODE* Node = (DEVICE_NODE*)Cur;
		if (StrCmp(Node->SymbolName, SymbolName) == 0)
			return Node->HdDevHandle;

		Cur = Cur->ForwardLink;

	}

	return NULL;

}

EFI_HANDLE
EFIAPI
DhGetRootDevice(
) {

	EFI_STATUS Status;
	EFI_LOADED_IMAGE_PROTOCOL* LdrImage;

	Status = gBS->HandleProtocol(gImageHandle, &gEfiLoadedImageProtocolGuid, &LdrImage);
	if (EFI_ERROR(Status))
		return NULL;

	return LdrImage->DeviceHandle;

}

/* 初始化库 */

VOID
EFIAPI
DhInitDeviceHelperLib(
) {

	DhScanHdDeviceMap();

}