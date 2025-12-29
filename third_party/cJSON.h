/* cJSON single-file header (trimmed) */
#ifndef CJSON_H
#define CJSON_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct cJSON {
    struct cJSON *next,*prev;
    struct cJSON *child;
    int type;
    char *valuestring;
    int valueint;
    double valuedouble;
    char *string;
} cJSON;

cJSON *cJSON_Parse(const char *value);
void cJSON_Delete(cJSON *c);
const cJSON *cJSON_GetObjectItemCaseSensitive(const cJSON *object, const char *string);

#ifdef __cplusplus
}
#endif
#endif
