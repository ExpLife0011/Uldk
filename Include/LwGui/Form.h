
#include <EdkClass/Common.h>
#include <LwGUI/LwGuiLib.h>
#include <LwGUI/Component.h>
#include <Lib/VideoLib.h>

typedef BOOLEAN(*IterChildCallback)(
	VOID* Parent,
	VOID* Child
	);

typedef struct _GUI_FORM
{

	// Ù–‘
	UINT16				Type;
	UINT16				Version;
	COMPONENT_ATTR		Attr;
	COMPONENT_OPS		Ops;

	EFI_GRAPHICS_OUTPUT_BLT_PIXEL FgColor;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL BgColor;

	EFI_HANDLE					SwapObject;
	HASH_KV						ComponentList;
	UINTN						Timeout;

	COMPONENT_COMMON_HEADER*	CurrentFocus;

	LIST_ENTRY					ChildrenList;
	UINTN						ChildrenCount;

	//∑Ω∑®
	VOID
	(EFIAPI* AddChild)(
		IN VOID* vSelf,
		IN COMPONENT_COMMON_HEADER* Child
		);

	VOID
	(EFIAPI* IterChildren)(
		IN VOID* vSelf, 
		IN IterChildCallback IterCallBack
		);

	VOID
	(EFIAPI* SetColor)(
		IN VOID* vSelf,
		IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL* BgColor,
		IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL* FgColor
		);

	PCOMPONENT_COMMON_HEADER
	(EFIAPI* NextForm)(
		IN	VOID*		vSelf,
		IN	CHAR16*		FormName
		);

	PCOMPONENT_COMMON_HEADER
	(EFIAPI* FindComponent)(
		IN VOID*		vSelf,
		IN CHAR16*		Name
		);

}GUI_FORM, *PGUI_FORM;

PGUI_FORM
EFIAPI
CreateForm(
	IN CHAR16* Name
);

PGUI_FORM
EFIAPI
CreateFormFromXML(
	IN XML_SECTION* XML
);