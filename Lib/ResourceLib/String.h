
#pragma once

#include <EdkClass/Common.h>
#include <Config.h>
#include <ExtTypes.h>

typedef struct _STRING_LIST_NODE {

	UINT16 StringId;
	UINT16 OffsetToContent;

}STRING_LIST_NODE, *PSTRING_LIST_NODE;