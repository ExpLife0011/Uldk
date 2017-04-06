
#include <EdkClass/Common.h>
#include <Lib/XmlParserLib.h>
#include <Lib/HashKVLib.h>

#define ENCODE_UTF8		1
#define ENCODE_UTF16	2

static UINT8 Encode = 0;
static UINT8 Version = 0;

/*
�����XML��Ȼ���Ǳ�׼��xml�ļ�������ֻ�Ƕ����������������Լ�����ļ�ֵ���ļ�
<root>
	<firstchild attr1=value1 attr2=value2>
		an string
	</firstchild>
	<secondchild
		attr1 = value1 attr2=	value2>
		<annode ></annode>
	</secondchild>
</root>
*/

/*
UINT16
EFIAPI
TranslateAttrToValue(
	UINT16 OriValue,
	CHAR16* Value)
{

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
*/

/* �ͷŵ�����XML���� */

VOID
EFIAPI
XpFreeSection(
	XML_SECTION* CurSection
) {

	FreePool(CurSection->SectionName);

	for (UINTN i = 0;i < CurSection->AttrCount;i++)
	{
		FreePool(CurSection->Attributes[i]);
		FreePool(CurSection->Value[i]);
	}

	for (UINTN i = 0;i < CurSection->ChildrenCount;i++)
		XpFreeSection(CurSection->Children[i]);

	FreePool(CurSection);

}

/* �����հ׿����ַ� */

static
CHAR16*
XpSkipBlankChar(
	CHAR16* Start
)
{
	while ((*Start == L' ') ||
		(*Start == L'\r') ||
		(*Start == L'\n') ||
		(*Start == L'\t'))
	{
		Start++;
	}

	return Start;
}


//��ȡһ������
//start����ʼ
//next������+1
//spec���Ƿ��Ǳ�׼���ʻ������ⵥ�ʣ������������Ǳ�׼���ʣ�������ֵ�����ⵥ�ʣ�

static
CHAR16*
XpReadUtf16Token(
	CHAR16* Start,
	CHAR16** Next,
	BOOLEAN Spec
) {

	BOOLEAN Pair = 0;
	CHAR16* End = Start;

	/* �Ƿ����ַ��� */
	switch (*End)
	{
	case '\'':
		Pair = 1;
		break;
	case '\"':
		Pair = 2;
		break;
	default:
		Pair = 0;
		break;
	}

	/* �ӹ����� */
	if (Pair != 0)
		End++;

	CHAR16 Current = *End;

	/*
	while ((Current != L' ') &&
	(Current != L'\"') &&
	(Current != L'\'') &&
	(Current != L'<') &&
	(Current != L'>') &&
	(Current != L'=') &&
	(Current != L'/'))*/

	/* ����Ǳ�׼���أ���ֻ��������ַ���_ */
	if (Spec)
	{

		while ((Current >= L'0'&&Current <= L'9') ||
			(Current >= L'a'&&Current <= L'z') ||
			(Current >= L'A'&&Current <= L'Z') ||
			(Current == L'_'))
		{

			End++;
			Current = *End;

		}

	}
	/* ���Ǳ�׼������ֻ�ǲ����ܿ����ַ� */
	else {

		while ((Current != L' ') &&
			(Current != L'\"') &&
			(Current != L'\'') &&
			(Current != L'<') &&
			(Current != L'>') &&
			(Current != L'=') &&
			(Current != L'/'))
		{

			End++;
			Current = *End;

		}

	}

	/* �Ǹ��Ƿ����ſ�ͷ */
	if (Start == End)
		return NULL;

	/* �����ؿ������� */
	UINTN Size = (End - Start + 1) * sizeof(CHAR16);
	CHAR16* Result = (CHAR16*)AllocateZeroPool(Size);
	CopyMem(Result, Start, Size - 2);

	/* ָ����صĺ�һ������ */
	*Next = End++;

	return Result;

}

/* ��ȡ��ʼ��� */

static
BOOLEAN
XpReadSectionTitle(
	CHAR16* Section,
	XML_SECTION* NewSection,
	CHAR16** NextSection
) {

	int AttrCount = 0;
	CHAR16* Start = Section;

	/* �ӹ���ͷ�հ��ַ� */
	Start = XpSkipBlankChar(Start);

	/* ������<��ͷ */
	if (*Start != '<')
		return FALSE;

	Start++;
	CHAR16* Next;
	CHAR16* SectionName = XpReadUtf16Token(Start, &Next, FALSE);
	if (SectionName == NULL || StrLen(SectionName) == 0)
		return FALSE;

	CHAR16* Attr, *Value;
	NewSection->SectionName = SectionName;

	Next = XpSkipBlankChar(Next);

	while (*Next != L'>')
	{

		/* ��ȡ������ */
		Attr = XpReadUtf16Token(Next, &Next, TRUE);
		if (Attr == NULL || StrLen(Attr) == 0)
			return FALSE;

		/* ��ȡ=�� */
		Next = XpSkipBlankChar(Next);
		if (*Next != L'=')
			return FALSE;

		Next++;
		Next = XpSkipBlankChar(Next);

		/* ��ȡ����ֵ */
		Value = XpReadUtf16Token(Next, &Next, FALSE);
		if (Value == NULL || StrLen(Value) == 0)
			return FALSE;

		/* ֵ�������в����ַ� */
		if ((*Next != ' ') && (*Next != '>'))
			return FALSE;

		NewSection->Attributes[AttrCount] = Attr;
		NewSection->Value[AttrCount] = Value;
		AttrCount++;

		Next = XpSkipBlankChar(Next);

	}

	NewSection->AttrCount = AttrCount;
	Next++;
	*NextSection = Next;
	return TRUE;

}

/* ��ȡĩβ��� */

static
BOOLEAN
XpReadSectionEnd(
	CHAR16* SectionStr,
	XML_SECTION* Section,
	CHAR16** Next
) {

	CHAR16* Start = XpSkipBlankChar(SectionStr);
	if ((*Start != L'<') || (*(Start + 1) != L'/'))
		return FALSE;

	Start += 2;
	CHAR16* EndName = XpReadUtf16Token(Start, &Start, FALSE);
	if (StrCmp(EndName, Section->SectionName) != 0)
	{
		FreePool(EndName);
		return FALSE;
	}

	FreePool(EndName);

	Start = XpSkipBlankChar(Start);

	if (*Start != L'>')
		return FALSE;

	*Next = Start + 1;
	return TRUE;

}

/* ��ȡXMLͷ */
/* under developing */

static
BOOLEAN
XpReadHeader(
	CHAR16*SectionStr
) {

	CHAR16* Start = SectionStr;
	Start = XpSkipBlankChar(Start);

	if (StrnCmp(Start, L"<?xml", 5) != 0)
		return FALSE;

	Start += 5;

	CHAR16* Next;
	CHAR16* Attr, *Value;
	Next = XpSkipBlankChar(Next);

	while (*Next != L'/')
	{

		Attr = XpReadUtf16Token(Next, &Next, TRUE);
		if (Attr == NULL || StrLen(Attr) == 0)
			return FALSE;

		Next = XpSkipBlankChar(Next);
		if (*Next != L'=')
			return FALSE;

		Next++;
		Next = XpSkipBlankChar(Next);

		Value = XpReadUtf16Token(Next, &Next, FALSE);
		if (Value == NULL || StrLen(Value) == 0)
			return FALSE;

		if (StrCmp(Attr, L"encoding") == 0) {
			if (StrCmp(Value, L"utf-8") == 0)
				Encode = ENCODE_UTF8;
			if (StrCmp(Value, L"utf-16") == 0)
				Encode = ENCODE_UTF16;

		}

		FreePool(Attr);
		FreePool(Value);

		Next = XpSkipBlankChar(Next);

	}


}

/* ��ȡXML�� */
/* ��ʱֻ֧��UTF16�Ķ�ȡ */

BOOLEAN
EFIAPI
XpReadSection(
	CHAR16* SectionStr,
	XML_SECTION** Section,
	CHAR16** Next
) {

	/* ��һ�ν��� */
	if (Next == NULL)
	{
		Encode = 0;
		Version = 0;
	}

	XML_SECTION* NewSection = (XML_SECTION*)AllocateZeroPool(sizeof(XML_SECTION));
	NewSection->AttrHashKV.MapProc = Default_String_Map_Proc;
	NewSection->AttrHashKV.CmpProc = StrCmp;

	CHAR16* Iter = SectionStr;
	if (!XpReadSectionTitle(Iter, NewSection, &Iter))
	{
		FreePool(NewSection);
		return FALSE;
	}

#ifdef ENABLE_HASHKV
	for (UINT16 i = 0;i < NewSection->AttrCount;i++)
		HkGenerateHashKvNode(&NewSection->AttrHashKV, NewSection->Attributes[i], NewSection->Value[i]);
#endif

	/* ѭ����ȡ�ӽڵ� */

	PXML_SECTION ChildSection = NULL;
	int ChildCount = 0;
	while (XpReadSection(Iter, &ChildSection, &Iter))
	{

		ChildSection->Parent = NewSection;
		NewSection->Children[ChildCount] = ChildSection;
		ChildCount++;

	}

#ifdef ENABLE_HASHKV
	for (UINT16 i = 0;i < ChildCount;i++)
		GenerateHashKvNode(&NewSection->ChildrenHashKV, NewSection->Children[i]->SectionName, (CHAR16*)NewSection->Children[i]);
#endif

	if (ChildCount == 0)
	{

		/* �Ǹ��ı����ͽڵ㣬ѭ����ȡ��< */
		CHAR16* ContentEnd = Iter;
		while (*ContentEnd++ != L'<');

		/* �����ı� */
		CHAR16* Content = (CHAR16*)AllocatePool((ContentEnd - Iter) * 2);
		CopyMem(Content, Iter, (ContentEnd - Iter - 1) * 2);
		Content[ContentEnd - Iter - 1] = L'\0';
		NewSection->SectionContent = Content;

		Iter = ContentEnd;
		Iter--;

		NewSection->SectionType = SECTYPE_TEXT;

	}
	else {

		NewSection->SectionType = SECTYPE_NODE;

	}

	//�����β���Ϸ����ͷŷ���
	if (!XpReadSectionEnd(Iter, NewSection, &Iter))
	{
		XpFreeSection(NewSection);
		return FALSE;
	}
	NewSection->ChildrenCount = ChildCount;

	*Section = NewSection;
	if (Next)
		*Next = Iter;

	return TRUE;

}
