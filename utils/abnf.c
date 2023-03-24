#include "abnf.h"
void insert_rule(abnf_rule **head_ref,char* name,char* value) {
    abnf_rule *new_rule = (abnf_rule *)malloc(sizeof(abnf_rule));
    new_rule->name = name;
    new_rule->value=value;
    new_rule->next = NULL;
    if (*head_ref == NULL) {
        *head_ref = new_rule;
        return;
    }
    abnf_rule *last = *head_ref;
    while (last->next != NULL) last = last->next;
    last->next = new_rule;
}
void print(abnf_rule *head) {
    abnf_rule *start = head;
    while (start != NULL) {
        printf("||%s=%s\n", start->name, start->value);
        start = start->next;
    }
}
uint16_t count(char *string, char sep) {
    size_t i = 0;
    for (; string[i] != 0 && string[i] != sep; i++)
        ;
    return i;
}
char* generate_all
int main() {
    // open and read the file test.abnf
    FILE *fp = fopen("/home/flo/ESISAR_3AS1/PROJETS/SERVER_WEB/utils/test.abnf", "r");
    char *buffer = NULL;
    size_t len = 0;
    ssize_t read;
    abnf_rule *abnf_head = NULL;
    while ((read = getline(&buffer, &len, fp)) != -1) {
        buffer = trim_space(buffer);
        if (buffer[0] == ';') continue;
        uint16_t cutting = count(buffer, '=');
        insert_rule(&abnf_head, trim_space(copy_sub_str(buffer, 0, cutting)), trim_space(copy_sub_str(buffer, cutting+1, -1)));
        // printf("%s\n", buffer);
    }
    printf("%p\n", abnf_head);
    printf("=>%s\n", abnf_head->name);
    print(abnf_head);

    fclose(fp);
    return 0;
}
