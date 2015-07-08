/*
 ============================================================================
 Name        : fuzzy.h
 Author      : Hsiang-Ho Lin
 Version     : 1.0.0
 Copyright   : GPLv2
 Description : 
 ============================================================================
 */

#ifndef _FUZZY_H_
#define _FUZZY_H_

#define DUPKEY_IGNORE	1
#define DUPKEY_ABORT	2

#define FUZZYI_HIT	1
#define FUZZYI_MISS	0

typedef struct FUZZY_STRUCT_ fuzzy_struct;

typedef struct FUZZY_W_DATA_ word_data;
struct FUZZY_W_DATA_
{
    char* pattern;
	int id;
};

fuzzy_struct* fuzzy_new(void);
int fuzzy_load_search_key(word_data data, fuzzy_struct* fuzs, int opt);
void fuzzy_free(fuzzy_struct *fuzs);
int fuzzy_search(fuzzy_struct *fuzs, char *result, char *query);
void fuzzy_load_conf(fuzzy_struct *fuzs, FILE *fconf);

#endif
