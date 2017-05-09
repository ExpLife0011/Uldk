
#pragma once

#include <EdkClass/Common.h>
#include <LwGUI/LwGuiLib.h>
#include <LwGUI/Component.h>
#include <LwGUI/Label.h>

typedef struct _GUI_LABEL
{

	UINT16				Type;
	UINT16				Version;
	COMPONENT_ATTR		Attr;
	COMPONENT_OPS		Ops;

	CHAR16*				Text;
	UINT16				Alignment;
	BOOLEAN				Transparent;

	VOID(EFIAPI* SetText)(
		IN	VOID*		vSelf,
		IN	CHAR16*		Text
		);

	VOID(EFIAPI* SetTransparent)(
		IN	VOID*		vSelf,
		IN	BOOLEAN		Transparent
		);

}GUI_LABEL, *PGUI_LABEL;

PGUI_LABEL
EFIAPI
CreateLabel(
	IN		CHAR16*	Name
);