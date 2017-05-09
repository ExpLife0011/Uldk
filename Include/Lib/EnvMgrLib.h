
#include <EdkClass/Common.h>
#include <EdkClass/MediaAccess.h>
#include <EdkClass/NetProtocolStack.h>

BOOLEAN
EFIAPI
EmIsPxeBoot(
);

VOID
EFIAPI
EmInit(
);

VOID
EFIAPI
EmSetEnv(
	IN CHAR16* Name,
	IN VOID* Value,
	IN UINTN ValueLength
);

VOID*
EFIAPI
EmGetEnv(
	IN CHAR16* Name
);