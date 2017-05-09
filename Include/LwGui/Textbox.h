
#pragma once

#include <EdkClass/Common.h>
#include <LwGUI/Component.h>
#include <LwGUI/TextBox.h>

typedef struct _GUI_TEXT
{

	UINT16				Type;
	UINT16				Version;
	COMPONENT_ATTR		Attr;
	COMPONENT_OPS		Ops;

	CHAR16				Text[128];

	UINTN				TextPos;
	BOOLEAN				Cursor;

	VOID
	(EFIAPI* SetText)(
		IN		VOID* vSelf,
		IN		CHAR16* Text
		);

	CHAR16*
	(EFIAPI* GetText)(
		IN		VOID* vSelf
		);

}GUI_TEXT, *PGUI_TEXT;

PGUI_TEXT
EFIAPI
CreateTextBox(
	IN		CHAR16*	Name
);
