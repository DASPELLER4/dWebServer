#!/usr/bin/python3
import os
import mimetypes
import secrets
import string


endpointsh = """#ifndef ENDPOINTS_H
#define ENDPOINTS_H

#include "response.h"
#include "request.h"
#include "consts.h"
#include "defineendpoints.h"

"""
endpointsc = """#ifndef ENDPOINTS_C
#define ENDPOINTS_C

#include <stdio.h>
#include <stdlib.h>
        
#include "endpoints.h"
"""

entries = {}
fof = None
def travelFolder(path):
    global fof
    for entry in os.listdir(path):
        entry = path + entry
        if [x for x in ["generate.py", "endpoints.c", "endpoints.h", ".swp"] if x in entry]:
            continue
        if os.path.isdir(entry):
            travelFolder(entry + '/')
            continue
        if os.path.basename(entry) == 'index.html':
            entries[path[1:]] = entry
            entries[entry[1:]] = entry
        elif entry == './404.html':
            fof = entry
        else:
            entries[entry[1:]] = entry

travelFolder('./')
for entry in entries:
    # https://www.geeksforgeeks.org/python/python-generate-random-string-of-given-length/
    functionName = ''.join(secrets.choice(string.ascii_letters) for _ in range(10))
    mimetype, _ = mimetypes.guess_type(entries[entry])
    content = None
    with open(entries[entry], "rb") as File:
        content = File.read()
    if "text" in mimetype:
        content += b'\0'
    endpointsh += f"""pushEndpoints
#undef endpoints
#define endpoints popEndpoints endpoints addEndpoint({functionName}, "{entry}");
response_t {functionName}(request_t);
"""
    endpointsc += f"""response_t {functionName}(request_t request){{
    header_t *headers = newHeaders();
    appendHeader(headers, (keyvalue_t){{0, "Content-Type", "{mimetype}"}});
    appendHeader(headers, (keyvalue_t){{0, "Content-Length", "{len(content)}"}});
    static char data[{len(content)}] = {{{','.join(hex(x) for x in content)}}};
    return (response_t){{200, "OK", headers, data, {len(content)}}};
}}
"""

if fof:
    endpointsh += """#define is404
response_t fourohfour(request_t);
"""
    content = None
    with open(fof, "rb") as File:
        content = File.read()
    content += b'\0'
    endpointsc += f"""response_t fourohfour(request_t request){{
    header_t *headers = newHeaders();
    appendHeader(headers, (keyvalue_t){{0, "Content-Type", "text/html"}});
    appendHeader(headers, (keyvalue_t){{0, "Content-Length", "{len(content)}"}});
    static char data[{len(content)}] = {{{','.join(hex(x) for x in content)}}};
    return (response_t){{404, "Not Found", headers, data, {len(content)}}};
}}
"""

    

endpointsh += """
#endif
"""
endpointsc += """
#endif
"""

with open("endpoints.c","w") as File:
    File.write(endpointsc)

with open("endpoints.h","w") as File:
    File.write(endpointsh)
