#include "parser.h"
// TODO update perent length on child add
int parseur(char* req, int bytes) {
    printf("size:%d req:%s\n", bytes, req);
    tree_node* root = tree_node_init(req);
    bool rep = validate_message(root);
    printf("rep:%d\n", rep);
    tree_node_print_all(root, 0);
    if (!rep) {
        printf("error\n");
        // tree_node_free(root);
        return 0;
    }
    return 1;
}
// nombre = 1*DIGIT
tree_node* validate_number(tree_node* parent) {
    int index = get_start(parent);
    if (!isdigit(parent->string[index])) {
        printf("not a number:%c\n", parent->string[index]);
        return NULL;
    }
    tree_node* node_nombre = tree_node_add_child(parent, parent->string, index, 0, "nombre");
    tree_node_add_child(node_nombre, parent->string, index, 1, "digit");
    index++;
    while (isdigit(parent->string[index])) {
        tree_node_add_child(node_nombre, parent->string, index, 1, "digit");
        index++;
    }
    return node_nombre;
}
// ponct = "," / "." / "!" / "?" / ":"
tree_node* validate_ponct(tree_node* parent) {
    int index = get_start(parent);
    switch (parent->string[index]) {
        case ',':
        case '.':
        case '!':
        case '?':
        case ':':
            return tree_node_add_child(parent, parent->string, index, 1, "ponct");
        default:
            return NULL;
    }
}
// separateur = SP / HTAB / "-" / "_"
tree_node* validate_separateur(tree_node* parent) {
    int index = get_start(parent);
    switch (parent->string[index]) {
        case ' ':
        case '\t':
        case '-':
        case '_':
            return tree_node_add_child(parent, parent->string, index, 1, "separateur");
        default:
            return NULL;
    }
}
// debut = "start"
tree_node* validate_debut(tree_node* parent) {
    int index = get_start(parent);
    if (strncasecmp(parent->string + index, "start", 5) == 0) {
        return tree_node_add_child(parent, parent->string, index, 5, "debut");
    }
    return NULL;
}
// fin = "fin"
tree_node* validate_fin(tree_node* parent) {
    int index = get_start(parent);
    if (strncasecmp(parent->string + index, "fin", 3) == 0) {
        return tree_node_add_child(parent, parent->string, index, 3, "fin");
    }
    return NULL;
}
// mot = 1*ALPHA separateur
tree_node* validate_mot(tree_node* parent) {
    int index = get_start(parent);
    printf("mot:%s\n", parent->string + index);
    tree_node* node_mot = tree_node_add_child(parent, parent->string, index, 1, "mot");  // 2
    if (!isalpha(parent->string[index])) {
        printf("alpha !ok\n");
        tree_node_free(node_mot);
        return NULL;
    }
    tree_node_add_child(node_mot, parent->string, index, 1, "alpha");
    index++;
    while (isalpha(parent->string[index])) {
        tree_node_add_child(node_mot, parent->string, index, 1, "alpha");
        index++;
    }

    if (validate_separateur(node_mot) == NULL) {
        tree_node_free(node_mot);
        return NULL;
    }

    return node_mot;
}
// mot ponct
tree_node* validate_mot_ponct(tree_node* parent) {
    tree_node* node_mot = NULL;
    tree_node* node_ponctuation = NULL;
    if ((node_mot = validate_mot(parent)) != NULL) {
        printf("mot ok\n");
        if ((node_ponctuation = validate_ponct(parent)) != NULL) {
            printf("ponct ok\n");
            return node_ponctuation;
        }
    }
    tree_node_free(node_mot);
    tree_node_free(node_ponctuation);
    return NULL;
}
// nombre separateur
tree_node* validate_number_separateur(tree_node* parent) {
    tree_node* node_number = NULL;
    tree_node* node_separateur = NULL;
    if ((node_number = validate_number(parent)) != NULL) {
        printf("mot ok\n");
        if ((node_separateur = validate_separateur(parent)) != NULL) {
            printf("ponct ok\n");
            return node_separateur;
        }
    }
    tree_node_free(node_number);
    tree_node_free(node_separateur);
    return NULL;
}
// message = debut 2*( mot ponct /nombre separateur ) [ponct] fin LF
bool validate_message(tree_node* parent) {
    if (validate_debut(parent) != NULL) {
        printf("debut ok\n");
        int i = 0;
        while (validate_number_separateur(parent) != NULL || validate_mot_ponct(parent) != NULL) {
            i++;
        }
        if (i < 2) return false;
        validate_ponct(parent);  // optional
        if (validate_fin(parent) != NULL) {
            printf("fin ok\n");
            if (parent->string[get_start(parent)] == '\n') {
                printf("LF ok\n");
                return true;
            }
        }
    }
    return false;
}

// message_body = *OCTET
tree_node* message_body(tree_node* parent) {
    int index = get_start(parent);
    return tree_node_add_child(parent, parent->string, index, strlen(parent->string), "message_body");
}

// obs_text= %x80-FF
tree_node* obs_text(tree_node* parent) {
    int index = get_start(parent);
    if (0x80 < parent->string[index] && parent->string[index] < 0xFF) {
        return tree_node_add_child(parent, parent->string, index, 1, "obs_text");
    }
    return NULL;
}

// vchar = %x21-7E
tree_node* vchar(tree_node* parent) {
    int index = get_start(parent);
    if (0x21 < parent->string[index] && parent->string[index] < 0x7E) {
        return tree_node_add_child(parent, parent->string, index, 1, "vchar");
    }
    return NULL;
}

// tchar = "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." /"^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA
tree_node* tchar(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_tchar = tree_node_add_child(parent, parent->string, index, 1, "tchar");
    char valids[] = "!#$%&'*+-.^_`|~";
    if (
        strchr(valids, parent->string[index]) != NULL ||
        DIGIT(node_tchar) != NULL ||
        ALPHA(node_tchar) != NULL) {
        return node_tchar;
    }
    tree_node_free(node_tchar);
    return NULL;
}

// token = 1*tchar
tree_node* token(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_token = tree_node_add_child(parent, parent->string, index, 1, "token");
    if (tchar(node_token) == NULL) {
        tree_node_free(node_token);
        return NULL;
    }
    while (tchar(node_token) != NULL)
        ;
    return node_token;
}

// cookie_name = token
tree_node* cookie_name(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_token = tree_node_add_child(parent, parent->string, index, 1, "cookie_name");
    if (token(node_token) == NULL) {
        tree_node_free(node_token);
        return NULL;
    }
    return node_token;
}

// field_name = token
tree_node* field_name(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_token = tree_node_add_child(parent, parent->string, index, 1, "field_name");
    if (token(node_token) == NULL) {
        tree_node_free(node_token);
        return NULL;
    }
    return node_token;
}

// type = token
tree_node* type(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_token = tree_node_add_child(parent, parent->string, index, 1, "type");
    if (token(node_token) == NULL) {
        tree_node_free(node_token);
        return NULL;
    }
    return node_token;
}

// subtype = token
tree_node* subtype(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_token = tree_node_add_child(parent, parent->string, index, 1, "subtype");
    if (token(node_token) == NULL) {
        tree_node_free(node_token);
        return NULL;
    }
    return node_token;
}

// method = token
tree_node* method(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_token = tree_node_add_child(parent, parent->string, index, 1, "method");
    if (token(node_token) == NULL) {
        tree_node_free(node_token);
        return NULL;
    }
    return node_token;
}

// port = *DIGIT
tree_node* port(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_port = tree_node_add_child(parent, parent->string, index, 0, "port");
    while (DIGIT(node_port) != NULL)
        ;
    return node_port;
}

// unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
tree_node* unreserved(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_unreserved = tree_node_add_child(parent, parent->string, index, 1, "unreserved");
    char valids[] = "-._~";
    if (ALPHA(node_unreserved) != NULL) {
        return node_unreserved;
    }
    if (DIGIT(node_unreserved) != NULL) {
        return node_unreserved;
    }
    if (strchr(valids, parent->string[index]) != NULL) {
        return tree_node_add_child(parent, parent->string, index, 1, "unreserved");
    }
    tree_node_free(node_unreserved);
    return NULL;
}

// sub_delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
tree_node* sub_delims(tree_node* parent) {
    int index = get_start(parent);
    char valids[] = "!$&'()*+,;=";
    if (strchr(valids, parent->string[index]) != NULL) {
        return tree_node_add_child(parent, parent->string, index, 1, "sub_delims");
    }
    return NULL;
}

// connection_option = token
tree_node* connection_option(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_token = tree_node_add_child(parent, parent->string, index, 1, "connection_option");
    if (token(node_token) == NULL) {
        tree_node_free(node_token);
        return NULL;
    }
    return node_token;
}
// HEXDIG = DIGIT / "A" / "B" / "C" / "D" / "E" / "F"
tree_node* HEXDIG(tree_node* parent) {
    int index = get_start(parent);
    char valids[] = "ABCDEF";
    if (isdigit(parent->string[index])) {
        return tree_node_add_child(parent, parent->string, index, 1, "HEXDIG");
    }
    if (strchr(valids, parent->string[index]) != NULL) {
        return tree_node_add_child(parent, parent->string, index, 1, "HEXDIG");
    }
    return NULL;
}
int at_least_x(tree_node* parent, tree_node* (*func)(tree_node*), int x) {
    int count = 0;
    while (func(parent) != NULL) {
        count++;
    }
    return count >= x;
}

// IPvFuture = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
tree_node* IPvFuture(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_IPvFuture = tree_node_add_child(parent, parent->string, index, 1, "IPvFuture");
    if (parent->string[index] != 'v') {
        tree_node_free(node_IPvFuture);
        return NULL;
    }
    index++;
    int start_index = index;
    if (!at_least_x(node_IPvFuture, HEXDIG, 1)) {
        tree_node_free(node_IPvFuture);
        return NULL;
    }
    index = get_start(node_IPvFuture);
    if (parent->string[index] != '.') {
        tree_node_free(node_IPvFuture);
        return NULL;
    }
    index++;
    start_index = index;
    if (!at_least_x(node_IPvFuture, unreserved, 1) && !at_least_x(node_IPvFuture, sub_delims, 1) && parent->string[index] != ':') {
        tree_node_free(node_IPvFuture);
        return NULL;
    }
    return node_IPvFuture;
}

// dec-octet = "25" %x30-35 / "2" %x30-34 DIGIT / "1" 2DIGIT / %x31-39 DIGIT / DIGIT
tree_node* dec_octet(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_dec_octet = tree_node_add_child(parent, parent->string, index, 1, "dec_octet");
    if (parent->string[index] == '2') {
        tree_node_add_child(node_dec_octet, parent->string, index, 1, "2");
        index++;
        if (parent->string[index] == '5') {
            tree_node_add_child(node_dec_octet, parent->string, index, 1, "5");
            index++;
            if (parent->string[index] >= '0' && parent->string[index] <= '5') {
                tree_node_add_child(node_dec_octet, parent->string, index, 1, "0-5");
                return node_dec_octet;
            }
            tree_node_free(node_dec_octet);
            return NULL;
        }
        if (parent->string[index] >= '0' && parent->string[index] <= '4') {
            tree_node_add_child(node_dec_octet, parent->string, index, 1, "0-4");
            index++;
            if (isdigit(parent->string[index]) != 0) {
                tree_node_add_child(node_dec_octet, parent->string, index, 1, "DIGIT");
                return node_dec_octet;
            }
            tree_node_free(node_dec_octet);
            return NULL;
        }
        tree_node_free(node_dec_octet);
        return NULL;
    }
    if (parent->string[index] == '1') {
        tree_node_add_child(node_dec_octet, parent->string, index, 1, "1");
        index++;
        if (isdigit(parent->string[index]) != 0) {
            tree_node_add_child(node_dec_octet, parent->string, index, 1, "DIGIT");
            index++;
            if (isdigit(parent->string[index]) != 0) {
                tree_node_add_child(node_dec_octet, parent->string, index, 1, "DIGIT");
                return node_dec_octet;
            }
            tree_node_free(node_dec_octet);
            return NULL;
        }
        tree_node_free(node_dec_octet);
        return NULL;
    }
    if (parent->string[index] >= '1' && parent->string[index] <= '9') {
        tree_node_add_child(node_dec_octet, parent->string, index, 1, "1-9");
        index++;
        if (isdigit(parent->string[index]) != 0) {
            tree_node_add_child(node_dec_octet, parent->string, index, 1, "DIGIT");
            return node_dec_octet;
        }
        tree_node_free(node_dec_octet);
        return NULL;
    }
    if (isdigit(parent->string[index]) != 0) {
        tree_node_add_child(node_dec_octet, parent->string, index, 1, "DIGIT");
        return node_dec_octet;
    }
    tree_node_free(node_dec_octet);
    return NULL;
}

// SP = %x20
tree_node* SP(tree_node* parent) {
    int index = get_start(parent);
    if (parent->string[index] == ' ') {
        return tree_node_add_child(parent, parent->string, index, 1, "SP");
    }
    return NULL;
}

// HTAB = %x09
tree_node* HTAB(tree_node* parent) {
    int index = get_start(parent);
    if (parent->string[index] == '\t') {
        return tree_node_add_child(parent, parent->string, index, 1, "HTAB");
    }
    return NULL;
}
// field_content = field_vchar [ 1*( SP / HTAB ) field_vchar ]
tree_node* field_content(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_field_content = tree_node_add_child(parent, parent->string, index, 1, "field_content");
    if (field_vchar(node_field_content) == NULL) {
        tree_node_free(node_field_content);
        return NULL;
    }
    index = get_start(parent);
    tree_node* tmp_parent = tree_node_add_child(parent, parent->string, index, 1, "tmp");
    if (!at_least_x(tmp_parent, SP, 1) && !at_least_x(tmp_parent, HTAB, 1)) {
        tree_node_free(tmp_parent);
        return node_field_content;
    }
    if (field_vchar(node_field_content) == NULL) {
        tree_node_free(tmp_parent);
        return node_field_content;
    }
    move_childs(tmp_parent, node_field_content);
    return node_field_content;
}

// field_value = *( field_content / obs_fold )
tree_node* field_value(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_field_value = tree_node_add_child(parent, parent->string, index, 1, "field_value");
    while (field_content(node_field_value) != NULL || obs_fold(node_field_value) != NULL)
        ;
    return node_field_value;
}

// pct_encoded = "%" HEXDIG HEXDIG
tree_node* pct_encoded(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_pct_encoded = tree_node_add_child(parent, parent->string, index, 1, "pct_encoded");
    if (parent->string[index] == '%') {
        tree_node_add_child(node_pct_encoded, parent->string, index, 1, "%");
        index++;
        if (
            HEXDIG(node_pct_encoded) == NULL ||
            HEXDIG(node_pct_encoded) == NULL) {
            tree_node_free(node_pct_encoded);
            return NULL;
        }
        return node_pct_encoded;
    }
    tree_node_free(node_pct_encoded);
    return NULL;
}

// pchar = unreserved / pct_encoded / sub_delims / ":" / "@"
tree_node* pchar(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_pchar = tree_node_add_child(parent, parent->string, index, 1, "pchar");
    if (unreserved(node_pchar) != NULL) {
        return node_pchar;
    }
    if (pct_encoded(node_pchar) != NULL) {
        return node_pchar;
    }
    if (sub_delims(node_pchar) != NULL) {
        return node_pchar;
    }
    if (parent->string[index] == ':') {
        tree_node_add_child(node_pchar, parent->string, index, 1, ":");
        return node_pchar;
    }
    if (parent->string[index] == '@') {
        tree_node_add_child(node_pchar, parent->string, index, 1, "@");
        return node_pchar;
    }
    tree_node_free(node_pchar);
    return NULL;
}

// segment = *pchar
tree_node* segment(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_segment = tree_node_add_child(parent, parent->string, index, 1, "segment");
    while (pchar(node_segment) != NULL)
        ;
    return node_segment;
}

// query = *( pchar / "/" / "?" )
tree_node* query(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_query = tree_node_add_child(parent, parent->string, index, 1, "query");
    bool end = false;
    while (!end) {
        if (pchar(node_query) != NULL) {
        } else if (parent->string[index] == '/') {
            tree_node_add_child(node_query, parent->string, index, 1, "/");
        } else if (parent->string[index] == '?') {
            tree_node_add_child(node_query, parent->string, index, 1, "?");
        } else {
            end = true;
        }
        index++;
    }
    return node_query;
}

// reg_name = *( unreserved / pct_encoded / sub_delims )
tree_node* reg_name(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_reg_name = tree_node_add_child(parent, parent->string, index, 1, "reg_name");
    while (unreserved(node_reg_name) != NULL || pct_encoded(node_reg_name) != NULL || sub_delims(node_reg_name) != NULL)
        ;
    return node_reg_name;
}

// DIGIT = %x30-39
tree_node* DIGIT(tree_node* parent) {
    int index = get_start(parent);
    if (parent->string[index] >= '0' && parent->string[index] <= '9') {
        return tree_node_add_child(parent, parent->string, index, 1, "DIGIT");
    }
    return NULL;
}
// ALPHA = %x41-5A / %x61-7A
tree_node* ALPHA(tree_node* parent) {
    int index = get_start(parent);
    if ((parent->string[index] >= 'A' && parent->string[index] <= 'Z') || (parent->string[index] >= 'a' && parent->string[index] <= 'z')) {
        return tree_node_add_child(parent, parent->string, index, 1, "ALPHA");
    }
    return NULL;
}

// Host = uri_host [ ":" port ]
tree_node* Host(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_Host = tree_node_add_child(parent, parent->string, index, 1, "Host");
    if (uri_host(node_Host) == NULL) {
        tree_node_free(node_Host);
        return NULL;
    }
    index = get_start(node_Host);
    if (parent->string[index] == ':') {
        tree_node_add_child(node_Host, parent->string, index, 1, ":");
        index++;
        if (port(node_Host) == NULL) {
            tree_node_free(node_Host);
            return NULL;
        }
    }
    return node_Host;
}

// OWS = *( SP / HTAB )
tree_node* OWS(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_OWS = tree_node_add_child(parent, parent->string, index, 1, "OWS");
    while (parent->string[index] == ' ' || parent->string[index] == '\t') {
        if (parent->string[index] == ' ') {
            tree_node_add_child(node_OWS, parent->string, index, 1, " ");
        }
        if (parent->string[index] == '\t') {
            tree_node_add_child(node_OWS, parent->string, index, 1, "\t");
        }
        index++;
    }
    return node_OWS;
}

// Host_header = "Host" ":" OWS Host OWS
tree_node* Host_header(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_Host_header = tree_node_add_child(parent, parent->string, index, 1, "Host_header");
    if (strncasecmp(parent->string + index, "Host:", 5) != 0) {
        tree_node_free(node_Host_header);
        return NULL;
    }
    tree_node_add_child(node_Host_header, parent->string, index, 4, "Host");
    index += 4;
    tree_node_add_child(node_Host_header, parent->string, index, 1, ":");
    index++;
    if (OWS(node_Host_header) == NULL ||
        Host(node_Host_header) == NULL ||
        OWS(node_Host_header) == NULL) {
        tree_node_free(node_Host_header);
        return NULL;
    }
    return node_Host_header;
}
// Expect = "100-continue"
tree_node* Expect(tree_node* parent) {
    int index = get_start(parent);
    if (strncasecmp(parent->string + index, "100-continue", 12) == 0) {
        return tree_node_add_child(parent, parent->string, index, 12, "Expect");
    }
    return NULL;
}

// IPv4address   = dec_octet "." dec_octet "." dec_octet "." dec_octet
tree_node* IPv4address(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_IPv4address = tree_node_add_child(parent, parent->string, index, 1, "IPv4address");
    for (int i = 0; i < 3; i++) {
        index = get_start(node_IPv4address);
        if (dec_octet(node_IPv4address) == NULL) {
            tree_node_free(node_IPv4address);
            return NULL;
        }
        index = get_start(node_IPv4address);
        if (parent->string[index] != '.') {
            tree_node_free(node_IPv4address);
            return NULL;
        }
        tree_node_add_child(node_IPv4address, parent->string, index, 1, ".");
    }
    return node_IPv4address;
}

// h16           = 1*4HEXDIG
tree_node* h16(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_h16 = tree_node_add_child(parent, parent->string, index, 1, "h16");
    for (int i = 0; i < 4; i++) {
        index = get_start(node_h16);
        if (HEXDIG(node_h16) == NULL) {
            tree_node_free(node_h16);
            return NULL;
        }
    }
    return node_h16;
}
// ls32          = ( h16 ":" h16 ) / IPv4address
tree_node* ls32(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_ls32 = tree_node_add_child(parent, parent->string, index, 1, "ls32");
    if (h16(node_ls32) == NULL ||
        parent->string[get_start(node_ls32)] != ':' ||
        h16(node_ls32) == NULL) {
        tree_node_free(node_ls32);
        return NULL;
    }
    return node_ls32;
}

// IPv6address   =                            6( h16 ":" ) ls32
//               /                       "::" 5( h16 ":" ) ls32
//               / [               h16 ] "::" 4( h16 ":" ) ls32
//               / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
//               / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
//               / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
//               / [ *4( h16 ":" ) h16 ] "::"              ls32
//               / [ *5( h16 ":" ) h16 ] "::"              h16
//               / [ *6( h16 ":" ) h16 ] "::"
// TODO good luck les mecs
// c'est moa ki sui quasé ou cé ip6 ki est quasé ?
// 8 morts 6 blessés je pète ma bière ma luubellule
tree_node* IPv6address(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_IPv6address = tree_node_add_child(parent, parent->string, index, 1, "IPv6address");
    int count = 0;
    while (parent->string[index] != ':') {
        if (h16(node_IPv6address) == NULL) {
            tree_node_free(node_IPv6address);
            return NULL;
        }
        index = get_start(node_IPv6address);
        if (parent->string[index] != ':') {
            tree_node_free(node_IPv6address);
            return NULL;
        }
        tree_node_add_child(node_IPv6address, parent->string, index, 1, ":");
        index++;
        count++;
    }
    if (count > 6) {
        tree_node_free(node_IPv6address);
        return NULL;
    }
    if (count == 6) {
        if (ls32(node_IPv6address) == NULL) {
            if (
                parent->string[index + 1] != ':' &&
                parent->string[index + 2] != ':') {
                tree_node_free(node_IPv6address);
                return NULL;
            }
            tree_node_add_child(node_IPv6address, parent->string, index, 2, "::");
        }
        return node_IPv6address;
    }
    if (count == 0) {
        if (parent->string[index + 1] != ':' ||
            parent->string[index + 2] != ':') {
            tree_node_free(node_IPv6address);
            return NULL;
        }
        tree_node_add_child(node_IPv6address, parent->string, index, 2, "::");
    }
    for (int i = 0; i < 5 - count; i++) {
        index += 2;
        if (h16(node_IPv6address) == NULL) {
            tree_node_free(node_IPv6address);
            return NULL;
        }
        index = get_start(node_IPv6address);
        if (parent->string[index] != ':') {
            tree_node_free(node_IPv6address);
            return NULL;
        }
        tree_node_add_child(node_IPv6address, parent->string, index, 1, ":");
        index++;
    }
    if (parent->string[index] != ':' ||
        parent->string[index + 1] != ':') {
        tree_node_free(node_IPv6address);
        return NULL;
    }
    tree_node_add_child(node_IPv6address, parent->string, index, 2, "::");
    index += 2;
    if (ls32(node_IPv6address) == NULL) {
        tree_node_free(node_IPv6address);
        return NULL;
    }
    return node_IPv6address;
}

// Expect_header = "Expect" ":" OWS Expect OWS
tree_node* Expect_header(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_Expect_header = tree_node_add_child(parent, parent->string, index, 0, "Expect_header");
    if (strncmp(parent->string + index, "Expect:", 7) != 0) {
        tree_node_free(node_Expect_header);
        return NULL;
    }
    tree_node_add_child(node_Expect_header, parent->string, index, 7, "Expect:");
    if (OWS(node_Expect_header) == NULL ||
        Expect(node_Expect_header) == NULL ||
        OWS(node_Expect_header) == NULL) {
        tree_node_free(node_Expect_header);
        return NULL;
    }
    return node_Expect_header;
}

// cookie_octet = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E
tree_node* cookie_octet(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_cookie_octet = tree_node_add_child(parent, parent->string, index, 1, "cookie_octet");
    if (parent->string[index] == 0x21 ||
        (parent->string[index] >= 0x23 && parent->string[index] <= 0x2B) ||
        (parent->string[index] >= 0x2D && parent->string[index] <= 0x3A) ||
        (parent->string[index] >= 0x3C && parent->string[index] <= 0x5B) ||
        (parent->string[index] >= 0x5D && parent->string[index] <= 0x7E)) {
        return node_cookie_octet;
    }
    tree_node_free(node_cookie_octet);
    return NULL;
}

// DQUOTE = %x22
tree_node* DQUOTE(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_DQUOTE = tree_node_add_child(parent, parent->string, index, 1, "DQUOTE");
    if (parent->string[index] == 0x22) {
        return node_DQUOTE;
    }
    tree_node_free(node_DQUOTE);
    return NULL;
}

// qdtext = HTAB / SP / "!" / %x23-5B / %x5D-7E / obs_text
tree_node* qdtext(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_qdtext = tree_node_add_child(parent, parent->string, index, 1, "qdtext");
    if (HTAB(node_qdtext) ||
        SP(node_qdtext) ||
        obs_text(node_qdtext)) {
        return node_qdtext;
    }
    if(parent->string[index] == '!'){
        tree_node_add_child(node_qdtext, parent->string, index, 1, "!");
        return node_qdtext;
    }
    if(parent->string[index] >= 0x23 && parent->string[index] <= 0x5B){
        tree_node_add_child(node_qdtext, parent->string, index, 1, "%x23-5B");
        return node_qdtext;
    }
    if(parent->string[index] >= 0x5D && parent->string[index] <= 0x7E){
        tree_node_add_child(node_qdtext, parent->string, index, 1, "%x5D-7E");
        return node_qdtext;
    }
    tree_node_free(node_qdtext);
    return NULL;
}
// cookie_value = ( DQUOTE *cookie_octet DQUOTE ) / *cookie_octet 
tree_node* cookie_value(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_cookie_value = tree_node_add_child(parent, parent->string, index, 0, "cookie_value");
    tree_node* tmp = tree_node_add_child(parent, parent->string, index, 0, "tmp");
    if (DQUOTE(tmp)) {
        while (cookie_octet(tmp)!=NULL);
        if (DQUOTE(tmp) == NULL) {
            tree_node_free(tmp);
            return NULL;
        }
        move_childs(tmp,node_cookie_value);
        return node_cookie_value;
    }
    while (cookie_octet(node_cookie_value)!=NULL);
    return node_cookie_value;
}

// cookie_pair = cookie_name "=" cookie_value
tree_node* cookie_pair(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_cookie_pair = tree_node_add_child(parent, parent->string, index, 0, "cookie_pair");
    if (cookie_name(node_cookie_pair) == NULL || parent->string[get_end(node_cookie_pair)] != '=') {
        tree_node_free(node_cookie_pair);
        return NULL;
    }
    tree_node_add_child(node_cookie_pair, parent->string, get_start(node_cookie_pair), 1, "=");
    if(cookie_value(node_cookie_pair) == NULL){
        tree_node_free(node_cookie_pair);
        return NULL;
    }
    return node_cookie_pair;
}

// cookie_string = cookie_pair *( ";" SP cookie_pair )
tree_node* cookie_string(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_cookie_string = tree_node_add_child(parent, parent->string, index, 0, "cookie_string");
    if (cookie_pair(node_cookie_string) == NULL) {
        tree_node_free(node_cookie_string);
        return NULL;
    }
    while (parent->string[get_end(node_cookie_string)] == ';') {
        tree_node_add_child(node_cookie_string, parent->string, get_end(node_cookie_string), 1, ";");
        if (SP(node_cookie_string) == NULL ||
            cookie_pair(node_cookie_string) == NULL) {
            tree_node_free(node_cookie_string);
            return NULL;
        }
    }
    return node_cookie_string;
}
// Cookie_header = "Cookie:" OWS cookie_string OWS
tree_node* Cookie_header(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_Cookie_header = tree_node_add_child(parent, parent->string, index, 0, "Cookie_header");
    if (strncmp(parent->string + index, "Cookie:", 7) != 0) { 
        tree_node_free(node_Cookie_header);
        return NULL;
    }
    tree_node_add_child(node_Cookie_header, parent->string, index, 8, "Cookie:");
    if (OWS(node_Cookie_header) == NULL ||
        cookie_string(node_Cookie_header) == NULL ||
        OWS(node_Cookie_header) == NULL) {
        tree_node_free(node_Cookie_header);
        return NULL;
    }
    return node_Cookie_header;
}
// quoted_pair = "\" ( HTAB / SP / VCHAR / obs_text )
tree_node* quoted_pair(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_quoted_pair = tree_node_add_child(parent, parent->string, index, 0, "quoted_pair");
    if (parent->string[index] != '\\') {
        tree_node_free(node_quoted_pair);
        return NULL;
    }
    tree_node_add_child(node_quoted_pair, parent->string, index, 1, "\\");
    if (HTAB(node_quoted_pair) ||
        SP(node_quoted_pair) ||
        VCHAR(node_quoted_pair) ||
        obs_text(node_quoted_pair)) {
        return node_quoted_pair;
    }
    tree_node_free(node_quoted_pair);
    return NULL;
}
// quoted_string = DQUOTE *( qdtext / quoted_pair ) DQUOTE
tree_node* quoted_string(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_quoted_string = tree_node_add_child(parent, parent->string, index, 0, "quoted_string");
    if (DQUOTE(node_quoted_string) == NULL) {
        tree_node_free(node_quoted_string);
        return NULL;
    }
    while (qdtext(node_quoted_string) != NULL || quoted_pair(node_quoted_string) != NULL);
    if (DQUOTE(node_quoted_string) == NULL) {
        tree_node_free(node_quoted_string);
        return NULL;
    }
    return node_quoted_string;
}