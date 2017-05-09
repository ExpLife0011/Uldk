
#include <EdkClass/Common.h>

#include <grub/types.h>
#include <grub/err.h>
#include <grub/file.h>
#include <grub/mm.h>

grub_file_t grub_file_open(
	UINT8* buf,
	UINT32 size
) {

	grub_file_t file;

	file = AllocateZeroPool(sizeof(*file));

	//部分固件的FAT驱动没有进行缓存，这里就直接全部读取到内存

	file->buf = buf;
	file->size = size;

	return file;

}


INT64 grub_file_read(grub_file_t file, void *buf,
	UINT64 len
) {

	grub_memcpy(buf, &file->buf[file->offset], len);

	file->offset += (UINT32)len;

	return len;

}

UINT32 grub_file_seek(grub_file_t file,
	UINT32 offset)
{

	file->offset = offset;

	return offset;

}

grub_err_t grub_file_close(grub_file_t file
) {

	grub_free(file->buf);
	grub_free(file);

	return 0;

}