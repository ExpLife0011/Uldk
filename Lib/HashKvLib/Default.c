
#include <EdkClass/Common.h>
#include <Lib/HashKVLib.h>


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