
#include "windows.h"

#define IS_COMMA(a)                ((a) == ',')
#define IS_HYPHEN(a)               ((a) == '-')
#define IS_DOT(a)                  ((a) == '.')
#define IS_LEFT_PARENTH(a)         ((a) == '(')
#define IS_RIGHT_PARENTH(a)        ((a) == ')')
#define IS_SLASH(a)                ((a) == '/')
#define IS_NULL(a)                 ((a) == '\0')

/* *:匹配任意字符 */
#define MATCH_ANY					1
/* str*:匹配以str开头的字符串 */
#define MATCH_HEAD					2
/* *str:匹配以str结尾的字符串 */
#define MATCH_TAIL					3
/* *str*:匹配出现str的字符串 */
#define MATCH_PARTICAL				4
/* str:匹配字符串str */
#define MATCH_ALL					5
/* 跳过子链匹配 */
#define MATCH_SKIP					6

#define MATCH_SELECT				7

typedef struct _MATCH_ELEMENT
{

	struct _MATCH_ELEMENT* Next;

	UINT8 ElementType;

	UINT8 MatchType;
	
	CHAR* MatchText;

	struct _MATCH_ELEMENT* SubList;

}MATCH_ELEMENT, *PMATCH_ELEMENT;

CHAR *
GetNextDeviceNodeStr(
	IN OUT CHAR		**DevicePath,
	OUT    BOOLEAN  *IsInstanceEnd
)
{
	CHAR	*Str;
	CHAR	*ReturnStr;
	UINT   ParenthesesStack;

	Str = *DevicePath;
	if (IS_NULL(*Str)) {
		return NULL;
	}

	//
	// Skip the leading '/', '(', ')' and ','
	//
	while (!IS_NULL(*Str)) {
		if (!IS_SLASH(*Str) &&
			!IS_COMMA(*Str)) {
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

static
CHAR*
SplitStr(
	IN OUT CHAR **List,
	IN     CHAR Separator
)
{
	CHAR  *Str;
	CHAR  *ReturnStr;

	Str = *List;
	ReturnStr = Str;

	if (IS_NULL(*Str)) {
		return NULL;
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

/*
对于表达式的编译，从匹配文本开始，每个类型节点生成一个ME类型节点
如果碰到[，则认为开始子表达式
*/

BOOL
IsMatchAll(char* node) {
	if (*node == '*') {
		return TRUE;
	}
	return FALSE;
}

BOOL
IsMatchSelect(char* node) {
	if (strchr(node, '|') != 0)
		return TRUE;
	return FALSE;
}

MATCH_ELEMENT*
Compiler(
	char* p
) {
	
	char* d = _strdup(p);
	char* node;
	BOOLEAN e;

	MATCH_ELEMENT* Head = NULL;
	MATCH_ELEMENT* LastElement = &Head;

	while ((node = GetNextDeviceNodeStr(&d, &e)) != NULL) {

		MATCH_ELEMENT* element = (MATCH_ELEMENT*)malloc(sizeof(*element));
		memset(element, 0, sizeof(*element));

		LastElement->Next = element;
		LastElement = element;

		if (IsMatchAll(node + 1) && (strcmp(node, "(*)") == 0)) {
			element->MatchType = MATCH_ANY;
			continue;
		}
		if (IsMatchSelect(node)) {
			element->MatchType = MATCH_SELECT;
			element->MatchText = SplitStr(&node, '(');
		}
		else {
			element->MatchType = MATCH_ALL;
			element->MatchText = SplitStr(&node, '(');
		}

		if (IsMatchAll(node) && (strcmp(node, "*)") == 0)) {

			MATCH_ELEMENT* sub_element = (MATCH_ELEMENT*)malloc(sizeof(*element));
			sub_element->MatchType = MATCH_SKIP;

			element->SubList = sub_element;
			continue;

		}

		char* para_Str = SplitStr(&node, ')');
		char* para;
		MATCH_ELEMENT* LastElement2 = element;

		while ((para = SplitStr(&para_Str, ',')) != NULL) {

			MATCH_ELEMENT* sub_element = (MATCH_ELEMENT*)malloc(sizeof(*element));
			memset(sub_element, 0, sizeof(*sub_element));

			LastElement2->Next = sub_element;

			if (IsMatchAll(para)) {
				sub_element->MatchType = MATCH_ANY;
				continue;
			}
			if (IsMatchSelect(para)) {
				sub_element->MatchType = MATCH_SELECT;
				sub_element->MatchText = para;
			}
			else {
				sub_element->MatchType = MATCH_ALL;
				sub_element->MatchText = para;
			}

		}

	}

	return Head;

}

BOOLEAN
MatchSelectPattern(
	char* node_name,
	char* pattern
) {

	char* pattern_str = _strdup(pattern);
	char* pattern_word;

	while ((pattern_word = SplitStr(&pattern_str, '|')) != NULL) {

		if (strcmp(pattern_word, node_name) == 0)
		{
			free(pattern_str);
			return TRUE;
		}

	}

	free(pattern_str);
	return FALSE;

}

BOOLEAN
MatchAllPattern(
	char* node_name,
	char* pattern
) {

	if (strcmp(node_name, pattern) == 0)
		return TRUE;
	return FALSE;

}

BOOLEAN
ComparePara(
	char* para_str,
	MATCH_ELEMENT* first_pattern
) {

	char* para = _strdup(para_str);
	BOOLEAN result = TRUE;

	MATCH_ELEMENT* cur_pattern = first_pattern;
	while (cur_pattern != NULL) {

		/* 模式未结束但是参数列表还没结束 */
		char* para_ele = SplitStr(&para, ',');
		if (para_ele == NULL) {
			result = FALSE;
			goto EXIT;
		}

		switch (cur_pattern->MatchType) {
		case MATCH_SELECT:

			if (!MatchSelectPattern(para_ele, cur_pattern->MatchText)) {
				result = FALSE;
				goto EXIT;
			}
			break;

		case MATCH_ALL:

			if (!MatchAllPattern(para_ele, cur_pattern->MatchText)) {
				result = FALSE;
				goto EXIT;
			}
			break;

		default:
			result = FALSE;
			goto EXIT;
		}

	}

EXIT:
	free(para);
	return result;

}

BOOLEAN
CompareNode(
	char* node_str,
	MATCH_ELEMENT* pattern_node
) {

	if (pattern_node->MatchType == MATCH_ANY)
		return FALSE;

	char *MatchText, *PatternWord;
	char *NodeName, *ParaStr, *Para;

	NodeName = SplitStr(&node_str, '(');

	switch (pattern_node->MatchType) {
	case MATCH_SELECT:

		if (!MatchSelectPattern(NodeName, pattern_node->MatchText)) {
			return FALSE;
		}
		break;

	case MATCH_ALL:

		if (!MatchAllPattern(NodeName, pattern_node->MatchText)) {
			return FALSE;
		}
		break;

	default:
		return FALSE;
	}

	if (pattern_node->SubList->MatchType == MATCH_SKIP)
		return TRUE;

	ParaStr = SplitStr(&NodeName, ')');

	return ComparePara(ParaStr, pattern_node->SubList);

}

BOOLEAN
Execute(
	char* dst,
	MATCH_ELEMENT* pattern
) {

	MATCH_ELEMENT* element = pattern;
	MATCH_ELEMENT* next_element;

	while (element != NULL) {

		switch (element->ElementType)
		{
		case MATCH_ANY:
			next_element = element->Next;

		}

	}

}

void main() {

	char tmp_pattern[] = "(*)/[usb|sata](*)/partition(1,*,3)/(*)";

	MATCH_ELEMENT* Mt = GenPattern(tmp_pattern);

}