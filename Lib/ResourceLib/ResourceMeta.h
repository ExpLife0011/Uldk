#pragma once

typedef struct _RESOURCE_INSTANCE {

	EFI_HANDLE ImageHandle;

	LIST_ENTRY FontList;
	LIST_ENTRY ImageList;
	LIST_ENTRY StringList;

}RESOURCE_INSTANCE, *PRESOURCE_INSTANCE;