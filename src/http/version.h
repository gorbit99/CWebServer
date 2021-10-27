#pragma once

#include "../utils/string.h"

typedef enum HttpVersion { Http10, Http11, Http20 } HttpVersion;

HttpVersion version_from_string(String *version_string);

String *version_as_string(HttpVersion version);
