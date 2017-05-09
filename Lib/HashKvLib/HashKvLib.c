
#include <EdkClass/Common.h>
#include <Lib/HashKVLib.h>

/* �򵥵���ʽHASHʵ�� */

/* Ĭ���ַ���Hash���� */

UINT16
EFIAPI
Default_String_Map_Proc(
	IN	BYTE*			Key
) {

	CHAR16* String = (CHAR16*)Key;
	UINT16 Len = (UINT16)StrLen(String);
	UINT16 V = 0;

	for (UINT16 i = 0;i < Len;i++)
		V += String[i];
	
	return V % SLOT_SIZE;

}

/* Ĭ���ַ����ȽϺ��� */

BOOLEAN
EFIAPI
Default_String_Cmp_Proc(
	IN	BYTE*			Src,
	IN	BYTE*			Dst
) {

	return (StrCmp((CHAR16*)Src, (CHAR16*)Dst) == 0);

}

/* ��ü���Hashֵ */

UINT16
EFIAPI
HkGetHashValue(
	IN	PHASH_KV	HashKv,
	IN	BYTE*		Key
) {

	return HashKv->MapProc(Key);

}

/* ���ɼ�ֵ�� */

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

/* ���б���Ѱ�Ҽ�ֵ�� */

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