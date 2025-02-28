//
// https://datatracker.ietf.org/doc/html/rfc3986
// RFC 3986 compliant URI parser
//

#ifndef URIPARSER_H
#define URIPARSER_H

#include "../types.h"
#include "../str.h"

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
int uri_parse(const char* uri_string, URI* uri); // parse a URI string into a URI struct
int uri_validate(const URI* uri); // validate a URI struct

// getters
const char* uri_get_scheme(const URI* uri);
const char* uri_get_userinfo(const URI* uri);
const char* uri_get_host(const URI* uri);
const char* uri_get_port(const URI* uri);
const char* uri_get_path(const URI* uri);
const char* uri_get_query(const URI* uri);
const char* uri_get_fragment(const URI* uri);

// util
int uri_is_absolute(const URI* uri);
int uri_is_relative(const URI* uri);

// debug
int uri_to_string(const URI* uri, char* buffer, size_t buffer_size);

#endif //URIPARSER_H