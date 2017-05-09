
#include <EdkClass/Common.h>

#include <grub/types.h>
#include <grub/err.h>
#include <grub/file.h>
#include <grub/mm.h>

EFI_STATUS grub_errno = 0;

grub_err_t
grub_error(
	grub_err_t errno,
	UINT8* errstr,
	...
) {

	grub_errno = errno;
	AsciiPrint(errstr);
	return errno;

}

void*
grub_malloc(
	UINT64 size
) {

	return AllocateZeroPool(size);

}

void*
grub_zalloc(
	UINT64 size
) {

	return AllocateZeroPool(size);

}

void
grub_memcpy(
	void* dst,
	const void* src,
	UINT64 size
) {

	CopyMem(dst, src, size);

}

UINT64
grub_memcmp(
	const UINT8* mem1,
	const UINT8* mem2,
	UINT64 size
) {

	return (UINT64)CompareMem(mem1, mem2, size);

}

void
grub_free(
	void* ptr
) {

	FreePool(ptr);

}

void
grub_memset(
	void* ptr,
	int value,
	UINT64 size
) {

	SetMem(ptr, size, value);

}

void
grub_memmove(
	void* dst,
	const void* src,
	UINT64 size
) {

	CopyMem(dst, src, size);

}

void*
grub_realloc(
	void* src,
	UINT64 new_size
) {

	return AllocateCopyPool(new_size, src);

}

char*
grub_strchr(
	char* src,
	char chr
) {

	char cs[2];
	cs[0] = chr;
	cs[1] = 0;
	return AsciiStrStr(src, cs);

}