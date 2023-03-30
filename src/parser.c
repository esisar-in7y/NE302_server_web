#include "parser.h"
// TODO move tmp that works LOL
int parseur(char* req, int bytes) {
    printf("size:%d req:%s\n", bytes, req);
    tree_node* root = tree_node_init(req);
    bool rep = HTTP_message(root) != NULL;  // validate_message(root);
    printf("rep:%d\n", rep);
    tree_node_print_all(root, 0);
    if (!rep) {
        printf("error\n");
        // tree_node_free(root);
        return 0;
    }
    return 1;
}
#define RED     "\033[31m"      /* Red */
#define RESET   "\033[0m"
void debug(tree_node* node_tmp,int line) {
    int middle=node_tmp->start_string+node_tmp->length_string;
    printf(">%-4d|%-20s|",line,tree_node_string[node_tmp->type]);
    print_sub_str(node_tmp->string, 0, middle);
    printf(RED);
    print_sub_str(node_tmp->string, middle, strlen(node_tmp->string)-middle);
    printf(RESET "\n");
}
// nombre = 1*DIGIT
tree_node* validate_number(tree_node* parent) {
    int index = get_start(parent);
    if (!isdigit(parent->string[index])) {
        printf("not a number:%c\n", parent->string[index]);
        return NULL;
    }
    tree_node* node_nombre = tree_node_add_node(parent, "nombre");
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
    tree_node* node_mot = tree_node_add_node(parent, "mot");  // 2
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

tree_node* check_sa(tree_node* parent, char* chars) {
    if (strncasecmp(parent->string + get_start(parent), chars, strlen(chars)) == 0) {
        return tree_node_add_child(parent, parent->string, get_start(parent), strlen(chars), chars);
    }
    return NULL;
}
tree_node* bloc_chain(tree_node* parent, tree_node* (**f)(tree_node* parent), int f_num) {
    tree_node* node_tmp = tree_node_tmp(parent);
    bool good = false;
    for (int i = 0; !good && i < f_num; i++) {
        if ((*(f + i))(node_tmp) != NULL) {
            good = true;
        }
    }
    if (good) {
        move_childs(node_tmp, parent);
        tree_node_free(node_tmp);
        return parent;
    } else {
        tree_node_free(node_tmp);
        return NULL;
    }
}
bool at_least_x(tree_node* parent, tree_node* (*func)(tree_node*), int x) {
    int count = 0;
    while (func(parent) != NULL) {
        count++;
    }
    return count >= x;
}

// CRLF = CR LF
tree_node* CRLF(tree_node* parent) {
    int index = get_start(parent);
    if (parent->string[index] == '\r' && parent->string[index + 1] == '\n') {
        return tree_node_add_child(parent, parent->string, index, 2, "CRLF");
    }
    return NULL;
}

// message_body = *OCTET
tree_node* message_body(tree_node* parent) {
    int index = get_start(parent);
    unsigned length = 0;
    while (parent->string[index + length + 1] != '\0')
        length++;
    return tree_node_add_child(parent, parent->string, index, length, "message_body");
}

// obs_text= %x80-FF
tree_node* obs_text(tree_node* parent) {
    int index = get_start(parent);
    if (0x80 <= parent->string[index] && parent->string[index] <= 0xFF) {
        return tree_node_add_child(parent, parent->string, index, 1, "obs_text");
    }
    return NULL;
}

// vchar = %x21-7E
tree_node* VCHAR(tree_node* parent) {
    int index = get_start(parent);
    if (0x21 <= parent->string[index] && parent->string[index] <= 0x7E) {
        return tree_node_add_child(parent, parent->string, index, 1, "vchar");
    }
    return NULL;
}

// tchar = "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." /"^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA
tree_node* tchar(tree_node* parent) {
    tree_node* node_tchar = tree_node_add_node(parent, "tchar");
    char valids[] = "!#$%&'*+-.^_`|~";
    if (
        DIGIT(node_tchar) != NULL ||
        ALPHA(node_tchar) != NULL) {
        return node_tchar;
    }
    int index = get_start(node_tchar);
    if (strchr(valids, parent->string[index]) != NULL) {
        tree_node_add_child(node_tchar, parent->string, index, 1, "tchar");
        return node_tchar;
    }
    tree_node_free(node_tchar);
    return NULL;
}

// token = 1*tchar
tree_node* token(tree_node* parent) {
    tree_node* node_token = tree_node_add_node(parent, "token");
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
    tree_node* node_token = tree_node_add_node(parent, "cookie_name");
    if (token(node_token) == NULL) {
        tree_node_free(node_token);
        return NULL;
    }
    return node_token;
}

// field_name = token
tree_node* field_name(tree_node* parent) {
    tree_node* node_token = tree_node_add_node(parent, "field_name");
    if (token(node_token) == NULL) {
        tree_node_free(node_token);
        return NULL;
    }
    return node_token;
}

// type = token
tree_node* type(tree_node* parent) {
    tree_node* node_token = tree_node_add_node(parent, "type");
    if (token(node_token) == NULL) {
        tree_node_free(node_token);
        return NULL;
    }
    return node_token;
}

// subtype = token
tree_node* subtype(tree_node* parent) {
    tree_node* node_token = tree_node_add_node(parent, "subtype");
    if (token(node_token) == NULL) {
        tree_node_free(node_token);
        return NULL;
    }
    return node_token;
}

// method = token
tree_node* method(tree_node* parent) {
    tree_node* node_token = tree_node_add_node(parent, "method");
    if (token(node_token) == NULL) {
        tree_node_free(node_token);
        return NULL;
    }
    return node_token;
}

// port = *DIGIT
tree_node* port(tree_node* parent) {
    tree_node* node_port = tree_node_add_node(parent, "port");
    while (DIGIT(node_port) != NULL)
        ;
    return node_port;
}

// unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
tree_node* unreserved(tree_node* parent) {
    tree_node* node_unreserved = tree_node_add_node(parent, "unreserved");
    char valids[] = "-._~";
    if (ALPHA(node_unreserved) != NULL) {
        return node_unreserved;
    }
    if (DIGIT(node_unreserved) != NULL) {
        return node_unreserved;
    }
    int index = get_start(parent);
    if (strchr(valids, parent->string[index]) != NULL) {
        return tree_node_add_child(parent, parent->string, index, 1, "unreserved");
    }
    tree_node_free(node_unreserved);
    return NULL;
}

// sub_delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
tree_node* sub_delims(tree_node* parent) {
    char valids[] = "!$&'()*+,;=";
    int index = get_start(parent);
    if (strchr(valids, parent->string[index]) != NULL) {
        return tree_node_add_child(parent, parent->string, index, 1, "sub_delims");
    }
    return NULL;
}

// connection_option = token
tree_node* connection_option(tree_node* parent) {
    tree_node* node_token = tree_node_add_node(parent, "connection_option");
    if (token(node_token) == NULL) {
        tree_node_free(node_token);
        return NULL;
    }
    return node_token;
}
// HEXDIG = DIGIT / "A" / "B" / "C" / "D" / "E" / "F"
tree_node* HEXDIG(tree_node* parent) {
    char valids[] = "ABCDEF";
    tree_node* node_digit = tree_node_add_node(parent, "HEXDIG");
    if (DIGIT(node_digit)) {
        return node_digit;
    }
    int index = get_start(parent);
    if (strchr(valids, parent->string[index]) != NULL) {
        node_digit->length_string = 1;
        return node_digit;
    }
    tree_node_free(node_digit);
    return NULL;
}

// IPvFuture = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
tree_node* IPvFuture(tree_node* parent) {
    tree_node* node_IPvFuture = tree_node_add_node(parent, "IPvFuture");
    if (
        check_sa(node_IPvFuture, "v") != NULL &&
        at_least_x(node_IPvFuture, HEXDIG, 1) &&
        check_sa(node_IPvFuture, ".") != NULL &&
        (unreserved(node_IPvFuture) != NULL ||
         sub_delims(node_IPvFuture) != NULL ||
         check_sa(node_IPvFuture, ":") != NULL)) {
        while (unreserved(node_IPvFuture) != NULL || sub_delims(node_IPvFuture) != NULL || check_sa(node_IPvFuture, ":"))
            ;
        return node_IPvFuture;
    }
    tree_node_free(node_IPvFuture);
    return NULL;
}

// dec-octet = "25" %x30-35 / "2" %x30-34 DIGIT / "1" 2DIGIT / %x31-39 DIGIT / DIGIT
tree_node* dec_octet(tree_node* parent) {
    int index = get_start(parent);
    tree_node* node_dec_octet = tree_node_add_node(parent, "dec_octet");
    tree_node* node_tmp = tree_node_tmp(node_dec_octet);
    if (check_sa(node_tmp, "2")) {
        index++;
        if (check_sa(node_tmp, "5")) {
            index++;
            if (parent->string[index] >= '0' && parent->string[index] <= '5') {
                tree_node_add_child(node_tmp, parent->string, index, 1, "0-5");
                move_childs(node_tmp, node_dec_octet);
                tree_node_free(node_tmp);
                return node_dec_octet;
            }
            tree_node_free(node_tmp);
        }
        if (parent->string[index] >= '0' && parent->string[index] <= '4') {
            tree_node_add_child(node_tmp, parent->string, index, 1, "0-4");
            if (DIGIT(node_tmp) != NULL) {
                move_childs(node_tmp, node_dec_octet);
                tree_node_free(node_tmp);
                return node_dec_octet;
            }
            tree_node_free(node_tmp);
        }
        tree_node_free(node_tmp);
    }
    node_tmp = tree_node_tmp(node_dec_octet);
    index = get_start(parent);
    if (check_sa(node_tmp, "1")) {
        index++;
        if (DIGIT(node_tmp) != NULL && DIGIT(node_tmp) != NULL) {
            move_childs(node_tmp, node_dec_octet);
            tree_node_free(node_tmp);
            return node_dec_octet;
        }
        tree_node_free(node_tmp);
    }
    node_tmp = tree_node_tmp(node_dec_octet);
    index = get_start(parent);
    if (parent->string[index] >= '1' && parent->string[index] <= '9') {
        tree_node_add_child(node_tmp, parent->string, index, 1, "1-9");
        if (DIGIT(node_tmp) != NULL) {
            move_childs(node_tmp, node_dec_octet);
            tree_node_free(node_tmp);
            return node_dec_octet;
        }
        tree_node_free(node_tmp);
    }
    if (DIGIT(node_dec_octet) != NULL) {
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
// field_vchar = VCHAR / obs_text
tree_node* field_vchar(tree_node* parent) {
    tree_node* node_field_vchar = tree_node_add_node(parent, "field_vchar");
    if (VCHAR(node_field_vchar) != NULL ||
        obs_text(node_field_vchar) != NULL) {
        return node_field_vchar;
    }
    tree_node_free(node_field_vchar);
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
// field_content = field_vchar *( *( SP / HTAB ) field_vchar )
//le prof est tres intelligent il a un qi superieur a celui d'un escargot
tree_node* field_content(tree_node* parent) {
    tree_node* node_field_content = tree_node_add_node(parent, "field_content");
    if (field_vchar(node_field_content) == NULL) {
        tree_node_free(node_field_content);
        return NULL;
    }
    tree_node* node_tmp = tree_node_tmp(node_field_content);
    while (SP(node_tmp) != NULL || HTAB(node_tmp) != NULL){
        field_vchar(node_tmp);
    }
    move_childs(node_tmp, node_field_content);
    tree_node_free(node_tmp);
    return node_field_content;
}

// field_value = *( field_content / obs_fold )
tree_node* field_value(tree_node* parent) {
    tree_node* node_field_value = tree_node_add_node(parent, "field_value");
    while (field_content(node_field_value) != NULL || obs_fold(node_field_value) != NULL);
    return node_field_value;
}

// pct_encoded = "%" HEXDIG HEXDIG
tree_node* pct_encoded(tree_node* parent) {
    tree_node* node_pct_encoded = tree_node_add_node(parent, "pct_encoded");
    if (check_sa(node_pct_encoded, "%")) {
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
    tree_node* node_pchar = tree_node_add_node(parent, "pchar");
    if (unreserved(node_pchar) != NULL ||
        pct_encoded(node_pchar) != NULL ||
        sub_delims(node_pchar) != NULL ||
        check_sa(node_pchar, ":") ||
        check_sa(node_pchar, "@")) {
        return node_pchar;
    }
    tree_node_free(node_pchar);
    return NULL;
}

// segment = *pchar
tree_node* segment(tree_node* parent) {
    tree_node* node_segment = tree_node_add_node(parent, "segment");
    while (pchar(node_segment) != NULL)
        ;
    return node_segment;
}

// query = *( pchar / "/" / "?" )
tree_node* query(tree_node* parent) {
    tree_node* node_query = tree_node_add_node(parent, "query");
    while (pchar(node_query) != NULL ||
           check_sa(node_query, "/") ||
           check_sa(node_query, "?"))
        ;
    return node_query;
}

// reg_name = *( unreserved / pct_encoded / sub_delims )
tree_node* reg_name(tree_node* parent) {
    tree_node* node_reg_name = tree_node_add_node(parent, "reg_name");
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
// uri_host = host
tree_node* uri_host(tree_node* parent) {
    tree_node* node_uri_host = tree_node_add_node(parent, "uri_host");
    if (host(node_uri_host) == NULL) {
        tree_node_free(node_uri_host);
        return NULL;
    }
    return node_uri_host;
}
// obs_fold = CRLF 1*( SP / HTAB )
tree_node* obs_fold(tree_node* parent) {
    tree_node* node_obs_fold = tree_node_add_node(parent, "obs_fold");
    if (CRLF(node_obs_fold) == NULL) {
        tree_node_free(node_obs_fold);
        return NULL;
    }
    if (SP(node_obs_fold) == NULL || HTAB(node_obs_fold) == NULL) {
        tree_node_free(node_obs_fold);
        return NULL;
    }
    bool end = false;
    while (!end) {
        tree_node* node_tmp = tree_node_tmp(node_obs_fold);
        if (SP(node_tmp) != NULL || HTAB(node_tmp) != NULL) {
            move_childs(node_tmp, node_obs_fold);
        } else {
            end = true;
        }
        tree_node_free(node_tmp);
    }
    return node_obs_fold;
}
// host = IP_literal / IPv4address / reg_name
tree_node* host(tree_node* parent) {
    tree_node* node_host = tree_node_add_node(parent, "host");
    if (IP_literal(node_host) != NULL || IPv4address(node_host) != NULL || reg_name(node_host) != NULL) {
        return node_host;
    }
    tree_node_free(node_host);
    return NULL;
}

// Host = uri_host [ ":" port ]
tree_node* Host(tree_node* parent) {
    tree_node* node_Host = tree_node_add_node(parent, "Host");
    if (uri_host(node_Host) == NULL) {
        tree_node_free(node_Host);
        return NULL;
    }
    tree_node* node_tmp = tree_node_tmp(node_Host);
    if (check_sa(node_tmp, ":") != NULL && port(node_tmp) != NULL) {
        move_childs(node_tmp, node_Host);
    }
    tree_node_free(node_tmp);
    return node_Host;
}

// OWS = *( SP / HTAB )
tree_node* OWS(tree_node* parent) {
    tree_node* node_OWS = tree_node_add_node(parent, "OWS");
    while (SP(node_OWS) != NULL || HTAB(node_OWS) != NULL){
        printf("childs:%d\n",node_OWS->childs_count);
    }
    debug(node_OWS,__LINE__);
    return node_OWS;
}

// Host_header = "Host" ":" OWS Host OWS
tree_node* Host_header(tree_node* parent) {
    tree_node* node_Host_header = tree_node_add_node(parent, "Host_header");
    if (check_sa(node_Host_header, "Host:") == NULL ||
        OWS(node_Host_header) == NULL ||
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
    tree_node* node_IPv4address = tree_node_add_node(parent, "IPv4address");
    for (int i = 0; i < 3; i++) {
        if (dec_octet(node_IPv4address) == NULL ||
            check_sa(node_IPv4address, ".") == NULL) {
            tree_node_free(node_IPv4address);
            return NULL;
        }
    }
    return node_IPv4address;
}

// h16           = 1*4HEXDIG
tree_node* h16(tree_node* parent) {
    tree_node* node_h16 = tree_node_add_node(parent, "h16");
    if(HEXDIG(node_h16) == NULL) {
        tree_node_free(node_h16);
        return NULL;
    }
    for (int i = 1; i < 4; i++) {
        HEXDIG(node_h16);
    }
    return node_h16;
}
// ls32          = ( h16 ":" h16 ) / IPv4address
tree_node* ls32(tree_node* parent) {
    tree_node* node_ls32 = tree_node_add_node(parent, "ls32");
    if (h16(node_ls32) == NULL ||
        check_sa(node_ls32, ":") ||
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

// Can you correct the following code ? I'm not sure it's correct
tree_node* IPv6address(tree_node* parent) {
    tree_node* node_IPv6address = tree_node_add_node(parent, "IPv6address");
    int count = 0;

    while (check_sa(node_IPv6address, ":")) {
        if (h16(node_IPv6address) == NULL || check_sa(node_IPv6address, ":") == NULL) {
            tree_node_free(node_IPv6address);
            return NULL;
        }
        count++;
    }

    if (count > 8) {
        tree_node_free(node_IPv6address);
        return NULL;
    }

    if (count == 8) {
        if (ls32(node_IPv6address) == NULL || check_sa(node_IPv6address, "::") == NULL) {
            tree_node_free(node_IPv6address);
            return NULL;
        }
        return node_IPv6address;
    }

    if (count == 0) {
        if (check_sa(node_IPv6address, "::") == NULL) {
            tree_node_free(node_IPv6address);
            return NULL;
        }
    }

    for (int i = 0; i < 5 - count; i++) {
        if (h16(node_IPv6address) == NULL || check_sa(node_IPv6address, ":") == NULL) {
            tree_node_free(node_IPv6address);
            return NULL;
        }
    }

    if (check_sa(node_IPv6address, "::") == NULL || ls32(node_IPv6address) == NULL) {
        tree_node_free(node_IPv6address);
        return NULL;
    }

    return node_IPv6address;
}

// Expect_header = "Expect" ":" OWS Expect OWS
tree_node* Expect_header(tree_node* parent) {
    tree_node* node_Expect_header = tree_node_add_node(parent, "Expect_header");
    if (check_sa(node_Expect_header, "Expect:") == NULL ||
        OWS(node_Expect_header) == NULL ||
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
    if (parent->string[index] == 0x21 ||
        (parent->string[index] >= 0x23 && parent->string[index] <= 0x2B) ||
        (parent->string[index] >= 0x2D && parent->string[index] <= 0x3A) ||
        (parent->string[index] >= 0x3C && parent->string[index] <= 0x5B) ||
        (parent->string[index] >= 0x5D && parent->string[index] <= 0x7E)) {
        return tree_node_add_child(parent, parent->string, index, 1, "cookie_octet");
    }
    return NULL;
}

// DQUOTE = %x22
tree_node* DQUOTE(tree_node* parent) {
    int index = get_start(parent);
    if (parent->string[index] == 0x22) {
        return tree_node_add_child(parent, parent->string, index, 1, "DQUOTE");
        ;
    }
    return NULL;
}

// qdtext = HTAB / SP / "!" / %x23-5B / %x5D-7E / obs_text
tree_node* qdtext(tree_node* parent) {
    tree_node* node_qdtext = tree_node_add_node(parent, "qdtext");
    if (HTAB(node_qdtext) ||
        SP(node_qdtext) ||
        obs_text(node_qdtext)) {
        return node_qdtext;
    }
    if (check_sa(node_qdtext, "!")) {
        return node_qdtext;
    }
    int index = get_start(parent);
    if (parent->string[index] >= 0x23 && parent->string[index] <= 0x5B) {
        tree_node_add_child(node_qdtext, parent->string, index, 1, "%x23-5B");
        return node_qdtext;
    }
    if (parent->string[index] >= 0x5D && parent->string[index] <= 0x7E) {
        tree_node_add_child(node_qdtext, parent->string, index, 1, "%x5D-7E");
        return node_qdtext;
    }
    tree_node_free(node_qdtext);
    return NULL;
}
// cookie_value = ( DQUOTE *cookie_octet DQUOTE ) / *cookie_octet
tree_node* cookie_value(tree_node* parent) {
    tree_node* node_cookie_value = tree_node_add_node(parent, "cookie_value");
    tree_node* node_tmp = tree_node_tmp(node_cookie_value);
    if (DQUOTE(node_tmp)) {
        while (cookie_octet(node_tmp) != NULL)
            ;
        if (DQUOTE(node_tmp) == NULL) {
            tree_node_free(node_tmp);
            return NULL;
        }
        move_childs(node_tmp, node_cookie_value);
        tree_node_free(node_tmp);
        return node_cookie_value;
    }
    while (cookie_octet(node_cookie_value) != NULL)
        ;
    return node_cookie_value;
}

// cookie_pair = cookie_name "=" cookie_value
tree_node* cookie_pair(tree_node* parent) {
    tree_node* node_cookie_pair = tree_node_add_node(parent, "cookie_pair");
    if (
        cookie_name(node_cookie_pair) == NULL ||
        check_sa(node_cookie_pair, "=") == NULL ||
        cookie_value(node_cookie_pair) == NULL) {
        tree_node_free(node_cookie_pair);
        return NULL;
    }
    return node_cookie_pair;
}
// IP_literal    = "[" ( IPv6address / IPvFuture  ) "]"
tree_node* IP_literal(tree_node* parent) {
    tree_node* node_IP_literal = tree_node_add_node(parent, "IP_literal");
    if (check_sa(node_IP_literal, "[") &&
        (IPv6address(node_IP_literal) || IPvFuture(node_IP_literal)) &&
        check_sa(node_IP_literal, "]")) {
        return node_IP_literal;
    }
    tree_node_free(node_IP_literal);
    return NULL;
}
// cookie_string = cookie_pair *( ";" SP cookie_pair )
tree_node* cookie_string(tree_node* parent) {
    tree_node* node_cookie_string = tree_node_add_node(parent, "cookie_string");
    if (cookie_pair(node_cookie_string) == NULL) {
        tree_node_free(node_cookie_string);
        return NULL;
    }
    bool end = false;
    while (!end) {
        tree_node* node_tmp = tree_node_tmp(node_cookie_string);
        if (check_sa(node_tmp, ";") &&
            SP(node_cookie_string) &&
            cookie_pair(node_cookie_string)) {
            move_childs(node_tmp, node_cookie_string);
        } else {
            end = true;
        }
        tree_node_free(node_tmp);
    }
    return node_cookie_string;
}
// Cookie_header = "Cookie:" OWS cookie_string OWS
tree_node* Cookie_header(tree_node* parent) {
    tree_node* node_Cookie_header = tree_node_add_node(parent, "Cookie_header");
    if (check_sa(node_Cookie_header, "Cookie:") == NULL ||
        OWS(node_Cookie_header) == NULL ||
        cookie_string(node_Cookie_header) == NULL ||
        OWS(node_Cookie_header) == NULL) {
        tree_node_free(node_Cookie_header);
        return NULL;
    }
    return node_Cookie_header;
}
// quoted_pair = "\" ( HTAB / SP / VCHAR / obs_text )
tree_node* quoted_pair(tree_node* parent) {
    tree_node* node_quoted_pair = tree_node_add_node(parent, "quoted_pair");
    if (check_sa(node_quoted_pair, "\\") != NULL &&
        (HTAB(node_quoted_pair) ||
         SP(node_quoted_pair) ||
         VCHAR(node_quoted_pair) ||
         obs_text(node_quoted_pair))) {
        return node_quoted_pair;
    }
    tree_node_free(node_quoted_pair);
    return NULL;
}
// quoted_string = DQUOTE *( qdtext / quoted_pair ) DQUOTE
tree_node* quoted_string(tree_node* parent) {
    tree_node* node_quoted_string = tree_node_add_node(parent, "quoted_string");
    if (DQUOTE(node_quoted_string) == NULL) {
        tree_node_free(node_quoted_string);
        return NULL;
    }
    while (qdtext(node_quoted_string) != NULL || quoted_pair(node_quoted_string) != NULL)
        ;
    if (DQUOTE(node_quoted_string) == NULL) {
        tree_node_free(node_quoted_string);
        return NULL;
    }
    return node_quoted_string;
}
// parameter = token "=" ( token / quoted_string )
tree_node* parameter(tree_node* parent) {
    tree_node* node_parameter = tree_node_add_node(parent, "parameter");
    if (
        token(node_parameter) != NULL &&
        check_sa(node_parameter, "=") != NULL &&
        (token(node_parameter) != NULL ||
         quoted_string(node_parameter) != NULL)) {
        return node_parameter;
    }
    tree_node_free(node_parameter);
    return NULL;
}
// media_type = type "/" subtype *( OWS ";" OWS parameter )
tree_node* media_type(tree_node* parent) {
    tree_node* node_media_type = tree_node_add_node(parent, "media_type");
    if (
        type(node_media_type) == NULL ||
        check_sa(node_media_type, "/") == NULL ||
        subtype(node_media_type) == NULL) {
        tree_node_free(node_media_type);
        return NULL;
    }
    bool end = false;
    while (!end) {
        tree_node* node_tmp = tree_node_tmp(node_media_type);
        if (OWS(node_tmp) && check_sa(node_tmp, ";") && OWS(node_tmp) && parameter(node_tmp)) {
            move_childs(node_tmp, node_media_type);
        } else {
            end = true;
        }
        tree_node_free(node_tmp);
    }
    return node_media_type;
}
// Content_Type = media_type
tree_node* Content_Type(tree_node* parent) {
    tree_node* node_Content_Type = tree_node_add_node(parent, "Content_Type");
    if (media_type(node_Content_Type) == NULL) {
        tree_node_free(node_Content_Type);
        return NULL;
    }
    return node_Content_Type;
}
// Content_Type_header = "Content-Type:" OWS Content_Type OWS
tree_node* Content_Type_header(tree_node* parent) {
    tree_node* node_Content_Type_header = tree_node_add_node(parent, "Content_Type_header");
    if (check_sa(node_Content_Type_header, "Content-Type:") == NULL ||
        OWS(node_Content_Type_header) == NULL ||
        Content_Type(node_Content_Type_header) == NULL ||
        OWS(node_Content_Type_header) == NULL) {
        tree_node_free(node_Content_Type_header);
        return NULL;
    }
    return node_Content_Type_header;
}
// Content_Length = 1*DIGIT
tree_node* Content_Length(tree_node* parent) {
    tree_node* node_Content_Length = tree_node_add_node(parent, "Content_Length");
    if (DIGIT(node_Content_Length) == NULL) {
        tree_node_free(node_Content_Length);
        return NULL;
    }
    while (DIGIT(node_Content_Length))
        ;
    return node_Content_Length;
}
// Content_Length_header = "Content-Length:" OWS Content_Length OWS
tree_node* Content_Length_header(tree_node* parent) {
    tree_node* node_Content_Length_header = tree_node_add_node(parent, "Content_Length_header");
    if (check_sa(node_Content_Length_header, "Content-Length:") == NULL ||
        OWS(node_Content_Length_header) == NULL ||
        Content_Length(node_Content_Length_header) == NULL ||
        OWS(node_Content_Length_header) == NULL) {
        tree_node_free(node_Content_Length_header);
        return NULL;
    }
    return node_Content_Length_header;
}

// Connection = *( "," OWS ) connection_option *( OWS "," [ OWS connection_option ] )
tree_node* Connection(tree_node* parent) {
    tree_node* node_Connection = tree_node_add_node(parent, "Connection");
    bool end = false;
    tree_node* node_tmp;
    while (!end) {
        node_tmp = tree_node_tmp(node_Connection);
        if (check_sa(node_tmp, ",") && OWS(node_tmp)) {
            move_childs(node_tmp, node_Connection);
        } else {
            end = true;
        }
        tree_node_free(node_tmp);
    }
    if (connection_option(node_Connection) == NULL) {
        tree_node_free(node_Connection);
        return NULL;
    }
    end = false;
    while (!end) {
        node_tmp = tree_node_tmp(node_Connection);
        if (OWS(node_tmp) && check_sa(node_tmp, ",")) {
            move_childs(node_tmp, node_Connection);
            if (OWS(node_tmp) && connection_option(node_tmp)) {
                move_childs(node_tmp, node_Connection);
            }
        } else {
            end = true;
        }
        tree_node_free(node_tmp);
    }
    return node_Connection;
}

// Connection_header = "Connection" ":" OWS Connection OWS
tree_node* Connection_header(tree_node* parent) {
    // tree_node_print_all(getRootTree(), 0);
    // tree_node_print(getRootTree(), 0);
    tree_node* node_Connection_header = tree_node_add_node(parent, "Connection_header");
    if (check_sa(node_Connection_header, "Connection:") != NULL &&
        OWS(node_Connection_header) != NULL &&
        Connection(node_Connection_header) != NULL &&
        OWS(node_Connection_header) != NULL) {
        return node_Connection_header;
    }
    // if (check_sa(node_Connection_header, "Connection:") != NULL &&
    //     OWS(node_Connection_header) != NULL &&
    //     Connection(node_Connection_header) != NULL &&
    //     OWS(node_Connection_header) != NULL) {
    //     return node_Connection_header;
    // }
    tree_node_free(node_Connection_header);
    return NULL;
}
// BWS = *( SP / HTAB )
tree_node* BWS(tree_node* parent) {
    tree_node* node_BWS = tree_node_add_node(parent, "BWS");
    while (check_sa(node_BWS, " ") || check_sa(node_BWS, "\t"))
        ;
    return node_BWS;
}
// transfer_parameter = token BWS "=" BWS ( token / quoted_string )
tree_node* transfer_parameter(tree_node* parent) {
    tree_node* node_transfer_parameter = tree_node_add_node(parent, "transfer_parameter");
    if (
        token(node_transfer_parameter) != NULL &&
        BWS(node_transfer_parameter) != NULL &&
        check_sa(node_transfer_parameter, "=") != NULL &&
        BWS(node_transfer_parameter) != NULL &&
        (token(node_transfer_parameter) != NULL ||
         quoted_string(node_transfer_parameter) != NULL)) {
        return node_transfer_parameter;
    }
    tree_node_free(node_transfer_parameter);
    return NULL;
}

// transfer_extension = token [ "=" ( token / quoted_string ) ]
tree_node* transfer_extension(tree_node* parent) {
    tree_node* node_transfer_extension = tree_node_add_node(parent, "transfer_extension");
    if (token(node_transfer_extension) == NULL) {
        tree_node_free(node_transfer_extension);
        return NULL;
    }
    tree_node* node_tmp = tree_node_tmp(node_transfer_extension);
    if (
        check_sa(node_tmp, "=") != NULL &&
        (token(node_tmp) != NULL || quoted_string(node_tmp) != NULL)) {
        move_childs(node_tmp, node_transfer_extension);
    }
    tree_node_free(node_tmp);
    return node_transfer_extension;
}
// Transfer_Encoding_header = "Transfer-Encoding" ":" OWS Transfer_Encoding OWS
tree_node* Transfer_Encoding_header(tree_node* parent) {
    tree_node* node_Transfer_Encoding_header = tree_node_add_node(parent, "Transfer_Encoding_header");
    if (
        check_sa(node_Transfer_Encoding_header, "Transfer-Encoding:") == NULL ||
        OWS(node_Transfer_Encoding_header) == NULL ||
        Transfert_encoding(node_Transfer_Encoding_header) == NULL ||
        OWS(node_Transfer_Encoding_header) == NULL) {
        tree_node_free(node_Transfer_Encoding_header);
        return NULL;
    }
    return node_Transfer_Encoding_header;
}
// header_field =  Connection_header / Content_Length_header / Content_Type_header / Cookie_header / Transfer_Encoding_header / Expect_header / Host_header / ( field_name ":" OWS field_value OWS )
tree_node* header_field(tree_node* parent) {
    tree_node* node_header_field = tree_node_add_node(parent, "header_field");
    if (
        Connection_header(node_header_field) != NULL ||
        Content_Length_header(node_header_field) != NULL ||
        Content_Type_header(node_header_field) != NULL ||
        Cookie_header(node_header_field) != NULL ||
        Transfer_Encoding_header(node_header_field) != NULL ||
        Expect_header(node_header_field) != NULL ||
        Host_header(node_header_field) != NULL) {
        return node_header_field;
    }
    if (field_name(node_header_field) != NULL &&
        check_sa(node_header_field, ":") != NULL &&
        OWS(node_header_field) != NULL &&
        field_value(node_header_field) != NULL &&
        OWS(node_header_field) != NULL) {
        return node_header_field;
    }
    tree_node_free(node_header_field);
    return NULL;
}
// transfert_coding = "chunked" / "compress" / "deflate" / "gzip" / transfer_extension
tree_node* transfert_coding(tree_node* parent) {
    tree_node* node_transfert_coding = tree_node_add_node(parent, "transfert_coding");
    debug(node_transfert_coding,__LINE__);
    if (check_sa(node_transfert_coding, "chunked") ||
        check_sa(node_transfert_coding, "compress") ||
        check_sa(node_transfert_coding, "deflate") ||
        check_sa(node_transfert_coding, "gzip") ||
        transfer_extension(node_transfert_coding)) {
        debug(node_transfert_coding,__LINE__);
        return node_transfert_coding;
    }
    tree_node_free(node_transfert_coding);
    return NULL;
}
// HTTP_name = %x48.54.54.50
tree_node* HTTP_name(tree_node* parent) {
    tree_node* node_HTTP_name = tree_node_add_node(parent, "HTTP_name");
    if (check_sa(node_HTTP_name, "HTTP") == NULL) {
        tree_node_free(node_HTTP_name);
        return NULL;
    }
    return node_HTTP_name;
}
// HTTP_version = HTTP_name "/" DIGIT "." DIGIT
tree_node* HTTP_version(tree_node* parent) {
    tree_node* node_HTTP_version = tree_node_add_node(parent, "HTTP_version");
    if (HTTP_name(node_HTTP_version) == NULL ||
        check_sa(node_HTTP_version, "/") == NULL ||
        DIGIT(node_HTTP_version) == NULL ||
        check_sa(node_HTTP_version, ".") == NULL ||
        DIGIT(node_HTTP_version) == NULL) {
        tree_node_free(node_HTTP_version);
        return NULL;
    }
    return node_HTTP_version;
}
// absolute_path = 1*( "/" segment )
tree_node* absolute_path(tree_node* parent) {
    tree_node* node_absolute_path = tree_node_add_node(parent, "absolute_path");
    if (check_sa(node_absolute_path, "/") == NULL || segment(node_absolute_path) == NULL) {
        tree_node_free(node_absolute_path);
        return NULL;
    }
    while (check_sa(node_absolute_path, "/") && segment(node_absolute_path) != NULL)
        ;
    return node_absolute_path;
}

// origin_form = absolute_path [ "?" query ]
tree_node* origin_form(tree_node* parent) {
    tree_node* node_origin_form = tree_node_add_node(parent, "origin_form");
    if (absolute_path(node_origin_form) == NULL) {
        tree_node_free(node_origin_form);
        return NULL;
    }
    tree_node* node_tmp = tree_node_tmp(node_origin_form);
    if (check_sa(node_tmp, "?") && query(node_tmp)) {
        move_childs(node_tmp, node_origin_form);
    }
    tree_node_free(node_tmp);
    return node_origin_form;
}
// request_target = origin_form
tree_node* request_target(tree_node* parent) {
    tree_node* node_request_target = tree_node_add_node(parent, "request_target");
    if (origin_form(node_request_target) == NULL) {
        tree_node_free(node_request_target);
        return NULL;
    }
    return node_request_target;
}
// request_line = method SP request_target SP HTTP_version CRLF
tree_node* request_line(tree_node* parent) {
    tree_node* node_request_line = tree_node_add_node(parent, "request_line");
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
    tree_node* node_start_line = tree_node_add_node(parent, "start_line");
    if (request_line(node_start_line) == NULL) {
        tree_node_free(node_start_line);
        return NULL;
    }
    return node_start_line;
}
// Transfert_encoding = *( "," OWS ) transfert_coding *( OWS "," [ OWS transfert_coding ] )
tree_node* Transfert_encoding(tree_node* parent) {
    tree_node* node_Transfert_encoding = tree_node_add_node(parent, "Transfert_encoding");
    bool end = false;
    tree_node* node_tmp=NULL;
    while (!end) {
        node_tmp = tree_node_tmp(node_Transfert_encoding);
        if (check_sa(node_tmp, ",") != NULL && OWS(node_tmp)!=NULL) {
            move_childs(node_tmp, node_Transfert_encoding);
        } else {
            end = true;
        }
        debug(node_Transfert_encoding,__LINE__);
    }
    if (transfert_coding(node_Transfert_encoding) == NULL) {
        tree_node_free(node_Transfert_encoding);
        return NULL;
    }
    end = false;
    while (!end) {
        node_tmp = tree_node_tmp(node_Transfert_encoding);
        if (OWS(node_tmp)!=NULL && check_sa(node_tmp, ",") != NULL) {
            move_childs(node_tmp, node_Transfert_encoding);
            tree_node_free(node_tmp);
            node_tmp = tree_node_tmp(node_Transfert_encoding);
            debug(node_Transfert_encoding,__LINE__);
            if (OWS(node_tmp)!=NULL && transfert_coding(node_tmp) != NULL) {
                move_childs(node_tmp, node_Transfert_encoding);
                debug(node_Transfert_encoding,__LINE__);
            }
        } else {
            end = true;
        }
        tree_node_free(node_tmp);
        debug(node_Transfert_encoding,__LINE__);
    }
    while(1);
    return node_Transfert_encoding;
}

// HTTP_message = start_line *( header_field CRLF ) CRLF [ message_body ]
tree_node* HTTP_message(tree_node* parent) {
    tree_node* node_HTTP_message = tree_node_add_node(parent, "HTTP_message");
    if (start_line(node_HTTP_message) == NULL) {
        tree_node_free(node_HTTP_message);
        return NULL;
    }
    bool end = false;
    while (!end) {
        tree_node* node_tmp = tree_node_tmp(node_HTTP_message);
        if (header_field(node_tmp) != NULL) {
            tree_node_print_all(node_tmp, 0);
            tree_node_print(node_tmp, 0);
            if (CRLF(node_tmp) != NULL) {
                move_childs(node_tmp, node_HTTP_message);
            } else {
                end = true;
            }
        } else {
            end = true;
        }
        tree_node_free(node_tmp);
    }
    if (CRLF(node_HTTP_message) == NULL) {
        tree_node_free(node_HTTP_message);
        return NULL;
    }
    message_body(node_HTTP_message);
    return node_HTTP_message;
}