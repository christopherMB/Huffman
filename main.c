#include <stdlib.h>
#include <stdio.h>
#include "huffman.h"
#include <time.h>
#define OPTI 1 //Part 2 : 0, Part 3 : 1

void pretty_print(int n){
    if (n >= 1000000){
        printf("%d ", n/1000000);
        n = n - (n/1000000 * 1000000);
    }
    if (n >= 1000){
        printf("%d ", n/1000);
        n = n - (n/1000 * 1000);
    } 
    printf("%d\n", n);
}

int main(){

    const char* input_file = "files/input.txt";
    const char* output_file = "files/output.txt";
    const char* output_huffman_file = "files/output_huffman.txt";
    clock_t begin, end;
    
        
        text_to_binary(input_file, output_file);
        printf("Input count : ");
        pretty_print(count_letters(input_file));
        printf("Standard Output count : ");
        pretty_print(count_letters(output_file));
    if (!OPTI){
        begin = clock();
        huffman_compression(input_file, output_huffman_file);
        printf("Huffman Output count : "); 
        pretty_print(count_letters(output_huffman_file));
        end = clock();
        
    }
    else{
        begin = clock();
        huffman_compression_opti(input_file, output_huffman_file);
        printf("Huffman Output count : ");
        pretty_print(count_letters(output_huffman_file));
        end = clock();
    }
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    double ratio = 100 * (double)count_letters(output_huffman_file)/(double)count_letters(output_file);
    printf("Compression rate : %.2f%% In (only) %.3f seconds\n", ratio, time_spent);
    
  



}