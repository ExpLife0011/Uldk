#pragma once

#include <EdkClass/Common.h>
#include <LwGUI/LwGuiLib.h>
#include <LwGUI/Component.h>
#include <LwGUI/Label.h>

typedef struct _GUI_IMAGE
{

	UINT16				Type;
	UINT16				Version;
	COMPONENT_ATTR		Attr;
	COMPONENT_OPS		Ops;

	CHAR16*				Text;
	UINT16				Alignment;
	BOOLEAN				Transparent;

}GUI_IMAGE, *PGUI_IMAGE;

PGUI_IMAGE
EFIAPI
CreateImage(
	IN		CHAR16*	Name
);