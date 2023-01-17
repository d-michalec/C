/*
Damian Michalec

Operating Systems
Regis University

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

int main(int argc, char **argv)
{

     // checking whether the appropiate amount of arguments was passed on. (3)
    if (argc != 3)
    {
        fprintf(stderr, "mycopy: invalid number of arguments.\n usage:  filecompare <sourcefile> <destinationfile>\n");
        exit(1);
    }
    // Opening two files
    FILE *file1 = fopen(argv[1], "rb");
    FILE *file2 = fopen(argv[2], "rb");

    // Check if the files were opened successfully
    if (file1 == NULL || file2 == NULL) {
        printf("Error opening one of the files!\n");
        return 1;
    }

    // Read the contents of the two files into memory
    fseek(file1, 0, SEEK_END);
    fseek(file2, 0, SEEK_END);
    long file1_size = ftell(file1);
    long file2_size = ftell(file2);
    fseek(file1, 0, SEEK_SET);
    fseek(file2, 0, SEEK_SET);
    unsigned char *data1 = (unsigned char*) malloc(file1_size);
    unsigned char *data2 = (unsigned char*) malloc(file2_size);
    fread(data1, 1, file1_size, file1);
    fread(data2, 1, file2_size, file2);

   // Compute the SHA256 hash of each file's contents
    unsigned char hash1[SHA256_DIGEST_LENGTH];
    unsigned char hash2[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256_ctx;
    SHA256_Init(&sha256_ctx);
    SHA256_Update(&sha256_ctx, data1, file1_size);
    SHA256_Final(hash1, &sha256_ctx);
    SHA256_Init(&sha256_ctx);
    SHA256_Update(&sha256_ctx, data2, file2_size);
    SHA256_Final(hash2, &sha256_ctx);

    // Compare the hashes byte by byte
    if (memcmp(hash1, hash2, SHA256_DIGEST_LENGTH) == 0) {
        printf("The two files are identical!\n");
    } else {
        printf("The two files are not identical.\n");
    }

    // Clean up
    fclose(file1);
    fclose(file2);
    free(data1);
    free(data2);

    return 0;
}