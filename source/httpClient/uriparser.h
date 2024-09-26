//
// Created by Alan on 26/09/2024.
//

#ifndef URIPARSER_H
#define URIPARSER_H

#include "types.h"
#include "str.h"

#define MAX_SCHEME_LENGTH 32
#define MAX_HOST_LENGTH 255
#define MAX_PATH_LENGTH 2048
#define MAX_QUERY_LENGTH 2048
#define MAX_FRAGMENT_LENGTH 512

typedef struct {
    char SCHEME[MAX_SCHEME_LENGTH];
    char HOST[MAX_HOST_LENGTH];
    uint16_t PORT;
    char PATH[MAX_PATH_LENGTH];
    char QUERY[MAX_QUERY_LENGTH];
    char FRAGMENT[MAX_FRAGMENT_LENGTH];
} URI;

void uri_init(URI* uri);

int uri_parse(const char* uri_string, URI* uri);

int uri_validate(const URI* uri);

#endif //URIPARSER_H
