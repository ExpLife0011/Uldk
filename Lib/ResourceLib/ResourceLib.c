
#include <EdkClass/Common.h>
#include <Lib/ResourceLib.h>

#include "ResourceMeta.h"

EFI_HANDLE
EFIAPI
ResLibCreateResource(
	IN EFI_HANDLE ImageHandle
) {

	PRESOURCE_INSTANCE ResInst = (PRESOURCE_INSTANCE)AllocateZeroPool(sizeof(*ResInst));
	if (ResInst == NULL)
		return NULL;

	InitializeListHead(&ResInst->FontList);
	InitializeListHead(&ResInst->ImageList);
	InitializeListHead(&ResInst->StringList);

	ResInst->ImageHandle = ImageHandle;

	return(EFI_HANDLE)ResInst;

}