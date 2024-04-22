#pragma once
#include "HiCodeCacheMappingDefs.h"

#ifdef GBAR3_HICODE_CACHE_MAPPING

extern u32 gHicodeUndefinedData[];

#ifdef __cplusplus
extern "C" {
#endif

void hic_unmapRomBlock(void);
void hic_initialize(void);

#ifdef __cplusplus
}
#endif

#endif
