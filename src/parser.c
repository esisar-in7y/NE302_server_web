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
// parameter = token "=" ( token / quoted_string )
tree_node* parameter(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_parameter = tree_node_add_child(parent, parent->string, index, 0, "parameter");
    if (token(node_parameter) == NULL || parent->string[get_end(node_parameter)] != '=') {
        tree_node_free(node_parameter);
        return NULL;
    }
    tree_node_add_child(node_parameter, parent->string, get_end(node_parameter), 1, "=");
    if (token(node_parameter) || quoted_string(node_parameter)) {
        return node_parameter;
    }
    tree_node_free(node_parameter);
    return NULL;
}
// media_type = type "/" subtype *( OWS ";" OWS parameter )
tree_node* media_type(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_media_type = tree_node_add_child(parent, parent->string, index, 0, "media_type");
    if (type(node_media_type) == NULL || parent->string[get_end(node_media_type)] != '/') {
        tree_node_free(node_media_type);
        return NULL;
    }
    tree_node_add_child(node_media_type, parent->string, get_end(node_media_type), 1, "/");
    if (subtype(node_media_type) == NULL) {
        tree_node_free(node_media_type);
        return NULL;
    }
    while (OWS(node_media_type) && parent->string[get_end(node_media_type)] == ';' && OWS(node_media_type)) {
        tree_node_add_child(node_media_type, parent->string, get_end(node_media_type), 1, ";");
        if (parameter(node_media_type) == NULL) {
            tree_node_free(node_media_type);
            return NULL;
        }
    }
    return node_media_type;
}
// Content_Type = media_type
tree_node* Content_Type(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_Content_Type = tree_node_add_child(parent, parent->string, index, 0, "Content_Type");
    if (media_type(node_Content_Type) == NULL) {
        tree_node_free(node_Content_Type);
        return NULL;
    }
    return node_Content_Type;
}
// Content_Type_header = "Content-Type:" OWS Content_Type OWS
tree_node* Content_Type_header(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_Content_Type_header = tree_node_add_child(parent, parent->string, index, 0, "Content_Type_header");
    if (strncmp(parent->string + index, "Content-Type:", 13) != 0) {
        tree_node_free(node_Content_Type_header);
        return NULL;
    }
    tree_node_add_child(node_Content_Type_header, parent->string, index, 14, "Content-Type:");
    if (OWS(node_Content_Type_header) == NULL ||
        Content_Type(node_Content_Type_header) == NULL ||
        OWS(node_Content_Type_header) == NULL) {
        tree_node_free(node_Content_Type_header);
        return NULL;
    }
    return node_Content_Type_header;
}
// Content_Length = 1*DIGIT
tree_node* Content_Length(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_Content_Length = tree_node_add_child(parent, parent->string, index, 0, "Content_Length");
    if (DIGIT(node_Content_Length) == NULL) {
        tree_node_free(node_Content_Length);
        return NULL;
    }
    while (DIGIT(node_Content_Length));
    return node_Content_Length;
}
// Content_Length_header = "Content-Length:" OWS Content_Length OWS
tree_node* Content_Length_header(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_Content_Length_header = tree_node_add_child(parent, parent->string, index, 0, "Content_Length_header");
    if (strncmp(parent->string + index, "Content-Length:", 15) != 0) {
        tree_node_free(node_Content_Length_header);
        return NULL;
    }
    tree_node_add_child(node_Content_Length_header, parent->string, index, 16, "Content-Length:");
    if (OWS(node_Content_Length_header) == NULL ||
        Content_Length(node_Content_Length_header) == NULL ||
        OWS(node_Content_Length_header) == NULL) {
        tree_node_free(node_Content_Length_header);
        return NULL;
    }
    return node_Content_Length_header;
}
// Connection = *( "," OWS ) connection_option *( OWS "," [ OWS connection_option ] )
tree_node* Connection(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_Connection = tree_node_add_child(parent, parent->string, index, 0, "Connection");
    while (parent->string[get_end(node_Connection)] == ',' && OWS(node_Connection));
    if (connection_option(node_Connection) == NULL) {
        tree_node_free(node_Connection);
        return NULL;
    }
    while (OWS(node_Connection) && parent->string[get_end(node_Connection)] == ',' && OWS(node_Connection)) {
        tree_node_add_child(node_Connection, parent->string, get_end(node_Connection), 1, ",");
        if (connection_option(node_Connection) == NULL) {
            tree_node_free(node_Connection);
            return NULL;
        }
    }
    return node_Connection;
}

// Connection_header = "Connection" ":" OWS Connection OWS
tree_node* Connection_header(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_Connection_header = tree_node_add_child(parent, parent->string, index, 0, "Connection_header");
    if (strncmp(parent->string + index, "Connection:", 11) != 0) {
        tree_node_free(node_Connection_header);
        return NULL;
    }
    tree_node_add_child(node_Connection_header, parent->string, index, 12, "Connection:");
    if (OWS(node_Connection_header) == NULL ||
        Connection(node_Connection_header) == NULL ||
        OWS(node_Connection_header) == NULL) {
        tree_node_free(node_Connection_header);
        return NULL;
    }
    return node_Connection_header;
}
// Transfert_encoding = *( "," OWS ) transfer_coding *( OWS "," [ OWS transfer_coding ] )
tree_node* Transfert_encoding(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_Transfert_encoding = tree_node_add_child(parent, parent->string, index, 0, "Transfert_encoding");
    bool end = false;
    while(!end){
        tree_node* tmp=tree_node_new(parent->string, get_end(node_Transfert_encoding), 1, NULL,"tmp");
        if(parent->string[get_end(tmp)] == ','){
            tree_node_add_child(tmp, parent->string, get_end(tmp), 1, ",");
            if(OWS(tmp) != NULL){
                move_childs(tmp, node_Transfert_encoding);
                tree_node_free(tmp);
                continue;
            }
        }
        end = true;
    }
    if(transfert_coding(node_Transfert_encoding)==NULL){
        tree_node_free(node_Transfert_encoding);
        return NULL;
    }
    end = false;
    while(!end){
        tree_node* tmp=tree_node_new(parent->string, get_end(node_Transfert_encoding), 1, NULL,"tmp");
        if(OWS(tmp) != NULL){
            tree_node_add_child(tmp, parent->string, get_end(tmp), 1, ",");
            if(parent->string[get_end(tmp)] == ','){
                tree_node* tmp2=tree_node_new(parent->string, get_end(tmp), 1, NULL,"tmp");
                if(OWS(tmp2) != NULL || transfert_coding(tmp2) != NULL){
                    move_childs(tmp, node_Transfert_encoding);
                }else{
                    tree_node_free(tmp2);
                }
                move_childs(tmp2, node_Transfert_encoding);
                tree_node_free(tmp2);
                continue;
            }
        }
        end = true;
    }
}

// transfer_parameter = token BWS "=" BWS ( token / quoted_string )
tree_node* transfer_parameter(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_transfer_parameter = tree_node_add_child(parent, parent->string, index, 0, "transfer_parameter");
    if (token(node_transfer_parameter) == NULL) {
        tree_node_free(node_transfer_parameter);
        return NULL;
    }
    if (BWS(node_transfer_parameter) == NULL ||
        parent->string[get_end(node_transfer_parameter)] != '=' ||
        BWS(node_transfer_parameter) == NULL) {
        tree_node_free(node_transfer_parameter);
        return NULL;
    }
    tree_node_add_child(node_transfer_parameter, parent->string, get_end(node_transfer_parameter), 1, "=");
    if (BWS(node_transfer_parameter) == NULL) {
        tree_node_free(node_transfer_parameter);
        return NULL;
    }
    if (token(node_transfer_parameter) == NULL && quoted_string(node_transfer_parameter) == NULL) {
        tree_node_free(node_transfer_parameter);
        return NULL;
    }
    return node_transfer_parameter;
}

// transfert_coding = "chunked" / "compress" / "deflate" / "gzip" / transfer_extension
tree_node* transfert_coding(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_transfert_coding = tree_node_add_child(parent, parent->string, index, 0, "transfert_coding");
    if (strncmp(parent->string + index, "chunked", 7) == 0) {
        tree_node_add_child(node_transfert_coding, parent->string, index, 7, "chunked");
        return node_transfert_coding;
    }
    if (strncmp(parent->string + index, "compress", 8) == 0) {
        tree_node_add_child(node_transfert_coding, parent->string, index, 8, "compress");
        return node_transfert_coding;
    }
    if (strncmp(parent->string + index, "deflate", 7) == 0) {
        tree_node_add_child(node_transfert_coding, parent->string, index, 7, "deflate");
        return node_transfert_coding;
    }
    if (strncmp(parent->string + index, "gzip", 4) == 0) {
        tree_node_add_child(node_transfert_coding, parent->string, index, 4, "gzip");
        return node_transfert_coding;
    }
    if (transfer_extension(node_transfert_coding) == NULL) {
        tree_node_free(node_transfert_coding);
        return NULL;
    }
    return node_transfert_coding;
}

// transfer_extension = token [ "=" ( token / quoted_string ) ]
tree_node* transfer_extension(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_transfer_extension = tree_node_add_child(parent, parent->string, index, 0, "transfer_extension");
    if (token(node_transfer_extension) == NULL) {
        tree_node_free(node_transfer_extension);
        return NULL;
    }
    tree_node* tmp=tree_node_new(parent->string, get_end(node_transfer_extension), 1, NULL,"tmp");
    if (parent->string[get_end(tmp)] == '=') {
        tree_node_add_child(tmp, parent->string, get_end(node_transfer_extension), 1, "=");
        if (token(tmp) == NULL && quoted_string(tmp) == NULL) {
            tree_node_free(node_transfer_extension);
            return NULL;
        }
    }
    move_childs(tmp,node_transfer_extension);
    tree_node_free(tmp);
    return node_transfer_extension;
}
// header_field =  Connection_header / Content_Length_header / Content_Type_header / Cookie_header / Transfer_Encoding_header / Expect_header / Host_header / ( field_name ":" OWS field_value OWS )
tree_node* header_field(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_header_field = tree_node_add_child(parent, parent->string, index, 0, "header_field");
    if (
        Connection_header(node_header_field) != NULL ||
        Content_Length_header(node_header_field) != NULL ||
        Content_Type_header(node_header_field) != NULL ||
        Cookie_header(node_header_field) != NULL ||
        Transfer_Encoding_header(node_header_field) != NULL ||
        Expect_header(node_header_field) != NULL ||
        Host_header(node_header_field) != NULL ||) {
        return node_header_field;
    }
    if (field_name(node_header_field) != NULL) {
        if (parent->string[get_end(node_header_field)] != ':') {
            tree_node_free(node_header_field);
            return NULL;
        }
        tree_node_add_child(node_header_field, parent->string, get_end(node_header_field), 1, ":");
        if (OWS(node_header_field) == NULL ||
            field_value(node_header_field) == NULL ||
            OWS(node_header_field) == NULL) {
            tree_node_free(node_header_field);
            return NULL;
        }
        return node_header_field;
    }
    tree_node_free(node_header_field);
    return NULL;
}
// HTTP_name = %x48.54.54.50
tree_node* HTTP_name(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_HTTP_name = tree_node_add_child(parent, parent->string, index, 0, "HTTP_name");
    if (strncmp(parent->string + index, "HTTP", 4) != 0) {
        tree_node_free(node_HTTP_name);
        return NULL;
    }
    tree_node_add_child(node_HTTP_name, parent->string, index, 4, "HTTP");
    return node_HTTP_name;
}
// HTTP_version = HTTP_name "/" DIGIT "." DIGIT
tree_node* HTTP_version(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_HTTP_version = tree_node_add_child(parent, parent->string, index, 0, "HTTP_version");
    if (HTTP_name(node_HTTP_version) == NULL) {
        tree_node_free(node_HTTP_version);
        return NULL;
    }
    if (parent->string[get_end(node_HTTP_version)] != '/') {
        tree_node_free(node_HTTP_version);
        return NULL;
    }
    tree_node_add_child(node_HTTP_version, parent->string, get_end(node_HTTP_version), 1, "/");
    if (DIGIT(node_HTTP_version) == NULL) {
        tree_node_free(node_HTTP_version);
        return NULL;
    }
    if (parent->string[get_end(node_HTTP_version)] != '.') {
        tree_node_free(node_HTTP_version);
        return NULL;
    }
    tree_node_add_child(node_HTTP_version, parent->string, get_end(node_HTTP_version), 1, ".");
    if (DIGIT(node_HTTP_version) == NULL) {
        tree_node_free(node_HTTP_version);
        return NULL;
    }
    return node_HTTP_version;
}
// absolute_path = 1*( "/" segment )
tree_node* absolute_path(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_absolute_path = tree_node_add_child(parent, parent->string, index, 0, "absolute_path");
    if (parent->string[get_end(node_absolute_path)] != '/') {
        tree_node_free(node_absolute_path);
        return NULL;
    }
    tree_node_add_child(node_absolute_path, parent->string, get_end(node_absolute_path), 1, "/");
    if (segment(node_absolute_path) == NULL) {
        tree_node_free(node_absolute_path);
        return NULL;
    }
    while (parent->string[get_end(node_absolute_path)] == '/' && segment(node_absolute_path)!=NULL);
    return node_absolute_path;
}

// origin_form = absolute_path [ "?" query ]
tree_node* origin_form(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_origin_form = tree_node_add_child(parent, parent->string, index, 0, "origin_form");
    if (absolute_path(node_origin_form) == NULL) {
        tree_node_free(node_origin_form);
        return NULL;
    }
    if (parent->string[get_end(node_origin_form)] == '?') {
        tree_node_add_child(node_origin_form, parent->string, get_end(node_origin_form), 1, "?");
        if (query(node_origin_form) == NULL) {
            tree_node_free(node_origin_form);
            return NULL;
        }
    }
    return node_origin_form;
}
// request_target = origin_form
tree_node* request_target(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_request_target = tree_node_add_child(parent, parent->string, index, 0, "request_target");
    if (origin_form(node_request_target) == NULL) {
        tree_node_free(node_request_target);
        return NULL;
    }
    return node_request_target;
}
// request_line = method SP request_target SP HTTP_version CRLF
tree_node* request_line(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_request_line = tree_node_add_child(parent, parent->string, index, 0, "request_line");
    if (
        method(node_request_line) == NULL ||
        SP(node_request_line) == NULL ||
        request_target(node_request_line) == NULL ||
        SP(node_request_line) == NULL ||
        HTTP_version(node_request_line) == NULL ||
        CRLF(node_request_line) == NULL) {
        tree_node_free(node_request_line);
        return NULL;
    }
    return node_request_line;
}
// start_line = request_line
tree_node* start_line(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_start_line = tree_node_add_child(parent, parent->string, index, 0, "start_line");
    if (request_line(node_start_line) == NULL) {
        tree_node_free(node_start_line);
        return NULL;
    }
    return node_start_line;
}
// HTTP_message = start_line *( header_field CRLF ) CRLF [ message_body ]
tree_node* HTTP_message(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_HTTP_message = tree_node_add_child(parent, parent->string, index, 0, "HTTP_message");
    if (start_line(node_HTTP_message) == NULL) {
        tree_node_free(node_HTTP_message);
        return NULL;
    }
    while (header_field(node_HTTP_message) != NULL && CRLF(node_HTTP_message) != NULL);
    if (CRLF(node_HTTP_message) == NULL) {
        tree_node_free(node_HTTP_message);
        return NULL;
    }
    if (message_body(node_HTTP_message) != NULL) {
        tree_node_free(node_HTTP_message);
        return NULL;
    }
    return node_HTTP_message;
}