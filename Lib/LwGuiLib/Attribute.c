
#include <EdkClass/Common.h>
#include <EdkClass/MediaAccess.h>

#include <LwGui/LwGuiLib.h>


BOOLEAN
EFIAPI
AttrReadColor(
	IN	CHAR16*		Value,
	IN	EFI_GRAPHICS_OUTPUT_BLT_PIXEL* Color
) {

	if ((StrLen(Value) != 6) && (StrLen(Value) != 8))
		return FALSE;

	CHAR16 R[3] = L"00";
	CHAR16 G[3] = L"00";
	CHAR16 B[3] = L"00";
	CHAR16 A[3] = L"00";

	CopyMem(R, Value, 2);
	CopyMem(G, Value + 2, 2);
	CopyMem(B, Value + 4, 2);

	if (StrLen(Value) == 8)
		CopyMem(A, Value + 6, 2);

	Color->Red = (UINT8)StrHexToUint64(R);
	Color->Green = (UINT8)StrHexToUint64(G);
	Color->Blue = (UINT8)StrHexToUint64(B);
	Color->Reserved = (UINT8)StrHexToUint64(A);

	return TRUE;

}

UINT16
EFIAPI
AttrReadAlignment(
	IN	CHAR16*		Value
) {

	if (StrCmp(Value, L"Left") == 0) {
		return GUI_ALIGN_LEFT;
	}
	else if (StrCmp(Value, L"Center") == 0) {
		return GUI_ALIGN_CENTER;
	}
	else if (StrCmp(Value, L"Right") == 0) {
		return GUI_ALIGN_RIGHT;
	}

	return GUI_ALIGN_LEFT;

}

UINT16
EFIAPI
AttrReadValue(
	UINT16 OriValue,
	CHAR16* Value
) {

	BOOLEAN Percent = FALSE;

	if (Value[StrLen(Value) - 1] == L'%')
		Percent = TRUE;

	if (!Percent)
		return (UINT16)StrDecimalToUintn(Value);
	else
	{
		Value[StrLen(Value) - 1] = 0;

		UINTN PctValue = StrDecimalToUintn(Value);
		return (UINT16)(OriValue*PctValue / 100);
	}

}
