/* Minimal cJSON stubs for parsing status JSON (replace with full cJSON if needed) */
#include "cJSON.h"
#include <stdlib.h>
#include <string.h>

/* NOTE: This is a placeholder. For full functionality, integrate official cJSON. */
cJSON *cJSON_Parse(const char *value) { (void)value; return NULL; }
void cJSON_Delete(cJSON *c) { (void)c; }
const cJSON *cJSON_GetObjectItemCaseSensitive(const cJSON *object, const char *string) { (void)object; (void)string; return NULL; }
