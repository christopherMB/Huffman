#include "huffman.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

///A
void char_to_binary(char c, char byte[8]){
    for (int i = 0; i < 8; i++){
        byte[i] = 0;
        if ( c >= pow(2,7-i)){
            byte[i] = 1;
            c -= pow(2,7-i);
        }    
    }
}

void text_to_binary(const char* input, const char* output){
    FILE* input_file;
    FILE* output_file;
    input_file = fopen(input, "r");
    output_file = fopen(output, "w");
    
    char ch;
    char byte[8];
    while ((ch = fgetc(input_file)) != EOF){
        char_to_binary(ch, byte);
        for (int i = 0; i < 8; i ++){
            fprintf(output_file, "%d", byte[i]);
        }
    }

    fclose(input_file);
    fclose(output_file);
}

///B
int count_letters(const char* input){
    FILE* input_file;
    input_file = fopen(input, "r");
    int count = 0;

    char ch;
    while ((ch = fgetc(input_file)) != EOF) count++;
    fclose(input_file);

    return count;
}

///C
Element* create_element(char c){
    Element* new_el = (Element*)malloc(sizeof(Element));
    new_el->node = (Node*)malloc(sizeof(Node));
    new_el->node->left = NULL;
    new_el->node->right = NULL;
    new_el->node->letter = c;
    new_el->node->occurrences = 1;
    new_el->next = NULL;
    return new_el;
}

void add_occurrence(char c, Element** l){
    if (*l == NULL) *l = create_element(c);
    else if ((*l)->node->letter == c) (*l)->node->occurrences++;
    else add_occurrence(c, &((*l)->next));
}

Element* get_occurrences(const char* input){
    FILE* input_file;
    input_file = fopen(input, "r");
    Element* list_of_occurrences = NULL;
    char ch;
    while ((ch = fgetc(input_file)) != EOF) add_occurrence(ch, &list_of_occurrences);
    fclose(input_file);

    return list_of_occurrences;
}

///D
Element* get_min_occurrence(Element** l){
    if (*l == NULL) return NULL;
    else if ((*l)->next == NULL){
        Element* res = *l;
        *l = NULL;
        return res;    
    } 
    else{
        Element ** res = l;
        Element ** temp = &((*l)->next);
        while (*temp != NULL){
            if ((*temp)->node->occurrences < (*res)->node->occurrences){
                res = temp;
            }
            temp = &((*temp)->next);
        }
        Element* min_occ = *res;
        *res = (*res)->next;
        min_occ->next = NULL;
        return min_occ;
    }
}

Node* get_huffman_tree(Element* l){
    if (l == NULL) return NULL;
    while (l->next != NULL){
        Element* left = get_min_occurrence(&l);
        Element* right = get_min_occurrence(&l);
        Node* new_node = (Node*)malloc(sizeof(Node));

         // The new node has occurrences of the sum of each old nodes
        new_node->occurrences = left->node->occurrences + right->node->occurrences;
        new_node->left = left->node;
        new_node->right = right->node;

        //We re-use the left as a new Element;
        free(right);
        left->node = new_node;
        left->next = l;
        l = left;
    }
    Node* res = l->node;
    free(l);
    return res;
}

///E
void store_node(Node* node, char code[MAX_CODE], const char* output){
    
    if (node != NULL){
        //If it is a leef, that means we have reached a letter, and we must store it.
        if (node->left == NULL && node->right == NULL){
            FILE* output_file;
            output_file = fopen(output, "a");
            fprintf(output_file, "%c : %s\n", node->letter, code);
            fclose(output_file);
        }
        else {
            // we modify the code, as we go down in the tree.
            char new_code[MAX_CODE];
            strcpy(new_code, code);
            strcat(new_code, "0");//0 for a right step
            store_node(node->left, new_code, output);
            strcpy(new_code, code);
            strcat(new_code, "1");//1 for a right step
            store_node(node->right, new_code, output); 
        }      
    }
}

void store_dictionary_from_tree(Node* t, const char* output){
    FILE* output_file;
    output_file = fopen(output, "w");//clear any previous dico
    fclose(output_file);
    char code[MAX_CODE] = "";
    store_node(t, code, output);
}

/// F
void get_code_from_dico(char c, const char* dico, char code[MAX_CODE]){
    FILE* dico_file;
    dico_file = fopen(dico, "r");
    int found = 0;
    char line[MAX_CODE*2];
    while (fgets(line, MAX_CODE*2, dico_file) && !found)
    {
        if (line[0] == c && line[1]!= ':'){//second condition to deal with the space being first in line after a line break
            found = 1;
            if (line[0] == '\n'){
                fgets(line, MAX_CODE*2, dico_file);
                strcpy(code, line + 3);
                *strchr(code, '\n') = '\0';
            }
            else {
                strcpy(code, line + 4);
                *strchr(code, '\n') = '\0';
            }
        }  
    }
    fclose(dico_file);
}

void huffman_code(const char* input, const char* dico, const char* output){
    FILE* input_file;
    FILE* output_file;
    input_file = fopen(input, "r");
    output_file = fopen(output, "w");
    char code[MAX_CODE];
    char ch;
    while ((ch = fgetc(input_file)) != EOF){
        get_code_from_dico(ch, dico, code);
        fprintf(output_file, "%s", code);
    }

    fclose(input_file);
    fclose(output_file);
}

/// G
void huffman_compression(const char* input, const char* output){
    const char* dico_file = "files/dico.txt";

    Element* occ = get_occurrences(input);
    Node* tree = get_huffman_tree(occ);
    store_dictionary_from_tree(tree, dico_file);
    huffman_code(input, dico_file, output);
    free_tree(tree);
}

///H
int move_in_tree(Node** t, char code){
    if (code == '0') *t = (*t)->left;
    else *t = (*t)->right;
    if ((*t)->left == NULL && (*t)->right == NULL) return 1;
    return 0;
}

void huffman_decode(const char* input, const char* output, Node* t){
    FILE* input_file;
    FILE* output_file;
    input_file = fopen(input, "r");
    output_file = fopen(output, "w");
    char ch;
    Node* temp = t;
    while ((ch = fgetc(input_file)) != EOF){
        if (move_in_tree(&temp, ch) == 1){
            fprintf(output_file, "%c", temp->letter);
            temp = t;
        }     
    }

    fclose(input_file);
    fclose(output_file);
}

/// I
int add_existing_occurrence_dico(char c, Node** array, size_t n){
    if (n <= 0) return 0;
    else{
        int first = 0;
        int last = n - 1;
        int middle = (first+last)/2;

        while (first <= last) {
            if (array[middle]->letter < c)
                first = middle + 1;
            else if (array[middle]->letter == c) {
                (array[middle]->occurrences)++;
                return 1;
            }
            else{
                last = middle - 1;
            }
            middle = (first + last)/2;
        }
        return 0;
    }
}

void add_occurrence_opti(char c, Node*** array, size_t* n){
    if (add_existing_occurrence_dico(c, *array, *n) == 0){// We first try to add it to an existing occurrence
        Node ** new_array = (Node**)malloc(sizeof(Node*) * (*n + 1));
        int add_index = -1;
        int i;
        for (i = 0; i < *n; i ++){
            if (add_index == -1 && c < (*array)[i]->letter){
                add_index = i;
                new_array[i + 1] = (*array)[i];
            }
            else {
                if (add_index == -1) new_array[i] = (*array)[i];
                else new_array[i + 1] = (*array)[i];
            } 
        }
        if (add_index == -1) add_index = i;
        
        if (*array) free(*array);
        else add_index = 0;
        Node* new_node = (Node*)malloc(sizeof(Node));
        new_node->left = NULL;
        new_node->right = NULL;
        new_node->letter = c;
        new_node->occurrences = 1;
        new_array[add_index] = new_node;
        *array = new_array;
        *n = *n + 1;
    }
    
}

void get_occurrences_opti(const char* input, Node*** array, size_t* n){
    FILE* input_file;
    input_file = fopen(input, "r");
    char ch;
    while ((ch = fgetc(input_file)) != EOF) add_occurrence_opti(ch, array, n);
    fclose(input_file);
}

/// J
void swap(Node**a, Node**b) {
    Node* temp = *a;
    *a = *b;
    *b = temp;
}

void heapify(Node** arr, int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    if (left < n && arr[left]->occurrences > arr[largest]->occurrences) largest = left;
    if (right < n && arr[right]->occurrences > arr[largest]->occurrences) largest = right;
    if (largest != i) {
        swap(&arr[i], &arr[largest]);
        heapify(arr, n, largest);
    }
}

void heapSort(Node** arr, int n) {
    for (int i = n / 2 - 1; i >= 0; i--) heapify(arr, n, i);
    for (int i = n - 1; i >= 0; i--) {
        swap(&arr[0], &arr[i]);
        heapify(arr, i, 0);
    }
}

/// K
Queue* create_queue(){
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->queue_first = NULL;
    q->queue_last = NULL;
    return q;
}

int is_empty(Queue* q){
    if (q->queue_first == NULL) return 1;
    else return 0;
}

void enqueue(Queue* q, Node* val){
    ElementQ* new_el = (ElementQ*)malloc(sizeof(ElementQ));
    if (q->queue_first == NULL) q->queue_last = new_el;
    else q->queue_first->prev = new_el;
    new_el->data = val;
    new_el->next = q->queue_first;
    q->queue_first = new_el;
}

Node* dequeue(Queue* q){
    if (!is_empty(q)){
        ElementQ* old = q->queue_last;
        if (q->queue_last == q->queue_first) q->queue_first = NULL;
        q->queue_last = q->queue_last->prev;
        if (q->queue_last) q->queue_last->next = NULL;
        Node* n = old->data;
        free(old);
        return n;
    }
    else {
        return NULL;
    }
}


Node* get_minfrom_queues(Queue* q1, Queue* q2){
    if (is_empty(q1)) return dequeue(q2);
    else if (is_empty(q2)) return dequeue(q1);
    else{
        if (q1->queue_last->data->occurrences < q2->queue_last->data->occurrences) return dequeue(q1); //we peek the 2 first in queue
        else return dequeue(q2);
    }
}


Node* get_huffman_tree_opti(Node** array, size_t n){
    Queue* q1, *q2;
    q1 = create_queue();
    q2 = create_queue();
    int i = 0;// Add all nodes to the first queue
    while (i < n){
        enqueue(q1, array[i]);
        i++;
    }

    while (!is_empty(q1) || !is_empty(q2)){
        Node* left = get_minfrom_queues(q1, q2);
        Node* right = get_minfrom_queues(q1, q2);
        if (right == NULL) {
            free(q1);
            free(q2);
            return left; // One node left => it's the tree.
        }
        else{
            // The new node has occurrences of the sum of each old nodes
            Node* new_node = (Node*)malloc(sizeof(Node));
            new_node->occurrences = left->occurrences + right->occurrences;
            new_node->left = left;
            new_node->right = right;
            enqueue(q2, new_node);
        }
    }
    free(q1);
    free(q2);
    return NULL;
}

/// L
void add_Node_BST(DicoNode** tree, DicoNode* added)
{
    if ((*tree) == NULL)
    {
        *tree = added;
    }
    else if ((*tree)->letter < added->letter)
    {
        add_Node_BST(&((*tree)->right), added);
    }
    else if ((*tree)->letter > added->letter)
    {
        add_Node_BST(&((*tree)->left), added);
    }
}

int BT_depth(DicoNode* tree)
{
    int p1;
    int p2;
    if (tree == NULL) return 0;
    else
    {
        p1 = BT_depth(tree->right);
        p2 = BT_depth(tree->left);
        if (p1 > p2) return(p1 + 1);
        else return(p2 + 1);
    }
}

int bf(DicoNode* tree) {
    if (tree == NULL) return 0;
    else {
        return BT_depth(tree->right) - BT_depth(tree->left);
    }
}
void right_rot(DicoNode** tree)
{
    if (*tree !=  NULL)
    {
        DicoNode* temp = (*tree)->left;
        (*tree)->left = temp->right;
        temp->right = (*tree);
        *tree = temp;
    }
}


void left_rot(DicoNode** tree)
{
    if (*tree !=NULL)
    {
        DicoNode* temp = (*tree)->right;
        (*tree)->right = temp->left;
        temp->left = (*tree);
        *tree = temp;
    }
}

void balance(DicoNode** tree)
{
    if (*tree != NULL)
    {
        balance(&((*tree)->right));
        balance(&((*tree)->right));

        int fact = bf((*tree));
        if (fact <= -2)
        {
            if (bf((*tree)->left) > 0)
            {
                left_rot(&(*tree)->left);
            }
            right_rot(tree);
        }
        else if (fact >= 2)
        {
            if (bf((*tree)->right) < 0)
            {
                right_rot(&(*tree)->right);
            }
            left_rot(tree);
        }
    }
}

void add_AVL(DicoNode** tree, DicoNode* added){
    add_Node_BST(tree, added);
    balance(tree);
}

void store_node_AVL(Node* node, char code[MAX_CODE], DicoNode** AVL){
    
    if (node != NULL){
        //If it is a leef, that means we have reached a letter, and we must store it.
        if( node->left == NULL && node->right == NULL){
            DicoNode* new_dnode = (DicoNode*)malloc(sizeof(DicoNode));
            new_dnode->letter = node->letter;
            strcpy(new_dnode->code, code);
            new_dnode->left = NULL;
            new_dnode->right = NULL;
            add_AVL(AVL, new_dnode);
        }
        else {
            // we modify the code, as we go down in the tree.
            char new_code[MAX_CODE];
            strcpy(new_code, code);
            strcat(new_code, "0");//0 for a right step
            store_node_AVL(node->left, new_code, AVL);
            strcpy(new_code, code);
            strcat(new_code, "1");//1 for a right step
            store_node_AVL(node->right, new_code, AVL); 
        }      
    }
}

DicoNode* get_dictionary_AVL_from_tree(Node* t){
    DicoNode* AVL = NULL;
    char code[MAX_CODE] = "";
    store_node_AVL(t, code, &AVL);
    return AVL;    
}

void get_code_from_AVL_dico(char c, DicoNode* dico, char code[MAX_CODE]){
    if (c == dico->letter){ // We have reached the letter ( we know it's in the tree)
        strcpy(code, dico->code);
    }
    else if (c > dico->letter) get_code_from_AVL_dico(c, dico->right, code);
    else get_code_from_AVL_dico(c, dico->left, code);
}

void huffman_code_opti(const char* input, DicoNode* dico_AVL, const char* output){
    FILE* input_file;
    FILE* output_file;
    input_file = fopen(input, "r");
    output_file = fopen(output, "w");
    char code[MAX_CODE];
    char ch;
    while ((ch = fgetc(input_file)) != EOF){
        get_code_from_AVL_dico(ch, dico_AVL, code);
        fprintf(output_file, "%s", code);
    }

    fclose(input_file);
    fclose(output_file);
}

/// M
void huffman_compression_opti(const char* input, const char* output){
    Node** occ = NULL;
    size_t n = 0;
    get_occurrences_opti(input, &occ, &n);
    heapSort(occ, n);
    Node* tree = get_huffman_tree_opti(occ, n);
    //creating the dico file
    store_dictionary_from_tree(tree, "files/dico.txt");
    DicoNode* tree_dico = get_dictionary_AVL_from_tree(tree);
    huffman_code_opti(input, tree_dico, output);
    free(occ);
    free_dico_tree(tree_dico);
    free_tree(tree);
}

/// N
void add_letter_to_huffman_tree(char c, char code[], Node** tree){
    if (*tree == NULL){
        *tree = (Node*)malloc(sizeof(Node));
        (*tree)->left = NULL;
        (*tree)->right = NULL;
    }
    if (code[0] == '\0') (*tree)->letter = c;
    else{
        if (code[0] == '0') add_letter_to_huffman_tree(c, code + 1, &((*tree)->left));
        else add_letter_to_huffman_tree(c, code + 1, &((*tree)->right));
    }

}

Node* get_huffman_tree_from_dico(const char* dico){
    FILE* dico_file;
    dico_file = fopen(dico, "r");
    int found = 0;
    char line[MAX_CODE*2];
    char code[MAX_CODE];
    char c;
    Node* tree = NULL;
    while (fgets(line, MAX_CODE*2, dico_file) && !found)
    {   
        c = line[0];
        if (line[1]!= ':'){//second condition to deal with the space being first in line after a line break
            if (line[0] == '\n'){
                fgets(line, MAX_CODE*2, dico_file);
                strcpy(code, line + 3);
                *strchr(code, '\n') = '\0';
            }
            else {
                strcpy(code, line + 4);
                *strchr(code, '\n') = '\0';
            }
            add_letter_to_huffman_tree(c, code, &tree);
        }
    }
    fclose(dico_file);
    return tree;
}

void huffman_decompression(const char* input, const char* dico, const char* output){
    Node* tree = get_huffman_tree_from_dico(dico);
    huffman_decode(input, output, tree);
    free_tree(tree);
}

/// Tools
void free_tree(Node* tree){
    if(tree){
        free_tree(tree->left);
        free_tree(tree->right);
        free(tree);
    }
}

void free_dico_tree(DicoNode* tree){
    if(tree){
        free_dico_tree(tree->left);
        free_dico_tree(tree->right);
        free(tree);
    }
}

void free_list(Element* l){
    if(l){
        free_list(l->next);
        free(l);
    }
}