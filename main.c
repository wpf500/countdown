#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DICT_FILE "en_GB.dic"
#define MAX_WORD_LEN 128

#define removelb(word) \
    do { \
        char *lb = strchr(word, '\n'); \
        if (lb) *lb = '\0'; \
    } while(0);

#define toindex(letter) ((letter) - 'a')

typedef struct node_s {
    struct node_s *child[26];
    int used;
} node_t;

node_t top;

/*
 * Add word to tree
 */
void node_add(node_t *node, char *word)
{
    while (*word) {
        int i = toindex(*word++);
        if (!node->child[i]) {
            node->child[i] = calloc(sizeof(node_t), 1);
        }
        node = node->child[i];
    }
    node->used = 1;
}

/*
 * Find a word in tree
 * @return 1 if found, 0 otherwise
 */
int node_find(node_t *node, char *word)
{
    while (*word) {
        int i = toindex(*word++);
        if (!node->child[i]) {
            return 0;
        }
        node = node->child[i];
    }
    return node->used;
}

/*
 * Looks for combinations of the letters in word in the tree
 * @return the size of the largest word found
 */
int node_traverse(node_t *node, char *word, int depth)
{
    int i, maxlen = 0;

    // found a word
    if (node->used) {
        maxlen = depth;
        printf("%.*s\n", depth, word);
    }

    for (i = depth; word[i]; i++) {
        int j = toindex(word[i]);
        if (node->child[j]) {
            int len;
            // move char to front of word
            char tmp = word[depth];
            word[depth] = word[i]; word[i] = tmp;

            len = node_traverse(node->child[j], word, depth + 1);
            if (len > maxlen) {
                maxlen = len;
            }

            // move char back
            word[i] = word[depth]; word[depth] = tmp;
        }
    }

    return maxlen;
}

/**** LOAD DICTIONARY ****/
/*
 * Only allow words where all the characters are a-z
 * with a length in the range 5-9. Convert all letters
 * to lowercase
 * @return 1 if word is valid, 0 otherwise
 */
int isvalid(char *word)
{
    int len = 0;
    while (*word && isalpha(*word)) {
        *word++ = tolower(*word);
        len++;
    }
    return !*word && len > 4 && len <= 9;
}

/*
 * Load the dictionary. Expects each line to contain one word
 */
void load(void)
{
    FILE *f = fopen(DICT_FILE, "r");
    char word[MAX_WORD_LEN];
    int count = 0, total = 0;
    while (fgets(word, sizeof(word), f)) {
        removelb(word);
        if (isvalid(word)) {
            node_add(&top, word);
            count++;
        }
        total++;
    }
    fclose(f);
    printf("loaded %d/%d words\n", count, total);
}

/*
 * Main
 */
int main(int argc, char *argv[])
{
    char word[MAX_WORD_LEN];

    memset(&top, sizeof(node_t), 0);
    load();

    while (fgets(word, sizeof(word), stdin)) {
        int len;
        removelb(word);
        len = node_traverse(&top, word, 0);
        if (!len) {
            printf("not found\n");
        } else {
            printf("found word with %d characters\n", len);
        }
    }

    return 0;
}

