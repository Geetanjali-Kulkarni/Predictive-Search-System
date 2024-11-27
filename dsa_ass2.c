#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define ALPHABET_SIZE 26
#define MAX_WORD_LENGTH 50
#define MAX_SUGGESTIONS 30

// Trie node structure
struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    bool is_end_of_word;
};

// Function to create a new Trie node
struct TrieNode *create_node() {
    struct TrieNode *node = (struct TrieNode *)malloc(sizeof(struct TrieNode));
    if (node) {
        node->is_end_of_word = false;
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            node->children[i] = NULL;
        }
    }
    return node;
}

// Function to insert a word into the Trie
void insert(struct TrieNode *root, const char *word) {
    struct TrieNode *node = root;
    while (*word) {
        if (isalpha(*word)) { // Check if the character is alphabetic
            int index = tolower(*word) - 'a';
            if (index < 0 || index >= ALPHABET_SIZE) {
                printf("Invalid character in word: %c\n", *word);
                return;
            }
            if (!node->children[index]) {
                node->children[index] = create_node();
            }
            node = node->children[index];
        }
        word++;
    }
    node->is_end_of_word = true;
}

// Function to load words from a file
void load_words_from_file(struct TrieNode *root, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char word[MAX_WORD_LENGTH];
    while (fgets(word, sizeof(word), file)) {
        // Remove trailing newline character
        word[strcspn(word, "\n")] = '\0';
        insert(root, word);
    }

    fclose(file);
}

// Function to find all words with the given prefix
void find_all_words(struct TrieNode *node, char *prefix, int depth, char words[][MAX_WORD_LENGTH], int *count) {
    if (*count >= MAX_SUGGESTIONS) return;

    if (node->is_end_of_word) {
        strcpy(words[*count], prefix);
        (*count)++;
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            prefix[depth] = 'a' + i;
            prefix[depth + 1] = '\0';
            find_all_words(node->children[i], prefix, depth + 1, words, count);
        }
    }
    prefix[depth] = '\0'; // Reset prefix to avoid overwriting
}

// Function to get autocomplete suggestions
void autocomplete(struct TrieNode *root, const char *prefix, char words[][MAX_WORD_LENGTH], int *count) {
    struct TrieNode *node = root;
    char new_prefix[MAX_WORD_LENGTH];
    strcpy(new_prefix, prefix);

    while (*prefix) {
        int index = tolower(*prefix) - 'a';
        if (index < 0 || index >= ALPHABET_SIZE || !node->children[index]) {
            return; // No words with this prefix
        }
        node = node->children[index];
        prefix++;
    }

    find_all_words(node, new_prefix, strlen(new_prefix), words, count);
}

// Main function to demonstrate usage
int main() {
    struct TrieNode *root = create_node();

    // Load words from a file
    const char *filename = "words.txt";
    load_words_from_file(root, filename);

    char prefix[MAX_WORD_LENGTH];
    char suggestions[MAX_SUGGESTIONS][MAX_WORD_LENGTH];
    int count = 0;

    printf("Enter search prefix: ");
    scanf("%s", prefix);

    autocomplete(root, prefix, suggestions, &count);

    printf("Autocomplete suggestions for '%s':\n", prefix);
    for (int i = 0; i < count; i++) {
        printf("%s\n", suggestions[i]);
    }

    // Free memory (optional but recommended)
    // Add a cleanup function to free allocated Trie nodes.

    return 0;
}
