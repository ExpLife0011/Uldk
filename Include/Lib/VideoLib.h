
#pragma once

#include <EdkClass/Common.h>
#include <EdkClass/Video.h>

#define BITMAP_FORMAT_UNKNOW		0x0000
#define BITMAP_FORMAT_RGB			0x0001
#define BITMAP_FORMAT_RGBA			0x0002
#define BITMAP_FORMAT_INDEX			0x0003

#define BRUSH_TYPE_FILL				1
#define BRUSH_TYPE_IMAGE			2
#define BRUSH_TYPE_STRING			3

typedef struct _VIDEO_BITMAP
{

	UINT16 Width;
	UINT16 Height;
	UINT16 Alpha;
	UINT16 Format;

	EFI_GRAPHICS_OUTPUT_BLT_PIXEL* BitmapData;

}VIDEO_BITMAP, *PVIDEO_BITMAP;

EFI_HANDLE
EFIAPI
VideoLibCreateBitmapObject(
	IN UINT16			Format,
	IN UINT16			Width,
	IN UINT16			Height
);

VOID
EFIAPI
VideoLibDestoryBitmapObject(
	IN EFI_HANDLE		BitmapObject
);

EFI_STATUS
EFIAPI
VideoLibFillRect(
	IN EFI_HANDLE		BitmapObject,
	IN UINT16			StartX,
	IN UINT16			StartY,
	IN UINT16			Width,
	IN UINT16			Height,
	IN UINT32			Color
);

EFI_STATUS
EFIAPI
VideoLibBitblt(
	IN EFI_HANDLE		SrcObject,
	IN EFI_HANDLE		DstObject,
	IN UINT16			SrcX,
	IN UINT16			SrcY,
	IN UINT16			DstX,
	IN UINT16			DstY,
	IN UINT16			Width,
	IN UINT16			Height
);

UINT16
EFIAPI
VideoLibGetStringWidth(
	IN CHAR16*			String
);

EFI_STATUS
EFIAPI
VideoLibRenderFont(
	IN	EFI_HANDLE		BitmapObject,
	IN	CHAR16*			String,
	IN	UINT32			Color
);

EFI_HANDLE
EFIAPI
VideoLibCreateImage(
	IN CHAR16*			FileName,
	IN UINT16			Width,
	IN UINT16			Height
);

EFI_HANDLE
EFIAPI
VideoLibDuplicateBitmap(
	IN EFI_HANDLE		BitmapObject
);


VOID
EFIAPI
VideoLibSetResourceHandle(
	IN EFI_HANDLE Handle
);

/* 获取图像信息 */

EFI_GRAPHICS_OUTPUT_BLT_PIXEL*
EFIAPI
VideoLibGetImageBitmapData(
	IN EFI_HANDLE		BitmapObject
);

UINT16
EFIAPI
VideoLibGetImageWidth(
	IN EFI_HANDLE		BitmapObject
);

UINT16
EFIAPI
VideoLibGetImageHeight(
	IN EFI_HANDLE		BitmapObject
);