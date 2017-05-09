
#include "grub/bitmap.h"
#include "grub/types.h"

#include <Lib/ImageDecoderLib.h>

UINT16
EFIAPI
GetBitmapWidth(
	IN EFI_HANDLE bitmap
) {

	struct grub_video_bitmap* b = (struct grub_video_bitmap*)bitmap;
	return b->mode_info.width;

}

UINT16
EFIAPI
GetBitmapHeight(
	IN EFI_HANDLE bitmap
) {

	struct grub_video_bitmap* b = (struct grub_video_bitmap*)bitmap;
	return b->mode_info.height;

}

UINT16
EFIAPI
GetBitmapDepth(
	IN EFI_HANDLE bitmap
) {

	struct grub_video_bitmap* b = (struct grub_video_bitmap*)bitmap;
	return b->mode_info.bytes_per_pixel;

}

VOID*
EFIAPI
GetBitmapData(
	IN EFI_HANDLE bitmap
) {

	struct grub_video_bitmap* b = (struct grub_video_bitmap*)bitmap;
	return b->data;

}