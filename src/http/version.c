#include "version.h"

#include "../utils/Errors.h"

#include <stdlib.h>

HttpVersion version_from_string(String *version_string) {
    if (string_cstrcmp(version_string, "HTTP/1.0") == 0) {
        return Http10;
    }
    if (string_cstrcmp(version_string, "HTTP/1.1") == 0) {
        return Http11;
    }
    if (string_cstrcmp(version_string, "HTTP/2.0") == 0) {
        return Http20;
    }

    eprintf("HTTP Version string \"%s\" couldn't be parsed",
            string_as_cstr(version_string));
    exit(-1);
}

String *version_as_string(HttpVersion version) {
    switch (version) {
    case Http10: return string_from_cstr("HTTP/1.0");
    case Http11: return string_from_cstr("HTTP/1.1");
    case Http20: return string_from_cstr("HTTP/2.0");
    default:
        eprintf("Not a supported HTTP version! (value: %u)\n", version);
        exit(-1);
    }
}
