#ifndef OCGAPI_H
#define OCGAPI_H
#include "ocgapi_types.h"

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

#if defined(WIN32) && defined(OCGCORE_EXPORT_FUNCTIONS)
#define OCGAPI EXTERN_C __declspec(dllexport)
#else
#define OCGAPI EXTERN_C
#endif

/*** CORE INFORMATION ***/
OCGAPI void OCG_GetVersion(int* major, int* minor);
/* OCGAPI void OCG_GetName(const char** name); Maybe created by git hash? */

/*** DUEL CREATION AND DESTRUCTION ***/
OCGAPI int OCG_CreateDuel(OCG_Duel* duel, OCG_DuelOptions options);
OCGAPI void OCG_DestroyDuel(OCG_Duel duel);
OCGAPI void OCG_DuelNewCard(OCG_Duel duel, OCG_NewCardInfo info);
OCGAPI void OCG_StartDuel(OCG_Duel duel);

/*** DUEL PROCESSING AND QUERYING ***/
OCGAPI int OCG_DuelProcess(OCG_Duel duel);
OCGAPI void* OCG_DuelGetMessage(OCG_Duel duel, uint32_t* length);
OCGAPI void OCG_DuelSetResponse(OCG_Duel duel, const void* buffer, uint32_t length);
OCGAPI int OCG_LoadScript(OCG_Duel duel, const char* buffer, uint32_t length, const char* name);

OCGAPI uint32_t OCG_DuelQueryCount(OCG_Duel duel, uint8_t team, uint32_t loc);
OCGAPI void* OCG_DuelQuery(OCG_Duel duel, uint32_t* length, OCG_QueryInfo info);
OCGAPI void* OCG_DuelQueryLocation(OCG_Duel duel, uint32_t* length, OCG_QueryInfo info);
OCGAPI void* OCG_DuelQueryField(OCG_Duel duel, uint32_t* length);

#endif /* OCGAPI_H */
