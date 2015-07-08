/*
 ============================================================================
 Name        : main.c
 Author      : Hsiang-Ho Lin
 Version     : 1.0.0
 Copyright   : GPLv2
 Description : 
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fuzzy.h"

#define BUFF_SIZE 1024

int main(int argc, char** argv)
{
    FILE *fin = fopen("../wordlist/list.txt", "r");
    FILE *fconf = fopen("fuzzyi.conf", "r");
    char buf[BUFF_SIZE] = {0};
    char query[BUFF_SIZE] = {0};
    int id = 0;
    int going = 1;
    word_data data;

    fuzzy_struct* fuzs = fuzzy_new();

    if(!fin || !fconf || !fuzs)
        goto exit;

    fuzzy_load_conf(fuzs, fconf);

    while(fscanf(fin, "%s", buf) != EOF)
    {
		data.pattern = buf;
		data.id = id;
        if(fuzzy_load_search_key(data, fuzs, DUPKEY_IGNORE) != 0)
            goto exit;
		id++;
    }

    while(going)
    {
        printf ("Typing (1 to leave):");
        fgets(query, BUFF_SIZE, stdin); 

        if(atoi(query) == 1)
            break;

	    query[strlen(query)-2] = '\0';

        if(fuzzy_search(fuzs, buf, query) == FUZZYI_HIT)
        {
            printf("Hit :)\n");
        }
        else
        {
            printf("Miss :(\n");
        }
    }

exit:
    fuzzy_free(fuzs);
    if(fin)
        fclose(fin);
    if(fconf)
        fclose(fconf);
    return 0;
}
