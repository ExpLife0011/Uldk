
#include <EdkClass/Common.h>
#include <Lib/HashKvLib.h>

#include <Lib/EnvMgrLib.h>

static HASH_KV EnvHashKv;

static
CHAR16 *
EFIAPI
StrDup(
	IN      CONST CHAR16              *Source
) {

	UINTN Size = StrLen(Source);
	CHAR16* Dest = AllocatePool((Size + 1) * sizeof(CHAR16));
	StrCpyS(Dest, (Size + 1) * sizeof(CHAR16), Source);
	return Dest;

}

VOID
EFIAPI
EmInit() {

	SetMem(&EnvHashKv, sizeof(HASH_KV), 0);
	EnvHashKv.CmpProc = Default_String_Cmp_Proc;
	EnvHashKv.MapProc = Default_String_Map_Proc;

}

/* 保存一个键值对 */

VOID
EFIAPI
EmSetEnv(
	IN CHAR16* Name,
	IN VOID* Value,
	IN UINTN ValueLength
) {

	VOID* Vb = AllocatePool(ValueLength);
	CopyMem(Vb, Value, ValueLength);

	PKV_PAIR Pair = HkFindPairInHashKv(&EnvHashKv, (BYTE*)Name);
	if (Pair == NULL)
	{
		CHAR16* Name2 = StrDup(Name);
		HkGenerateHashKvNode(&EnvHashKv, (BYTE*)Name2, (BYTE*)Vb);
	}
	else {
		FreePool(Pair->Value);
		Pair->Value = Vb;
	}

}

/* 查询一个键值对 */

VOID*
EFIAPI
EmGetEnv(
	IN CHAR16* Name
) {

	PKV_PAIR Pair = HkFindPairInHashKv(&EnvHashKv, (BYTE*)Name);
	if (Pair == NULL)
	{
		return NULL;
	}
	else {
		return Pair->Value;
	}

}

VOID
EFIAPI
EmDestory(
) {

}