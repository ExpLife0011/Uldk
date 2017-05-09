
#include <EdkClass/Common.h>

#define IS_COMMA(a)                ((a) == L',')
#define IS_HYPHEN(a)               ((a) == L'-')
#define IS_DOT(a)                  ((a) == L'.')
#define IS_LEFT_PARENTH(a)         ((a) == L'(')
#define IS_RIGHT_PARENTH(a)        ((a) == L')')
#define IS_SLASH(a)                ((a) == L'/')
#define IS_NULL(a)                 ((a) == L'\0')

/* *:ƥ�������ַ� */
#define MATCH_ANY					1
/* str*:ƥ����str��ͷ���ַ��� */
#define MATCH_HEAD					2
/* *str:ƥ����str��β���ַ��� */
#define MATCH_TAIL					3
/* *str*:ƥ�����str���ַ��� */
#define MATCH_PARTICAL				4
/* str:ƥ���ַ���str */
#define MATCH_ALL					5

#define MATCH_REPEAT_ANY			6
#define MATCH_REPEAT_ONCE			7
MEDIA_DEVICE_PATH
typedef struct _MATCH_ELEMENT {

	UINT8 MatchType;
	CHAR16* MatchText;

}MATCH_ELEMENT, *PMATCH_ELEMENT;