//
// https://datatracker.ietf.org/doc/html/rfc3986
//

#include <stddef.h>

#include "uriparser.h"

void uri_init(URI* uri) {
    if (uri) {
        memset(uri, 0, sizeof(URI));
    }
}

static int is_scheme_char(char c) {
    return isalnum(c) || c == '+' || c == '-' || c == '.';
}

static int parse_scheme(const char** uri_string, char* scheme) {
    const char* start = *uri_string;
    while (is_scheme_char(**uri_string)) {
        (*uri_string)++;
    }

    if (**uri_string == ':') {
        size_t length = *uri_string - start;
        if (length < MAX_SCHEME_LENGTH) {
            strncpy(scheme, start, length);
            scheme[length] = '\0';
            (*uri_string)++;  // skip ':'
            return 1;
        }
    }

    *uri_string = start;
    return 0;
}

static int parse_authority(const char** uri_string, char* userinfo, char* host, char* port) {
    if ((*uri_string)[0] != '/' || (*uri_string)[1] != '/') {
        return 0;
    }
    *uri_string += 2;

    const char* authority_start = *uri_string;
    const char* userinfo_end = strchr(authority_start, '@');
    const char* host_end = strpbrk(userinfo_end ? userinfo_end + 1 : authority_start, ":/");

    if (userinfo_end && userinfo_end < host_end) {
        size_t userinfo_length = userinfo_end - authority_start;
        if (userinfo_length < MAX_USERINFO_LENGTH) {
            strncpy(userinfo, authority_start, userinfo_length);
            userinfo[userinfo_length] = '\0';
        }
        authority_start = userinfo_end + 1;
    }

    if (host_end) {
        size_t host_length = host_end - authority_start;
        if (host_length < MAX_HOST_LENGTH) {
            strncpy(host, authority_start, host_length);
            host[host_length] = '\0';
        }

        if (*host_end == ':') {
            const char* port_start = host_end + 1;
            const char* port_end = strpbrk(port_start, "/?#");
            if (!port_end) port_end = port_start + strlen(port_start);

            size_t port_length = port_end - port_start;
            if (port_length < MAX_PORT_LENGTH) {
                strncpy(port, port_start, port_length);
                port[port_length] = '\0';
            }
            *uri_string = port_end;
        } else {
            *uri_string = host_end;
        }
    } else {
        strcpy(host, authority_start);
        *uri_string += strlen(authority_start);
    }

    return 1;
}

static void parse_path(const char** uri_string, char* path) {
    const char* end = strpbrk(*uri_string, "?#");
    if (!end) end = *uri_string + strlen(*uri_string);

    size_t length = end - *uri_string;
    if (length < MAX_PATH_LENGTH) {
        strncpy(path, *uri_string, length);
        path[length] = '\0';
    }
    *uri_string = end;
}

static void parse_query(const char** uri_string, char* query) {
    if (**uri_string == '?') {
        (*uri_string)++;
        const char* end = strchr(*uri_string, '#');
        if (!end) end = *uri_string + strlen(*uri_string);

        size_t length = end - *uri_string;
        if (length < MAX_QUERY_LENGTH) {
            strncpy(query, *uri_string, length);
            query[length] = '\0';
        }
        *uri_string = end;
    }
}

static void parse_fragment(const char** uri_string, char* fragment) {
    if (**uri_string == '#') {
        (*uri_string)++;
        size_t length = strlen(*uri_string);
        if (length < MAX_FRAGMENT_LENGTH) {
            strcpy(fragment, *uri_string);
        }
    }
}

int uri_parse(const char* uri_string, URI* uri) {
    if (!uri_string || !uri) {
        return 0;
    }

    uri_init(uri);

    const char* current = uri_string;

    if (parse_scheme(&current, uri->Scheme)) {
        if (parse_authority(&current, uri->UserInfo, uri->Host, uri->Port)) {
            parse_path(&current, uri->Path);
        } else {
            return 0;
        }
    } else {
        parse_path(&current, uri->Path);
    }

    parse_query(&current, uri->Query);
    parse_fragment(&current, uri->Fragment);

    return 1;
}

int uri_validate(const URI* uri) {
    if (!uri) {
        return 0;
    }

    if (uri->Scheme[0] && !uri->Host[0]) {
        return 0;  // Scheme present but no host
    }

    // TODO: More validation checks

    return 1;  // URI is valid
}

const char* uri_get_scheme(const URI* uri) {
    return uri ? uri->Scheme : NULL;
}

const char* uri_get_userinfo(const URI* uri) {
    return uri ? uri->UserInfo : NULL;
}

const char* uri_get_host(const URI* uri) {
    return uri ? uri->Host : NULL;
}

const char* uri_get_port(const URI* uri) {
    return uri ? uri->Port : NULL;
}

const char* uri_get_path(const URI* uri) {
    return uri ? uri->Path : NULL;
}

const char* uri_get_query(const URI* uri) {
    return uri ? uri->Query : NULL;
}

const char* uri_get_fragment(const URI* uri) {
    return uri ? uri->Fragment : NULL;
}

int uri_is_absolute(const URI* uri) {
    return uri && uri->Scheme[0] != '\0';
}

int uri_is_relative(const URI* uri) {
    return uri && uri->Scheme[0] == '\0';
}

int uri_to_string(const URI* uri, char* buffer, size_t buffer_size) {
    if (!uri || !buffer || buffer_size == 0) {
        return 0;
    }

    size_t written = 0;

    // Scheme
    if (uri->Scheme[0]) {
        written += snprintf(buffer + written, buffer_size - written, "%s:", uri->Scheme);
    }

    // Authority
    if (uri->Host[0]) {
        written += snprintf(buffer + written, buffer_size - written, "//");
        if (uri->UserInfo[0]) {
            written += snprintf(buffer + written, buffer_size - written, "%s@", uri->UserInfo);
        }
        written += snprintf(buffer + written, buffer_size - written, "%s", uri->Host);
        if (uri->Port[0]) {
            written += snprintf(buffer + written, buffer_size - written, ":%s", uri->Port);
        }
    }

    // Path
    written += snprintf(buffer + written, buffer_size - written, "%s", uri->Path);

    // Query
    if (uri->Query[0]) {
        written += snprintf(buffer + written, buffer_size - written, "?%s", uri->Query);
    }

    // Fragment
    if (uri->Fragment[0]) {
        written += snprintf(buffer + written, buffer_size - written, "#%s", uri->Fragment);
    }

    return (written < buffer_size) ? 1 : 0;
}
