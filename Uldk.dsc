
[Defines]
  PLATFORM_NAME                  = Uldk
  PLATFORM_GUID                  = DF5B81FC-642D-406A-B72D-C5887CA67207
  PLATFORM_VERSION               = 0.3
  DSC_SPECIFICATION              = 0x00010005
  SUPPORTED_ARCHITECTURES        = IA32|X64
  OUTPUT_DIRECTORY               = Build/Uldk
  BUILD_TARGETS                  = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER               = DEFAULT

[LibraryClasses]

  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf

  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf  
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf

  NetLib|MdeModulePkg/Library/DxeNetLib/DxeNetLib.inf
  IpIoLib|MdeModulePkg/Library/DxeIpIoLib/DxeIpIoLib.inf
  UdpIoLib|MdeModulePkg/Library/DxeUdpIoLib/DxeUdpIoLib.inf
  TcpIoLib|MdeModulePkg/Library/DxeTcpIoLib/DxeTcpIoLib.inf
  DpcLib|MdeModulePkg/Library/DxeDpcLib/DxeDpcLib.inf

  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf

  HashKvLib|Uldk/Lib/HashKvLib/HashKvLib.inf
  XmlParserLib|Uldk/Lib/XmlParserLib/XmlParserLib.inf
  EnvMgrLib|Uldk/Lib/EnvMgrLib/EnvMgrLib.inf
  DeviceHelperLib|Uldk/Lib/DeviceHelperLib/DeviceHelperLib.inf
  MtftpLib|Uldk/Lib/MtftpLib/MtftpLib.inf
  FileLib|Uldk/Lib/FileHandleLib/FileLib.inf
  SearchLib|Uldk/Lib/Search/Search.inf
  ImageDecoderLib|Uldk/Lib/ImageDecoderLib/ImageDecoderLib.inf
  VideoLib|Uldk/Lib/VideoLib/VideoLib.inf
  ResourceLib|Uldk/Lib/ResourceLib/ResourceLib.inf
  LwGuiLib|Uldk/Lib/LwGuiLib/LwGuiLib.inf

###################################################################################################
#
# Components Section - list of the modules and components that will be processed by compilation
#                      tools and the EDK II tools to generate PE32/PE32+/Coff image files.
#
# Note: The EDK II DSC file is not used to specify how compiled binary images get placed
#       into firmware volume images. This section is just a list of modules to compile from
#       source into UEFI-compliant binaries.
#       It is the FDF file that contains information on combining binary files into firmware
#       volume images, whose concept is beyond UEFI and is described in PI specification.
#       Binary modules do not need to be listed in this section, as they should be
#       specified in the FDF file. For example: Shell binary (Shell_Full.efi), FAT binary (Fat.efi),
#       Logo (Logo.bmp), and etc.
#       There may also be modules listed in this section that are not required in the FDF file,
#       When a module listed here is excluded from FDF file, then UEFI-compliant binary will be
#       generated for it, but the binary will not be put into any firmware volume.
#
###################################################################################################

[Components]

	Uldk/Lib/HashKvLib/HashKvLib.inf
	Uldk/Lib/XmlParserLib/XmlParserLib.inf
	Uldk/Lib/EnvMgrLib/EnvMgrLib.inf
	Uldk/Lib/DeviceHelperLib/DeviceHelperLib.inf
	Uldk/Lib/MtftpLib/MtftpLib.inf
	Uldk/Lib/FileHandleLib/FileLib.inf
	Uldk/Lib/Search/Search.inf
	Uldk/Lib/ImageDecoderLib/ImageDecoderLib.inf
	Uldk/Lib/VideoLib/VideoLib.inf
	Uldk/Lib/ResourceLib/ResourceLib.inf
	Uldk/Lib/LwGuiLib/LwGuiLib.inf

	Uldk/TestUefi/TestUefi.inf