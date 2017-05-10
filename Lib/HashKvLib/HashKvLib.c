
#include <EdkClass/Common.h>
#include <Lib/HashKVLib.h>

/* 简单的链式HASH实现 */


/* 获得键的Hash值 */

UINT16
EFIAPI
HkGetHashValue(
	IN	PHASH_KV	HashKv,
	IN	BYTE*		Key
) {

	return HashKv->MapProc(Key);

}

/* 生成键值对 */

VOID
EFIAPI
HkGenerateHashKvNode(
	IN	PHASH_KV	HashKv,
	IN	BYTE*		Key,
	IN	BYTE*		Value
) {

	UINT16 Hv = HkGetHashValue(HashKv, Key);
	PKV_PAIR Pair = (PKV_PAIR)AllocateZeroPool(sizeof(KV_PAIR));
	Pair->Next = NULL;
	Pair->Key = Key;
	Pair->Value = Value;

	if (HashKv->Heads[Hv] == NULL)
		HashKv->Heads[Hv] = Pair;
	else
	{
		PKV_PAIR First = HashKv->Heads[Hv];
		HashKv->Heads[Hv] = Pair;
		Pair->Next = First;
	}

}

/* 在列表中寻找键值对 */

PKV_PAIR
EFIAPI
HkFindPairInHashKv(
	IN	PHASH_KV	HashKv,
	IN	BYTE*		Key
) {

	UINT16 Hv = HkGetHashValue(HashKv, Key);
	PKV_PAIR Cur = HashKv->Heads[Hv];

	while (Cur != NULL)
	{
		if (HashKv->CmpProc(Cur->Key, Key))
			return Cur;
		
		Cur = Cur->Next;
	}

	return NULL;

}