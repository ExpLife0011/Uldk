
#pragma once

#include <EdkClass/Common.h>

#define EXPORT_FUNC(x)	x

/* Byte-orders.  */
static inline UINT16 grub_swap_bytes16(UINT16 _x)
{
	return (UINT16)((_x << 8) | (_x >> 8));
}

#define grub_swap_bytes16_compile_time(x) ((((x) & 0xff) << 8) | (((x) & 0xff00) >> 8))
#define grub_swap_bytes32_compile_time(x) ((((x) & 0xff) << 24) | (((x) & 0xff00) << 8) | (((x) & 0xff0000) >> 8) | (((x) & 0xff000000UL) >> 24))
#define grub_swap_bytes64_compile_time(x)	\
({ \
   UINT64 _x = (x); \
   (UINT64) ((_x << 56) \
                    | ((_x & (UINT64) 0xFF00ULL) << 40) \
                    | ((_x & (UINT64) 0xFF0000ULL) << 24) \
                    | ((_x & (UINT64) 0xFF000000ULL) << 8) \
                    | ((_x & (UINT64) 0xFF00000000ULL) >> 8) \
                    | ((_x & (UINT64) 0xFF0000000000ULL) >> 24) \
                    | ((_x & (UINT64) 0xFF000000000000ULL) >> 40) \
                    | (_x >> 56)); \
})

#if (defined(__GNUC__) && (__GNUC__ > 3) && (__GNUC__ > 4 || __GNUC_MINOR__ >= 3)) || defined(__clang__)
static inline UINT32 grub_swap_bytes32(UINT32 x)
{
	return __builtin_bswap32(x);
}

static inline UINT64 grub_swap_bytes64(UINT64 x)
{
	return __builtin_bswap64(x);
}
#else					/* not gcc 4.3 or newer */
static inline UINT32 grub_swap_bytes32(UINT32 _x)
{
	return ((_x << 24)
		| ((_x & (UINT32)0xFF00UL) << 8)
		| ((_x & (UINT32)0xFF0000UL) >> 8)
		| (_x >> 24));
}

static inline UINT64 grub_swap_bytes64(UINT64 _x)
{
	return ((_x << 56)
		| ((_x & (UINT64)0xFF00ULL) << 40)
		| ((_x & (UINT64)0xFF0000ULL) << 24)
		| ((_x & (UINT64)0xFF000000ULL) << 8)
		| ((_x & (UINT64)0xFF00000000ULL) >> 8)
		| ((_x & (UINT64)0xFF0000000000ULL) >> 24)
		| ((_x & (UINT64)0xFF000000000000ULL) >> 40)
		| (_x >> 56));
}
#endif					/* not gcc 4.3 or newer */


#ifdef GRUB_CPU_WORDS_BIGENDIAN
# define grub_cpu_to_le16(x)	grub_swap_bytes16(x)
# define grub_cpu_to_le32(x)	grub_swap_bytes32(x)
# define grub_cpu_to_le64(x)	grub_swap_bytes64(x)
# define grub_le_to_cpu16(x)	grub_swap_bytes16(x)
# define grub_le_to_cpu32(x)	grub_swap_bytes32(x)
# define grub_le_to_cpu64(x)	grub_swap_bytes64(x)
# define grub_cpu_to_be16(x)	((UINT16) (x))
# define grub_cpu_to_be32(x)	((UINT32) (x))
# define grub_cpu_to_be64(x)	((UINT64) (x))
# define grub_be_to_cpu16(x)	((UINT16) (x))
# define grub_be_to_cpu32(x)	((UINT32) (x))
# define grub_be_to_cpu64(x)	((UINT64) (x))
# define grub_cpu_to_be16_compile_time(x)	((UINT16) (x))
# define grub_cpu_to_be32_compile_time(x)	((UINT32) (x))
# define grub_cpu_to_be64_compile_time(x)	((UINT64) (x))
# define grub_be_to_cpu64_compile_time(x)	((UINT64) (x))
# define grub_cpu_to_le32_compile_time(x)	grub_swap_bytes32_compile_time(x)
# define grub_cpu_to_le64_compile_time(x)	grub_swap_bytes64_compile_time(x)
# define grub_cpu_to_le16_compile_time(x)	grub_swap_bytes16_compile_time(x)
#else /* ! WORDS_BIGENDIAN */
# define grub_cpu_to_le16(x)	((UINT16) (x))
# define grub_cpu_to_le32(x)	((UINT32) (x))
# define grub_cpu_to_le64(x)	((UINT64) (x))
# define grub_le_to_cpu16(x)	((UINT16) (x))
# define grub_le_to_cpu32(x)	((UINT32) (x))
# define grub_le_to_cpu64(x)	((UINT64) (x))
# define grub_cpu_to_be16(x)	grub_swap_bytes16(x)
# define grub_cpu_to_be32(x)	grub_swap_bytes32(x)
# define grub_cpu_to_be64(x)	grub_swap_bytes64(x)
# define grub_be_to_cpu16(x)	grub_swap_bytes16(x)
# define grub_be_to_cpu32(x)	grub_swap_bytes32(x)
# define grub_be_to_cpu64(x)	grub_swap_bytes64(x)
# define grub_cpu_to_be16_compile_time(x)	grub_swap_bytes16_compile_time(x)
# define grub_cpu_to_be32_compile_time(x)	grub_swap_bytes32_compile_time(x)
# define grub_cpu_to_be64_compile_time(x)	grub_swap_bytes64_compile_time(x)
# define grub_be_to_cpu64_compile_time(x)	grub_swap_bytes64_compile_time(x)
# define grub_cpu_to_le16_compile_time(x)	((UINT16) (x))
# define grub_cpu_to_le32_compile_time(x)	((UINT32) (x))
# define grub_cpu_to_le64_compile_time(x)	((UINT64) (x))

#endif /* ! WORDS_BIGENDIAN */