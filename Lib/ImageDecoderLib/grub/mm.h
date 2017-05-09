
#include "types.h"

#define ARRAY_SIZE(array) (sizeof (array) / sizeof (array[0]))

void*
grub_malloc(
	UINT64 size
);

void*
grub_zalloc(
	UINT64 size
);

void
grub_memcpy(
	void* dst,
	const void* src,
	UINT64 size
);

UINT64
grub_memcmp(
	const UINT8* mem1,
	const UINT8* mem2,
	UINT64 size
);


void
grub_free(
	void* ptr
);

void
grub_memset(
	void* ptr,
	int value,
	UINT64 size
);

void
grub_memmove(
	void* dst,
	const void* src,
	UINT64 size
);

void*
grub_realloc(
	void* src,
	UINT64 new_size
);