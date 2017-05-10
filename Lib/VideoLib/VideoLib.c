
#include <EdkClass/Common.h>
#include <EdkClass/Video.h>
#include <EdkClass/MediaAccess.h>

#include <ExtTypes.h>

#include <Lib/FileLib.h>
#include <Lib/ImageDecoderLib.h>
#include <Lib/VideoLib.h>
#include <Lib/ResourceLib.h>

static EFI_HANDLE ResourceHandle;

/* 创建一个图形对象 */

EFI_HANDLE
EFIAPI
VideoLibCreateBitmapObject(
	IN UINT16 Format,
	IN UINT16 Width,
	IN UINT16 Height
) {

	PVIDEO_BITMAP Bitmap = (PVIDEO_BITMAP)AllocatePool(sizeof(*Bitmap));
	if (Bitmap == NULL)
		return NULL;

	if (Width == 0 || Height == 0)
		return NULL;

	Bitmap->Width = Width;
	Bitmap->Height = Height;
	Bitmap->Format = Format;

	Bitmap->BitmapData = AllocateZeroPool(
		Width*Height * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
	);

	return (EFI_HANDLE)Bitmap;

}

/* 销毁一个图形对象 */

VOID
EFIAPI
VideoLibDestoryBitmapObject(
	IN EFI_HANDLE Bitmap
) {

	if(((PVIDEO_BITMAP)Bitmap)->BitmapData)
		FreePool(((PVIDEO_BITMAP)Bitmap)->BitmapData);

	FreePool(Bitmap);

}

/* 填充一个图形对象 */

EFI_STATUS
EFIAPI
VideoLibFillRect(
	IN EFI_HANDLE		BitmapObject,
	IN UINT16			StartX,
	IN UINT16			StartY,
	IN UINT16			Width,
	IN UINT16			Height,
	IN UINT32			Color
) {

	PVIDEO_BITMAP Dst = (PVIDEO_BITMAP)BitmapObject;

	if (Dst == NULL)
		return EFI_INVALID_PARAMETER;

	if (StartX > Dst->Width || StartY > Dst->Height)
		return EFI_INVALID_PARAMETER;

	UINT32* TmpVector = AllocatePool(sizeof(UINT32)*Width);

	SetMem32(TmpVector, sizeof(UINT32)*Width, Color);

	for (UINT16 i = StartY;i <StartY + Height;i++)
	{
		CopyMem(&Dst->BitmapData[Dst->Width*i + StartX], 
			TmpVector, 
			sizeof(UINT32)*Width
		);
	}

	FreePool(TmpVector);

	return EFI_SUCCESS;

}

/* 复制一个图形对象 */

EFI_HANDLE
EFIAPI
VideoLibDuplicateBitmap(
	IN EFI_HANDLE		BitmapObject
) {

	PVIDEO_BITMAP Dst = (PVIDEO_BITMAP)AllocateZeroPool(sizeof(*Dst));
	PVIDEO_BITMAP Src = (PVIDEO_BITMAP)BitmapObject;

	CopyMem(Dst, Src, sizeof(*Dst));

	VOID* Buffer = AllocateCopyPool(
		Dst->Width*Dst->Height * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL), Src->BitmapData
	);
	if (!Buffer)
	{
		FreePool(Dst);
		return NULL;
	}

	Dst->BitmapData = Buffer;

	return Dst;

}

/* 图形拷贝 */

EFI_STATUS
EFIAPI
VideoLibBitblt(
	IN EFI_HANDLE SrcObject,
	IN EFI_HANDLE DstObject,
	IN UINT16 SrcX,
	IN UINT16 SrcY,
	IN UINT16 DstX,
	IN UINT16 DstY,
	IN UINT16 Width,
	IN UINT16 Height
) {

	PVIDEO_BITMAP Src, Dst;
	Src = (PVIDEO_BITMAP)SrcObject;
	Dst = (PVIDEO_BITMAP)DstObject;

	//源图片只能整块或者其中一部分
	if (SrcX < 0 || SrcY < 0)
		return EFI_INVALID_PARAMETER;

	if ((Width > Src->Width - SrcX) ||
		(Height > Src->Height - SrcY))
		return EFI_INVALID_PARAMETER;

	if (SrcX > Src->Width)
		return EFI_INVALID_PARAMETER;

	if (SrcY > Src->Height)
		return EFI_INVALID_PARAMETER;

	if (DstX > Dst->Width)
		return EFI_INVALID_PARAMETER;

	if (DstY > Dst->Height)
		return EFI_INVALID_PARAMETER;

	//将合并区以外的裁剪掉

	UINT16 ActualWidth = MIN(Dst->Width - DstX, Src->Width - SrcX);
	UINT16 ActualHeight = MIN(Dst->Height - DstY, Src->Height - SrcY);
	UINT16 ActualSrcX = SrcX, ActualSrcY = SrcY, ActualDstX = DstX, ActualDstY = DstY;

	if (DstX < 0)
	{
		ActualSrcX = SrcX - DstX;
		ActualWidth = ActualWidth - DstX;
		ActualDstX = 0;
	}

	if (DstY < 0)
	{
		ActualSrcY = SrcY - DstY;
		ActualHeight = ActualHeight - DstY;
		ActualDstY = 0;
	}

	if (ActualWidth > Width)
		ActualWidth = Width;

	if (ActualHeight > Height)
		ActualHeight = Height;

	//修改RGB但不能影响到ALPHA

	if (Src->Format == BITMAP_FORMAT_RGB)
	{

		EFI_GRAPHICS_OUTPUT_BLT_PIXEL* LineBuffer = AllocatePool(sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)*ActualWidth);

		for (UINT16 i = ActualSrcY;i < ActualSrcY + ActualHeight;i++)
		{

			register UINT32*	DstBuffer = (UINT32*)&Dst->BitmapData[(ActualDstY + (i - ActualSrcY))*Dst->Width + ActualDstX];
			register UINT32*	LineBuffer = (UINT32*)&Src->BitmapData[i*Src->Width + ActualSrcX];

			for (UINT16 j = 0;j < ActualWidth;j++)
			{

				DstBuffer[j] = ((LineBuffer[j]) & 0xFFFFFF) | ((DstBuffer[j]) & 0xFF000000);

			}

		}
	}
	else if (Src->Format == BITMAP_FORMAT_RGBA)
	{

		for (UINT16 i = ActualSrcY;i < ActualSrcY + ActualHeight;i++)
		{

			EFI_GRAPHICS_OUTPUT_BLT_PIXEL*	DstBuffer = &Dst->BitmapData[(ActualDstY + (i - ActualSrcY))*Dst->Width + ActualDstX];
			EFI_GRAPHICS_OUTPUT_BLT_PIXEL* LineBuffer = &Src->BitmapData[i*Src->Width + ActualSrcX];

			for (UINT16 j = 0;j < ActualWidth;j++)
			{

				register UINT32 alpha = LineBuffer->Reserved;
				DstBuffer->Red = ((LineBuffer->Red*alpha) >> 8) + ((DstBuffer->Red*(255 - alpha)) >> 8);
				DstBuffer->Green = ((LineBuffer->Green*alpha) >> 8) + ((DstBuffer->Green*(255 - alpha)) >> 8);
				DstBuffer->Blue = ((LineBuffer->Blue*alpha) >> 8) + ((DstBuffer->Blue*(255 - alpha)) >> 8);

				LineBuffer++;
				DstBuffer++;

			}


		}

	}

	return	EFI_SUCCESS;

}

/* 将字符填充到缓冲区中 */

static
EFI_STATUS
VideoLibPrintCharToBuffer(
	CHAR16				Char,
	EFI_IMAGE_OUTPUT*	Output,
	UINT32				FgColor,
	UINT32				BgColor,
	UINTN				StartX,
	UINTN				StartY,
	UINTN*				RealWidth
) {

	CHAR16 IndexCode = Char;

	EFI_HANDLE Index = ResLibFindCharCode(ResourceHandle, IndexCode);

	/* 要么透明要么不透明 */

	BOOLEAN Transparent = (((EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)&BgColor)->Reserved == 0);

	BYTE* BitData = ResLibRetrieveCharMatrix(ResourceHandle, Index);
	UINTN FontWidth = ResLibGetCharWidth(ResourceHandle, Index);
	UINTN DrawWidth = (FontWidth > (Output->Width - StartX)) ? (Output->Width - StartX) : FontWidth;
	UINTN DrawHeight = (16 > (Output->Height - StartY)) ? (Output->Height - StartY) : 16;

	if (FontWidth > 8)
		FontWidth = 16;

	/* 渲染字符 */

	for (UINTN i = StartY;i < StartY + DrawHeight;i++)
	{

		unsigned char* LineBit = (unsigned char*)(BitData + (i - StartY)*(FontWidth / 8));
		UINT32 l = LineBit[0];
		if (FontWidth > 8)
		{
			l <<= 8;
			l += LineBit[1];
		}

		register UINT32* LinePixel = (UINT32*)&Output->Image.Bitmap[i*Output->Width];

		for (UINTN j = StartX;j < StartX + DrawWidth;j++)
		{
			if (_bittest(&l, FontWidth - 1 - (j - StartX)))
			{
				LinePixel[j] = FgColor;
			}
			else {

				if (!Transparent)
					LinePixel[j] = BgColor;

			}
		}
	}

	*RealWidth = DrawWidth;
	return EFI_SUCCESS;

}

/* 将一个字符串打到缓冲区中 */

EFI_STATUS
EFIAPI
VideoLibPrintStringToBuffer(
	CHAR16*				String,
	EFI_IMAGE_OUTPUT*	Output,
	UINT32				FgColor,
	UINT32				BgColor,
	UINTN				StartX,
	UINTN				StartY
) {

	if (String == NULL)
		return EFI_INVALID_PARAMETER;

	if (StartY > Output->Height)
		return EFI_SUCCESS;

	UINTN CurX = StartX;

	for (UINTN i = 0;i < StrLen(String);i++)
	{
		UINTN RealWidth = 0;
		VideoLibPrintCharToBuffer(String[i], Output, FgColor, BgColor, CurX, StartY, &RealWidth);
		CurX += RealWidth;

		if (CurX > Output->Width)
		{
			break;
		}
	}

	return EFI_SUCCESS;

}

UINT16
EFIAPI
VideoLibGetStringWidth(
	IN CHAR16*		String
) {

	return ResLibGetStringWidth(ResourceHandle, String);

}

EFI_STATUS
EFIAPI
VideoLibRenderFont(
	IN	EFI_HANDLE	BitmapObject,
	IN	CHAR16*		String,
	IN	UINT32		Color
) {

	PVIDEO_BITMAP Bitmap = (PVIDEO_BITMAP)BitmapObject;

	EFI_IMAGE_OUTPUT Output;
	Output.Width = Bitmap->Width;
	Output.Height = Bitmap->Height;
	Output.Image.Bitmap = Bitmap->BitmapData;

	return VideoLibPrintStringToBuffer(String, &Output, Color, 0, 0, 0);

}

/* 打开一个图片并返回一个图片对象 */

EFI_HANDLE
EFIAPI
VideoLibCreateImage(
	IN CHAR16*	FileName,
	IN UINT16	Width,
	IN UINT16	Height
) {

	PVIDEO_BITMAP Bitmap = (PVIDEO_BITMAP)AllocatePool(sizeof(*Bitmap));
	if (Bitmap == NULL)
		return NULL;

	CHAR8* buf;
	UINT32 size;

	EFI_HANDLE FileHandle;
	FileHandle = FileLibCreateFile(FileName, EFI_FILE_MODE_READ, 0);
	if (FileHandle == NULL) {
		goto EXIT2;
	}

	size = (UINT32)FileLibGetFileSize(FileHandle);
	buf = AllocatePool((UINTN)size);
	if (FileLibReadFile(FileHandle, buf, size) != size) {
		goto EXIT1;
	}
	FileLibCloseFile(FileHandle);

	EFI_IMAGE_OUTPUT Output;
	Output.Width = Width;
	Output.Height = Height;

	if(!ReadPng(&Output, buf, size))
	{
		goto EXIT1;
	}

	Bitmap->Width = Output.Width;
	Bitmap->Height = Output.Height;
	Bitmap->BitmapData = Output.Image.Bitmap;
	Bitmap->Format = BITMAP_FORMAT_RGB;
	FreePool(buf);

	return (EFI_HANDLE)Bitmap;

EXIT1:

	FreePool(buf);
	
EXIT2:

	FreePool(Bitmap);

	return NULL;

}

VOID
EFIAPI
VideoLibSetResourceHandle(
	IN EFI_HANDLE Handle
) {

	ResourceHandle = Handle;

}


/* 获取图像信息 */

EFI_GRAPHICS_OUTPUT_BLT_PIXEL*
EFIAPI
VideoLibGetImageBitmapData(
	IN EFI_HANDLE BitmapObject
) {

	PVIDEO_BITMAP Bitmap = (PVIDEO_BITMAP)BitmapObject;
	return Bitmap->BitmapData;

}

UINT16
EFIAPI
VideoLibGetImageWidth(
	IN EFI_HANDLE BitmapObject
) {

	PVIDEO_BITMAP Bitmap = (PVIDEO_BITMAP)BitmapObject;
	return Bitmap->Width;

}

UINT16
EFIAPI
VideoLibGetImageHeight(
	IN EFI_HANDLE BitmapObject
) {

	PVIDEO_BITMAP Bitmap = (PVIDEO_BITMAP)BitmapObject;
	return Bitmap->Height;

}