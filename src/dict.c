#include "dict.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct Trie{
    Trie *alphabets[26];     // 0 is for a and 26 is for Z.
    bool end;
};

Trie *create(){
    Trie *newTree = malloc(sizeof(Trie));
    for (int i = 0; i < 26; i++){
        newTree->alphabets[i] = NULL;
    }
    newTree->end = false;
    return newTree;
}

void insert(Trie *dict, char *str){
    if (dict == NULL || str == NULL){
        return;
    }
    Trie *flow = dict;
    int stringLength = strlen(str);
    for (int i = 0; i < stringLength; i++){
        if (str[i] < 'a' || str[i] > 'z') {
            continue;  // Skip invalid characters, or return early
        }
        int index = str[i] - 'a';
        if (flow->alphabets[index] == NULL){
            Trie *downLayer = create();             // Creates a new layer.
            flow->alphabets[index] = downLayer;     // The newly created downLayer is connected to the upper layer root at the right alphabetical index.
            flow = flow->alphabets[index];          // The flow of the program now points to the new downLayer.
            continue;
        }
        flow = flow->alphabets[index];              // If words are pre-existing, just go down the road.
    }
    flow->end = true;                               // When done with the entire word, mark it as the end of some word.
}

bool lookup(Trie *dict, char *str){
    Trie *flow = dict;
    int stringLength = strlen(str);
    for (int i = 0; i < stringLength; i++){
        if (str[i] < 'a' || str[i] > 'z') {
            continue;  // Skip invalid characters, or return early
        }
        int index = str[i] - 'a';
        if (flow->alphabets[index] == NULL){        // If there is no link, it means the words haven't been inserted, exit and return false.
            return 0;
        }
        flow = flow->alphabets[index];              // If the words have been inserted at the right index (thanks to insert()), just go down the road.
    }
    return flow->end;                        // To see if the entire word has been inserted, not some smaller version of it. Matter was inserted and looked up, not mat.
}

void destroy(Trie *dict){
    if (dict == NULL){
        return;
    }
    for (int i = 0; i < 26; i++){
        if (dict -> alphabets[i] != NULL){
            destroy(dict->alphabets[i]);
        }
    }
    free(dict);
}