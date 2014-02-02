/*
 ============================================================================
 Name        : fuzzy.c
 Author      : Hsiang-Ho Lin
 Version     : 1.0.0
 Copyright   : GPLv2
 Description : 
 ============================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "fuzzy.h"

#define BUFF_SIZE 1024

typedef struct TRIE_NODE_ trie_node;
struct TRIE_NODE_
{
    trie_node *next_list;
    trie_node *next_one;
    int is_final;
    char* pattern;
    char key;
	int id;
};

struct FUZZY_STRUCT_
{
    trie_node *root;
    char** fuzzy_map;
};

static inline trie_node* trie_node_get_children_head(trie_node* n)
{
	return n->next_list;
}

static inline trie_node* trie_node_get_next_sibling(trie_node* n)
{
	return n->next_one;
}

void trie_node_set_next_sibling(trie_node* node, trie_node* next)
{
	node->next_one = next;
}

void trie_node_set_next_head(trie_node* node, trie_node* next)
{
	node->next_list = next;
}

trie_node* trie_node_lookup_children(trie_node* n, char ch)
{
    trie_node *next = trie_node_get_children_head(n);
    while(next)
    {
        if(next->key == ch)
            break;
        next = trie_node_get_next_sibling(next);
    }
    return next;
}

void trie_node_set_children(trie_node* p_n, trie_node* c_n)
{
    trie_node *next = trie_node_get_children_head(p_n);
    if(next == NULL)
		trie_node_set_next_head(p_n, c_n);
    while(next)
    {
        if(trie_node_get_next_sibling(next) == NULL)
        {
			trie_node_set_next_sibling(next, c_n);
            break;
        }
		next = trie_node_get_next_sibling(next);
    }
}

trie_node* trie_node_new(char key)
{
    trie_node* n = (trie_node*) malloc(sizeof(trie_node));
    if(!n)
        return NULL;
    n->next_list = NULL;
    n->next_one = NULL;
    n->pattern = NULL;
    n->key = key;
    n->is_final = 0;
	n->id = -1;
    return n;
}

static inline int trie_node_get_key(trie_node* n)
{
	return n->key;
}

static inline int trie_node_is_final(trie_node* n)
{
	return n->is_final;
}

static inline char* trie_node_get_pattern(trie_node* n)
{
	return n->pattern;
}

static inline word_data trie_node_get_data(trie_node* n)
{
	word_data data;
	data.pattern = n->pattern;
	data.id = n->id;
	return data;
}

void trie_node_free_children(trie_node *node)
{
    trie_node *e_n = trie_node_get_children_head(node);
    trie_node *tmp = NULL;
    while(e_n)
    {
        trie_node_free_children(e_n);
        tmp = trie_node_get_next_sibling(e_n);
        free(trie_node_get_pattern(e_n));
        free(e_n);
        e_n = tmp;
    }
}

int trie_build(word_data data, fuzzy_struct *fuzs, int opt)
{
    if(!fuzs || !fuzs->root) 
        return -1;

    int i;
    trie_node *n = fuzs->root;
    trie_node *prev = NULL;
    int len = strlen(data.pattern);
	char* str = data.pattern;

    for(i = 0; i < len; i++)
    {
        prev = n;
        n = trie_node_lookup_children(n, str[i]);
        if(!n)
        {
            n = trie_node_new(str[i]);
            if(!n)
                return -1;
            trie_node_set_children(prev,n);
        }
        if( i == len -1)
        {
            if(n->is_final == 0)
            {
                n->is_final = 1;
                n->pattern = (char*) malloc(len+1);
                memcpy(n->pattern, data.pattern, len);
                n->pattern[len] = '\0';
				n->id = data.id;
            }
            else
            {
				if(opt == DUPKEY_ABORT)
				{
                	printf("Duplicate keyword \"%s\"\n", n->pattern);
                	return -1;
				}
            }
        }
    }
    return 0;
}

fuzzy_struct* fuzzy_new(void)
{
    fuzzy_struct* fuzs = (fuzzy_struct*) malloc(sizeof(fuzzy_struct));
    if(!fuzs)
        return NULL;
    fuzs->root = (trie_node*) malloc(sizeof(trie_node));
    if(!fuzs->root)
    {
        free(fuzs);
        return NULL;
    }
    fuzs->root->next_list = NULL;
    fuzs->root->next_one = NULL;
    fuzs->root->key = 0;
    fuzs->root->is_final = 0;
   
    fuzs->fuzzy_map = (char**) malloc (sizeof(char*) * 26);
    memset(fuzs->fuzzy_map, 0, sizeof(char*) * 26);
    return fuzs;
}

int fuzzy_load_search_key(word_data data, fuzzy_struct *fuzs, int opt)
{
    int ret = 0;
    if(!fuzs)
        return -1;
    ret = trie_build(data,fuzs,opt);
    return ret;
}

void fuzzy_free(fuzzy_struct *fuzs)
{
    int i;

    trie_node_free_children(fuzs->root);
    free(fuzs->root);
    for(i = 0; i < 26; i++)
    {
        free(fuzs->fuzzy_map[i]);
    }
    free(fuzs->fuzzy_map);
    free(fuzs);
}

void _fuzzy_complete(trie_node* n)
{
    trie_node *child = NULL;
    word_data data;

    if(trie_node_is_final(n))
    {
        data = trie_node_get_data(n);
        printf("complete: %s\n", data.pattern);
        return;
    }

    child = trie_node_get_children_head(n);

    if(child)
    {
        _fuzzy_complete(child);
    }
    else
    {
        printf("%s error?\n", __FUNCTION__);
    }
}

int _fuzzy_try_search(fuzzy_struct *fuzs, trie_node* n, char *str, int quo_del, int quo_add)
{
    int i;
    trie_node *child = NULL;
    word_data data;

    if(str[0] == '\0')
    {
        if(trie_node_is_final(n))
        {
		    data = trie_node_get_data(n);
            printf("exact: %s\n", data.pattern);
            return FUZZYI_HIT;
        }
        else
        {
            _fuzzy_complete(n);
            return FUZZYI_HIT;
        }
    }
    else if(str[0] < 'a' || str[0] > 'z')
        return FUZZYI_MISS;

    // exact match
    child = trie_node_lookup_children(n, str[0]);
    if(child && _fuzzy_try_search(fuzs, child, str + 1, quo_del, quo_add) == FUZZYI_HIT)
        return FUZZYI_HIT;

    // keyboard vicinity
    for(i = 1; i < fuzs->fuzzy_map[str[0]-'a'][0] + 1; i++)
    {
        child = trie_node_lookup_children(n, (char)(fuzs->fuzzy_map[str[0]-'a'][i] + 'a'));

        if(child && _fuzzy_try_search(fuzs, child, str + 1, quo_del, quo_add) == FUZZYI_HIT)
            return FUZZYI_HIT;
    }

    // deletion case
    if(quo_del > 0)
    {
        child = trie_node_get_children_head(n);
        while(child)
        {
            if(_fuzzy_try_search(fuzs, child, str + 1, quo_del-1, quo_add) == FUZZYI_HIT)
                return FUZZYI_HIT;
            child = trie_node_get_next_sibling(child);
        }
    }

    // addition case
    if(quo_add > 0 && str[1] != '\0')
    {
        child = trie_node_lookup_children(n, str[1]);
        if(child && _fuzzy_try_search(fuzs, child, str + 2, quo_del, quo_add - 1) == FUZZYI_HIT)
            return FUZZYI_HIT;
    }

    return FUZZYI_MISS; 
}

int fuzzy_search(fuzzy_struct *fuzs, char *result, char *query)
{
    char *p = query;
    int quo_del = 0;
    int quo_add = 0;

    for ( ; *p != '\0'; ++p) *p = tolower(*p);

    if(!fuzs)
        return -1;

    if(strlen(query) > 5)
    {
        quo_del = 1;
        quo_add = 1;
    }

    return _fuzzy_try_search(fuzs, fuzs->root, query, quo_del, quo_add);
}

void fuzzy_load_conf(fuzzy_struct *fuzs, FILE *fconf)
{
    char buf[BUFF_SIZE] = {0};    
    int i;
    
    if(!fuzs || !fconf || !fuzs->fuzzy_map)
        return;

    while(fscanf(fconf, "%s", buf) != EOF)
    {
        fuzs->fuzzy_map[(buf[0]-'a')] = (char*) malloc (sizeof(char) * strlen(buf));
        memset(fuzs->fuzzy_map[(buf[0]-'a')], 0, sizeof(char) * strlen(buf));

        fuzs->fuzzy_map[(buf[0]-'a')][0] = strlen(buf) - 1;
        for(i = 1; i < strlen(buf); i++)
        {
            fuzs->fuzzy_map[(buf[0]-'a')][i] = buf[i] - 'a';
        }
    }
}
