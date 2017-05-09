
#pragma once

#include <EdkClass/Common.h>
#include <ExtTypes.h>

#define ENABLE_HASHKV

#define SLOT_SIZE		8

typedef
UINT16
(EFIAPI* MAP_HASH_VALUE)(
	IN	BYTE*			Key
	);

typedef
BOOLEAN
(EFIAPI* CMP_VALUE)(
	IN	BYTE*			Src,
	IN	BYTE*			Dst
	);

typedef struct _KV_PAIR
{

	struct _KV_PAIR*	Next;
	BYTE*				Key;
	BYTE*				Value;


}KV_PAIR, *PKV_PAIR;

typedef struct _HASH_KV
{

	KV_PAIR*			Heads[SLOT_SIZE];
	UINTN				NodeCount[SLOT_SIZE];
	MAP_HASH_VALUE		MapProc;
	CMP_VALUE			CmpProc;

}HASH_KV, *PHASH_KV;

/* ��ü���Hashֵ */

UINT16
EFIAPI
HkGetHashValue(
	IN	PHASH_KV	HashKv,
	IN	BYTE*		Key
);

/* ���ɼ�ֵ�� */

VOID
EFIAPI
HkGenerateHashKvNode(
	IN	PHASH_KV	HashKv,
	IN	BYTE*		Key,
	IN	BYTE*		Value
);

/* ���б���Ѱ�Ҽ�ֵ�� */

PKV_PAIR
EFIAPI
HkFindPairInHashKv(
	IN	PHASH_KV	HashKv,
	IN	BYTE*		Key
);

/* һЩ���͵�Ĭ��Hash���� */

UINT16
EFIAPI
Default_String_Map_Proc(
	IN	BYTE*			Key
);

BOOLEAN
EFIAPI
Default_String_Cmp_Proc(
	IN	BYTE*			Src,
	IN	BYTE*			Dst
);