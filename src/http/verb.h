#pragma once

#include "../utils/string.h"

typedef enum HttpVerb {
    HttpGet,
    HttpHead,
    HttpPost,
    HttpPut,
    HttpDelete,
    HttpConnect,
    HttpOptions,
    HttpTrace,
    HttpPatch
} HttpVerb;

HttpVerb verb_from_string(String *verb_string);

String *verb_as_string(HttpVerb verb);
