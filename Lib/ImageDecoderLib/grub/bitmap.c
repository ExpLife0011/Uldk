
#include <grub/types.h>
#include <grub/err.h>
#include <grub/file.h>
#include <grub/bitmap.h>
#include <grub/video.h>
#include <grub/mm.h>

extern int grub_errno;

/* List of bitmap readers registered to system.  */
static grub_video_bitmap_reader_t bitmap_readers_list;

/* Register bitmap reader.  */
void
grub_video_bitmap_reader_register(grub_video_bitmap_reader_t reader)
{
	reader->next = bitmap_readers_list;
	bitmap_readers_list = reader;
}

/* Unregister bitmap reader.  */
void
grub_video_bitmap_reader_unregister(grub_video_bitmap_reader_t reader)
{
	grub_video_bitmap_reader_t *p, q;

	for (p = &bitmap_readers_list, q = *p; q; p = &(q->next), q = q->next)
		if (q == reader)
		{
			*p = q->next;
			break;
		}
}

/* Creates new bitmap, saves created bitmap on success to *bitmap.  */
grub_err_t
grub_video_bitmap_create(struct grub_video_bitmap **bitmap,
	unsigned int width, unsigned int height,
	enum grub_video_blit_format blit_format)
{
	struct grub_video_mode_info *mode_info;
	unsigned int size;

	if (!bitmap)
		return grub_error(GRUB_ERR_BUG, "invalid argument");

	*bitmap = 0;

	if (width == 0 || height == 0)
		return grub_error(GRUB_ERR_BUG, "invalid argument");

	*bitmap = (struct grub_video_bitmap *)grub_malloc(sizeof(struct grub_video_bitmap));
	if (!*bitmap)
		return grub_errno;

	mode_info = &((*bitmap)->mode_info);

	/* Populate mode_info.  */
	mode_info->width = width;
	mode_info->height = height;
	mode_info->blit_format = blit_format;

	switch (blit_format)
	{
	case GRUB_VIDEO_BLIT_FORMAT_RGBA_8888:
		mode_info->mode_type = GRUB_VIDEO_MODE_TYPE_RGB
			| GRUB_VIDEO_MODE_TYPE_ALPHA;
		mode_info->bpp = 32;
		mode_info->bytes_per_pixel = 4;
		mode_info->number_of_colors = 256;
		mode_info->red_mask_size = 8;
		mode_info->red_field_pos = 0;
		mode_info->green_mask_size = 8;
		mode_info->green_field_pos = 8;
		mode_info->blue_mask_size = 8;
		mode_info->blue_field_pos = 16;
		mode_info->reserved_mask_size = 8;
		mode_info->reserved_field_pos = 24;
		break;

	case GRUB_VIDEO_BLIT_FORMAT_RGB_888:
		mode_info->mode_type = GRUB_VIDEO_MODE_TYPE_RGB;
		mode_info->bpp = 24;
		mode_info->bytes_per_pixel = 3;
		mode_info->number_of_colors = 256;
		mode_info->red_mask_size = 8;
		mode_info->red_field_pos = 0;
		mode_info->green_mask_size = 8;
		mode_info->green_field_pos = 8;
		mode_info->blue_mask_size = 8;
		mode_info->blue_field_pos = 16;
		mode_info->reserved_mask_size = 0;
		mode_info->reserved_field_pos = 0;
		break;

	case GRUB_VIDEO_BLIT_FORMAT_INDEXCOLOR:
		mode_info->mode_type = GRUB_VIDEO_MODE_TYPE_INDEX_COLOR;
		mode_info->bpp = 8;
		mode_info->bytes_per_pixel = 1;
		mode_info->number_of_colors = 256;
		mode_info->red_mask_size = 0;
		mode_info->red_field_pos = 0;
		mode_info->green_mask_size = 0;
		mode_info->green_field_pos = 0;
		mode_info->blue_mask_size = 0;
		mode_info->blue_field_pos = 0;
		mode_info->reserved_mask_size = 0;
		mode_info->reserved_field_pos = 0;
		break;

	default:
		grub_free(*bitmap);
		*bitmap = 0;

		return grub_error(GRUB_ERR_NOT_IMPLEMENTED_YET,
			"unsupported bitmap format");
	}

	mode_info->pitch = width * mode_info->bytes_per_pixel;

	/* Calculate size needed for the data.  */
	size = (width * mode_info->bytes_per_pixel) * height;

	(*bitmap)->data = grub_zalloc(size);
	if (!(*bitmap)->data)
	{
		grub_free(*bitmap);
		*bitmap = 0;

		return grub_errno;
	}

	return GRUB_ERR_NONE;
}

/* Frees all resources allocated by bitmap.  */
grub_err_t
grub_video_bitmap_destroy(struct grub_video_bitmap *bitmap)
{
	if (!bitmap)
		return GRUB_ERR_NONE;

	grub_free(bitmap->data);
	grub_free(bitmap);

	return GRUB_ERR_NONE;
}