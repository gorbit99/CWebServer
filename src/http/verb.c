#include "verb.h"

#include "../utils/Errors.h"

#include <stdlib.h>

HttpVerb verb_from_string(String *verb_string) {
    string_map(verb_string, string_map_tolower);

    if (string_cstrcmp(verb_string, "get") == 0) {
        return HttpGet;
    }
    if (string_cstrcmp(verb_string, "head") == 0) {
        return HttpHead;
    }
    if (string_cstrcmp(verb_string, "post") == 0) {
        return HttpPost;
    }
    if (string_cstrcmp(verb_string, "put") == 0) {
        return HttpPut;
    }
    if (string_cstrcmp(verb_string, "delete") == 0) {
        return HttpDelete;
    }
    if (string_cstrcmp(verb_string, "connect") == 0) {
        return HttpConnect;
    }
    if (string_cstrcmp(verb_string, "options") == 0) {
        return HttpOptions;
    }
    if (string_cstrcmp(verb_string, "trace") == 0) {
        return HttpTrace;
    }
    if (string_cstrcmp(verb_string, "patch") == 0) {
        return HttpPatch;
    }

    eprintf("Unknown http verb: %s!", string_as_cstr(verb_string));
    exit(-1);
}

String *verb_as_string(HttpVerb verb) {
    switch (verb) {
    case HttpGet: return string_from_cstr("GET");
    case HttpHead: return string_from_cstr("HEAD");
    case HttpPost: return string_from_cstr("POST");
    case HttpPut: return string_from_cstr("PUT");
    case HttpDelete: return string_from_cstr("DELETE");
    case HttpConnect: return string_from_cstr("CONNECT");
    case HttpOptions: return string_from_cstr("OPTIONS");
    case HttpTrace: return string_from_cstr("TRACE");
    case HttpPatch: return string_from_cstr("PATCH");
    default: eprintf("Unknown verb! (value: %u)\n", verb); exit(-1);
    }
}
