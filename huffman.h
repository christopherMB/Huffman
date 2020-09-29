#ifndef _HUFFMAN_H
#define _HUFFMAN_H
#define MAX_CODE 50
#include <stdlib.h>

///A
//Converts a character to a byte.
void char_to_binary(char c, char byte[8]);
//Converts a text to bytes (basic ASCII table)
void text_to_binary(const char* input, const char* output);

///B
//Counts the number of char in a file
int count_letters(const char* input);

///C
//List structure (we use a Node as data, cf question D)
typedef struct Element{
    struct Node* node;
    struct Element* next;
}Element;
//Creates an Element containing a given char
Element* create_element(char c);
//Adds an occurence to a list of char
void add_occurrence(char c, Element** l);
//Returns a list containing all char occurences in a given text file
Element* get_occurrences(const char* input);

///D
typedef struct Node{
    char letter;
    int occurrences;
    struct Node* right;
    struct Node* left;
}Node;
//Removes and resturns the Element with the smallest occurrences
Element* get_min_occurrence(Element** l);
//Returns a Huffman tree from an occurrences list.
Node* get_huffman_tree(Element* l);

///E
//Stores a given node with the associated code.
void store_node(Node* node, char code[MAX_CODE], const char* output);
//Stores the Huffman dictionary from a Huffman tree in a file.
//each left step is a 0, each right step is a 1
void store_dictionary_from_tree(Node* t, const char* output);

/// F
// Gets the code for a letter from the dictionary
void get_code_from_dico(char c, const char* dico, char code[MAX_CODE]);
// Converts a text to 0 and 1 using the Huffman dictionnary
void huffman_code(const char* input, const char* dico, const char* output);

/// G
// Wraps up the whole thing
void huffman_compression(const char* input, const char* output);

///H
// Moves a pointer in a tree to reach the letter read as binary
int move_in_tree(Node** t, char code);
// Decodes a huffman code, but needs the tree to do so
void huffman_decode(const char* input, const char* output, Node* t);

/// I
// Tries to increment a pre-existing char
int add_existing_occurrence_dico(char c, Node** array, size_t n);
// Adds an occurence to an array of char
void add_occurrence_opti(char c, Node*** array, size_t* n);
// Returns a list containing all char occurences in a given text file
void get_occurrences_opti(const char* input, Node*** array, size_t* n);

/// J
// Functions for heap sort
void swap(Node**a, Node**b);
void heapify(Node** arr, int n, int i);
void heapSort(Node** arr, int n);

/// K
// Queue struct
typedef struct ElementQ{
    Node* data;
    struct ElementQ* next;
    struct ElementQ* prev;
}ElementQ;

typedef struct Queue{
    ElementQ* queue_first;
    ElementQ* queue_last;
} Queue;

Queue* create_queue();
int is_empty(Queue* q);
void enqueue(Queue* q, Node* val);
Node* dequeue(Queue* q);
// Gets the minimum node from the 2 queues
Node* get_minfrom_queues(Queue* q1, Queue* q2);

// Get tree from sorted array
Node* get_huffman_tree_opti(Node** array, size_t n);

/// L
// AVL
typedef struct DicoNode{
    char letter;
    char code[MAX_CODE];
    struct DicoNode* right;
    struct DicoNode* left;
}DicoNode;

void add_Node_BST(DicoNode** tree, DicoNode* added);
int BT_depth(DicoNode* tree);
int bf(DicoNode* tree);
void right_rot(DicoNode** tree);
void left_rot(DicoNode** tree);
void balance(DicoNode** tree);
void add_AVL(DicoNode** tree, DicoNode* added);

// Adds a Huffman node to a dico AVL
void store_node_AVL(Node* node, char code[MAX_CODE], DicoNode** AVL);
//Gets a dico as AVL
DicoNode* get_dictionary_AVL_from_tree(Node* t);
//Gets the code to a letter in an AVL dico
void get_code_from_AVL_dico(char c, DicoNode* dico, char code[MAX_CODE]);
// Converts a text to 0 and 1 using the AVL dictionnary
void huffman_code_opti(const char* input, DicoNode* dico_AVL, const char* output);

/// M
// Wraps up the whole thing
void huffman_compression_opti(const char* input, const char* output);

/// N
// Adds a letter to a Huffman tree using its code
void add_letter_to_huffman_tree(char c, char code[], Node** tree);
// Creates a Huffman tree from a dico file
Node* get_huffman_tree_from_dico(const char* dico);
// Decompresses a file from a dico file
void huffman_decompression(const char* input, const char* dico, const char* output);

/// Tools

void free_tree(Node* tree);
void free_dico_tree(DicoNode* tree);
void free_list(Element* l);

#endif