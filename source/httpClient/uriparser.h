//
// https://datatracker.ietf.org/doc/html/rfc3986
// RFC 3986 compliant URI parser
//

#ifndef URIPARSER_H
#define URIPARSER_H

#include "types.h"
#include "str.h"

#define MAX_SCHEME_LENGTH 32
#define MAX_USERINFO_LENGTH 256
#define MAX_HOST_LENGTH 256
#define MAX_PORT_LENGTH 6
#define MAX_PATH_LENGTH 2048
#define MAX_QUERY_LENGTH 2048
#define MAX_FRAGMENT_LENGTH 256

typedef struct {
    char Scheme[MAX_SCHEME_LENGTH];
    char UserInfo[MAX_USERINFO_LENGTH];
    char Host[MAX_HOST_LENGTH];
    char Port[MAX_PORT_LENGTH];
    char Path[MAX_PATH_LENGTH];
    char Query[MAX_QUERY_LENGTH];
    char Fragment[MAX_FRAGMENT_LENGTH];
} URI;

void uri_init(URI* uri);

int uri_parse(const char* uri_string, URI* uri);

int uri_validate(const URI* uri);

#endif //URIPARSER_H
