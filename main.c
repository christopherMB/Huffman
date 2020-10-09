#include <stdlib.h>
#include <stdio.h>
#include "huffman.h"
#include <time.h>
#include <string.h>
#define OPTI 0 //Part 2 : 0, Part 3 : 1

void pretty_print(int n){
    int million = 0;
    int thousand = 0;
    if (n >= 1000000){
        million++;
        printf("%d ", n/1000000);
        n = n - (n/1000000 * 1000000);
    }
    if (n >= 1000){
        thousand++;
        if(million && n/1000 < 100) printf("0");
        if(million && n/1000 < 10) printf("0");
        printf("%d ", n/1000);
        n = n - (n/1000 * 1000);
    } 
    if(thousand && n < 100) printf("0");
    if(thousand && n < 10) printf("0");
    printf("%d\n", n);
}


int main(int argc, char *argv[]){

    const char* input_file = "files/input.txt";
    const char* output_file = "files/binary.txt";
    const char* output_huffman_file = "files/huffman.txt";
    const char* dico_file = "files/dico.txt";
    const char* decompressed_file = "files/decompressed.txt";
    clock_t begin, end;
        text_to_binary(input_file, output_file);
        printf("Input count : ");
        pretty_print(count_letters(input_file));
        printf("Standard Output count : ");
        pretty_print(count_letters(output_file));
    if (!OPTI && argc == 1){
        begin = clock();
        huffman_compression(input_file, output_huffman_file);
        end = clock();
        printf("Huffman Output count : "); 
        pretty_print(count_letters(output_huffman_file));
        
        //Decoding part
        Element* occ = get_occurrences(input_file);
        Node* tree = get_huffman_tree(occ);
        huffman_decode(output_huffman_file, decompressed_file, tree);
        free_tree(tree);
    }
    else if((argc != 0 && strcmp(argv[1], "opti") == 0) || OPTI){
        begin = clock();
        huffman_compression_opti(input_file, output_huffman_file);
        end = clock();
        printf("Huffman Output count : ");
        pretty_print(count_letters(output_huffman_file));
        
        //Decoding part
        huffman_decompression(output_huffman_file, dico_file,  decompressed_file);
    }
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    double ratio = 100 * (double)count_letters(output_huffman_file)/(double)count_letters(output_file);
    printf("Compression rate : %.2f%% In (only) %.3f seconds\n", ratio, time_spent);
    
  



}
