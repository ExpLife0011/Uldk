
#include <EdkClass/Common.h>
#include <EdkClass/Helper.h>

#include "DevicePathInternal.h"

/**

Duplicates a string.

@param  Src  Source string.

@return The duplicated string.

**/
static
CHAR16 *
DpLibStrDuplicate(
	IN CONST CHAR16  *Src
)
{
	return AllocateCopyPool(StrSize(Src), Src);
}

/**

Get parameter in a pair of parentheses follow the given node name.
For example, given the "Pci(0,1)" and NodeName "Pci", it returns "0,1".

@param  Str      Device Path Text.
@param  NodeName Name of the node.

@return Parameter text for the node.

**/
static
CHAR16 *
GetParamByNodeName(
	IN CHAR16 *Str,
	IN CHAR16 *NodeName
)
{
	CHAR16  *ParamStr;
	CHAR16  *StrPointer;
	UINTN   NodeNameLength;
	UINTN   ParameterLength;

	//
	// Check whether the node name matchs
	//
	NodeNameLength = StrLen(NodeName);
	if (StrnCmp(Str, NodeName, NodeNameLength) != 0) {
		return NULL;
	}

	ParamStr = Str + NodeNameLength;
	if (!IS_LEFT_PARENTH(*ParamStr)) {
		return NULL;
	}

	//
	// Skip the found '(' and find first occurrence of ')'
	//
	ParamStr++;
	ParameterLength = 0;
	StrPointer = ParamStr;
	while (!IS_NULL(*StrPointer)) {
		if (IS_RIGHT_PARENTH(*StrPointer)) {
			break;
		}
		StrPointer++;
		ParameterLength++;
	}
	if (IS_NULL(*StrPointer)) {
		//
		// ')' not found
		//
		return NULL;
	}

	ParamStr = AllocateCopyPool((ParameterLength + 1) * sizeof(CHAR16), ParamStr);
	if (ParamStr == NULL) {
		return NULL;
	}
	//
	// Terminate the parameter string
	//
	ParamStr[ParameterLength] = L'\0';

	return ParamStr;
}

/**
Gets current sub-string from a string list, before return
the list header is moved to next sub-string. The sub-string is separated
by the specified character. For example, the separator is ',', the string
list is "2,0,3", it returns "2", the remain list move to "0,3"

@param  List        A string list separated by the specified separator
@param  Separator   The separator character

@return A pointer to the current sub-string

**/
static
CHAR16 *
SplitStr(
	IN OUT CHAR16 **List,
	IN     CHAR16 Separator
)
{
	CHAR16  *Str;
	CHAR16  *ReturnStr;

	Str = *List;
	ReturnStr = Str;

	if (IS_NULL(*Str)) {
		return ReturnStr;
	}

	//
	// Find first occurrence of the separator
	//
	while (!IS_NULL(*Str)) {
		if (*Str == Separator) {
			break;
		}
		Str++;
	}

	if (*Str == Separator) {
		//
		// Find a sub-string, terminate it
		//
		*Str = L'\0';
		Str++;
	}

	//
	// Move to next sub-string
	//
	*List = Str;

	return ReturnStr;
}

/**
Gets the next parameter string from the list.

@param List            A string list separated by the specified separator

@return A pointer to the current sub-string

**/
static
CHAR16 *
GetNextParamStr(
	IN OUT CHAR16 **List
)
{
	//
	// The separator is comma
	//
	return SplitStr(List, L',');
}

/**
Get one device node from entire device path text.

@param DevicePath      On input, the current Device Path node; on output, the next device path node
@param IsInstanceEnd   This node is the end of a device path instance

@return A device node text or NULL if no more device node available

**/
CHAR16 *
GetNextDeviceNodeStr(
	IN OUT CHAR16   **DevicePath,
	OUT    BOOLEAN  *IsInstanceEnd
)
{
	CHAR16  *Str;
	CHAR16  *ReturnStr;
	UINTN   ParenthesesStack;

	Str = *DevicePath;
	if (IS_NULL(*Str)) {
		return NULL;
	}

	//
	// Skip the leading '/', '(', ')' and ','
	//
	while (!IS_NULL(*Str)) {
		if (!IS_SLASH(*Str) &&
			!IS_COMMA(*Str) &&
			!IS_LEFT_PARENTH(*Str) &&
			!IS_RIGHT_PARENTH(*Str)) {
			break;
		}
		Str++;
	}

	ReturnStr = Str;

	//
	// Scan for the separator of this device node, '/' or ','
	//
	ParenthesesStack = 0;
	while (!IS_NULL(*Str)) {
		if ((IS_COMMA(*Str) || IS_SLASH(*Str)) && (ParenthesesStack == 0)) {
			break;
		}

		if (IS_LEFT_PARENTH(*Str)) {
			ParenthesesStack++;
		}
		else if (IS_RIGHT_PARENTH(*Str)) {
			ParenthesesStack--;
		}

		Str++;
	}

	if (ParenthesesStack != 0) {
		//
		// The '(' doesn't pair with ')', invalid device path text
		//
		return NULL;
	}

	if (IS_COMMA(*Str)) {
		*IsInstanceEnd = TRUE;
		*Str = L'\0';
		Str++;
	}
	else {
		*IsInstanceEnd = FALSE;
		if (!IS_NULL(*Str)) {
			*Str = L'\0';
			Str++;
		}
	}

	*DevicePath = Str;

	return ReturnStr;
}

/*
judge if the current node is full match node
*/

static
BOOLEAN
IsFullMatchNode(
	IN CHAR16** Patter
) {

	CHAR16* Str = *Patter;
	if (!IS_LEFT_PARENTH(*Str))
		return FALSE;

	Str++;
	if (*Str != L'*')
		return FALSE;

	Str++;
	if (!IS_RIGHT_PARENTH(*Str))
		return FALSE;

	*Patter = Str + 1;
	return TRUE;

}

UINTN 
EFIAPI
DpLibGetDevicePathSize(
	IN EFI_DEVICE_PATH_PROTOCOL* DevPath
) {

	return GetDevicePathSize(DevPath);

}

EFI_DEVICE_PATH_PROTOCOL*
EFIAPI
DpLibDuplicateDevicePath(
	IN EFI_DEVICE_PATH_PROTOCOL* DevPath
) {

	return DuplicateDevicePath(DevPath);

}

EFI_DEVICE_PATH_PROTOCOL*
EFIAPI
DpLibFindSpecificNode(
	IN EFI_DEVICE_PATH_PROTOCOL*	DevPath,
	IN UINT8						Type,
	IN UINT8						SubType,
	OUT EFI_DEVICE_PATH_PROTOCOL*	SubPath
) {

	EFI_DEVICE_PATH_PROTOCOL* DpNode = DevPath;

	while (!IsDevicePathEnd(DpNode)) {

		if ((DevicePathType(DpNode) == Type) &&
			(DevicePathSubType(DpNode) == SubType))
		{
			return DpNode;
		}

		DpNode = NextDevicePathNode(DpNode);

	}
	
	return NULL;

}

#define MATCH_ANY		1
#define MATCH_NODE		2
#define MATCH_ALL		3

EFI_STATUS
EFIAPI
DpLibFilterDevicePath(
	IN CHAR16* MatchExp
) {

	EFI_DEVICE_PATH_PROTOCOL* DevicePath;
	CHAR16 *DevicePathStr,*DeviceNodeStr,*PatternNodeStr,*PatternStr;
	EFI_HANDLE* Handles, *NewHandles;
	UINTN Count, NewCount;
	UINT8 PatternType;
	BOOLEAN InstanceEnd1, InstanceEnd2;

	gBS->LocateHandleBuffer(ByProtocol, &gEfiDevicePathProtocolGuid, NULL, &Count, &Handles);
	
	NewHandles = AllocateZeroPool(4 * 1024);
	NewCount = 0;

	for (UINTN i = 0;i < Count;i++) {

		DevicePath = DevicePathFromHandle(Handles[i]);
		if (DevicePath == NULL) continue;

		DevicePathStr = ConvertDevicePathToText(DevicePath, TRUE, TRUE);
		DeviceNodeStr = DevicePathStr;

		PatternStr = DpLibStrDuplicate(MatchExp);
		PatternNodeStr = PatternStr;

		while (PatternNodeStr != NULL) {

			if (IsFullMatchNode(&PatternStr)) {

				PatternType = MATCH_ALL;

			}
			else {

				/* Get node string */
				PatternNodeStr = GetNextDeviceNodeStr(&PatternStr, &InstanceEnd1);
				DeviceNodeStr = GetNextDeviceNodeStr(&DevicePathStr, &InstanceEnd2);

				/*
				if devicepath ends and patterpath doesn't,
				judge if next patter node is full match
				*/
				if ((DeviceNodeStr == NULL) && (PatternNodeStr != NULL)) {

					if (!IsFullMatchNode(&PatternStr))
						goto EXIT_FOR;
					else {
						NewHandles[NewCount] = Handles[i];
						NewCount++;
						goto EXIT_FOR;
					}

				}
				/*
				all ends
				*/
				else if ((DeviceNodeStr == NULL) && (PatternNodeStr == NULL)) {
					NewHandles[NewCount] = Handles[i];
					NewCount++;
					goto EXIT_FOR;
				}
				/*
				not match obviously
				*/
				else {
					goto EXIT_FOR;
				}

				/* compare the node name : NodeName*/
				if (StrCmp(
					SplitStr(&PatternNodeStr, L'('), SplitStr(DeviceNodeStr, L'(') != 0)
					)
					goto EXIT_FOR;

				/* illegal expression format : NodeName()*/
				if (*PatternNodeStr == ')')
					return EFI_NOT_FOUND;

				/* any node content matchs : NodeName(*) */
				if (*PatternNodeStr == L'*') {
					if (*(PatternNodeStr + 1) == L')') {
						continue;
					}
					/* illegal expression format */
					else if (!IS_COMMA(*(PatternNodeStr + 1)))
						return EFI_NOT_FOUND;

				}
			}

			CHAR16* PatternNextPara, *DevicePathNextPara;
			while (1) {

				PatternNextPara = SplitStr(&PatternNodeStr, ',');
				DevicePathNextPara = SplitStr(&DeviceNodeStr, ',');

				if (PatternNextPara == L'*')
				{
					continue;
				}
				else {
					if (StrCmp(PatternNextPara, DevicePathNextPara) != 0)
						goto EXIT_FOR;
				}


			}


		}

		EXIT_FOR:

	}

}