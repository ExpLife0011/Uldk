
#ifndef GRUB_FILE_HEADER
#define GRUB_FILE_HEADER	1

#include <grub/types.h>
#include <grub/err.h>

/* File description.  */
struct grub_file
{
	/* File name.  */
	char *name;

	/* The underlying device.  */
	EFI_HANDLE device;

	/* The underlying filesystem.  */
	EFI_HANDLE fs;

	/* The current offset.  */
	UINT32 offset;
	UINT32 progress_offset;

	/* Progress info. */
	UINT64 last_progress_time;
	UINT32 last_progress_offset;
	UINT64 estimated_speed;

	/* The file size.  */
	UINT32 size;

	/* If file is not easily seekable. Should be set by underlying layer.  */
	int not_easily_seekable;

	/* Filesystem-specific data.  */
	void *data;

	/* This is called when a sector is read. Used only for a disk device.  */
	//grub_disk_read_hook_t read_hook;

	/* Caller-specific data passed to the read hook.  */
	void *read_hook_data;

	unsigned char* buf;

	UINT32 buf_off;

};
typedef struct grub_file *grub_file_t;

grub_file_t grub_file_open(UINT8* buf, UINT32 size);
INT64(grub_file_read) (grub_file_t file, void *buf,
	UINT64 len);
UINT32(grub_file_seek) (grub_file_t file, UINT32 offset);
grub_err_t(grub_file_close) (grub_file_t file);

#endif