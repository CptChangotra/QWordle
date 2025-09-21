#include "wordle.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORDS 10000

Trie *generateDict(char *filename, int k, char *selected1, char *selected2) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Failed to open dictionary file");
        return NULL;
    }

    char **words = malloc(sizeof(char *) * MAX_WORDS);
    if (words == NULL) {
        perror("Memory allocation failed for words");
        fclose(fp);
        return NULL;
    }

    int count = 0;
    char buffer[k + 2]; // +1 for \0 string end and +1 for \n.

    while (fgets(buffer, sizeof(buffer), fp) && count < MAX_WORDS) {
        size_t len = strcspn(buffer, "\n");
        buffer[len] = '\0';

        if (strlen(buffer) != (size_t)k)
            continue;

        words[count] = strdup(buffer);

        if (words[count] == NULL) {
            perror("strdup failed");
            for (int j = 0; j < count; j++) free(words[j]);
            free(words);
            fclose(fp);
            return NULL;
        }
        count++;
    }

    fclose(fp);

    if (count == 0) {
        fprintf(stderr, "Empty dictionary or no valid words of length %d\n", k);
        free(words);
        return NULL;
    }

    int index = (int)(drand48() * count);
    strcpy(selected1, words[index]);

    if (selected2 != NULL) {
        int index2;
        if (count > 1) {
            do {
                index2 = (int)(drand48() * count);
            } while (index2 == index);
            strcpy(selected2, words[index2]);
        } else {
            strcpy(selected2, words[index]);
        }
    }

    Trie *trie = create();
    for (int i = 0; i < count; i++) {
        insert(trie, words[i]);        
        free(words[i]);                
    }

    free(words);

    return trie;
}

char *guess(Trie *dict, int k) {
    char *buffer = malloc(k + 2);

    printf("Please input your guess: ");
    while (fgets(buffer, k + 2, stdin)) {
        size_t len = strlen(buffer);
        if (buffer[len - 1] == '\n') buffer[len - 1] = '\0';

        for (int i = 0; i < k; i++) {
            buffer[i] = tolower(buffer[i]);
        }

        if (!dict || lookup(dict, buffer)) {
            return buffer;
        }

        printf("Invalid word. Try again: ");
    }

    fprintf(stderr, "Input error.\n");
    free(buffer);
    exit(EXIT_FAILURE);
}

feedback_result *getFeedback(char *guess, char *word1, char *word2, int k) {
    feedback_result *feedback = malloc(sizeof(feedback_result) * k);
    int used1[k], used2[k];
    memset(used1, 0, sizeof(used1));
    memset(used2, 0, sizeof(used2));

    // First pass: exact matches
    for (int i = 0; i < k; ++i) {
        feedback[i] = WRONG;
        if (guess[i] == word1[i]) {
            feedback[i] = CORRECT;
            used1[i] = 1;
        } else if (word2 && guess[i] == word2[i]) {
            feedback[i] = QUANTUMCORRECT;
            used2[i] = 1;
        }
    }

    // Second pass: wrong position matches
    for (int i = 0; i < k; ++i) {
        if (feedback[i] == CORRECT || feedback[i] == QUANTUMCORRECT) continue;

        // Check word1 first
        bool found = false;
        for (int j = 0; j < k; ++j) {
            if (!used1[j] && guess[i] == word1[j]) {
                feedback[i] = WRONGPOS;
                used1[j] = 1;
                found = true;
                break;
            }
        }
        
        // Only check word2 if not found in word1
        if (!found && word2) {
            for (int j = 0; j < k; ++j) {
                if (!used2[j] && guess[i] == word2[j]) {
                    feedback[i] = QUANTUMWRONGPOS;
                    used2[j] = 1;
                    break;
                }
            }
        }
    }

    return feedback;
}

void printFeedback(feedback_result *feedback, int k) {
    printf("Result: ");
    for (int i = 0; i < k; ++i) {
        switch (feedback[i]) {
            case CORRECT: printf(CORRECT_UNICODE); break;
            case WRONGPOS: printf(PRESENT_UNICODE); break;
            case WRONG: printf(WRONG_UNICODE); break;
            case QUANTUMCORRECT: printf(QCORRECT_UNICODE); break;
            case QUANTUMWRONGPOS: printf(QPRESENT_UNICODE); break;
        }
    }
    printf("\n");
}

bool checkWin(feedback_result *feedback, int k) {
    for (int i = 0; i < k; ++i) {
        if (feedback[i] != CORRECT && feedback[i] != QUANTUMCORRECT)
            return false;
    }
    return true;
}