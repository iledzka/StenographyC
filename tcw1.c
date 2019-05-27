

/**
 * cw1.c
 *
 * Iza Ledzka
 *
 * Steganography program used to encode and decode text messages inside a file.
 * Accepts PPM in the Plan PPM format (magic number P3)
 * http://netpbm.sourceforge.net/doc/ppm.html
 *
 * To compile:
 * gcc -o steg tcw1.c
 *
 * Tu run:
 * ./steg e file1.ppm > file2.ppm
 * This call allows to encode a message inside file2.ppm.
 * The program will prompt you for a secret (only integers with less than 20 digits long)
 * Then you should see a prompt for a message, restricted to 256 characters.
 * After inputting message, press enter.
 *
 * steg d file.ppm
 * This call allows to read a message encoded inside file,.ppm
 * The program will prompt for a secret (integer of less than 20 digits length)
 * When correct secret provided, a message will appear in stdout
 * Incorrect secret returns unreadable message
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ppm.h file contains datatypes and functions related to ppm file
#include "ppm.h"

#define MAX_SECRET_LENGTH  20
#define MAX_MSG_LENGTH     256

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 3)
    {
        printf("Usage: ./steg [flag] [infile]\n");
        return 1;
    }
    
    // store secret and message - will be entered by user
    char secret[MAX_SECRET_LENGTH];
    char message[MAX_MSG_LENGTH];
   
    int secretInt = 0;
    
    // remember filename
    char* infile = argv[2];
    
    // get flag
    char* flag = argv[1];
    
    // prompt for a secret
    fprintf(stderr, "Please enter the secret (restricted to %d characters)\n", MAX_SECRET_LENGTH);
    fgets(secret, MAX_SECRET_LENGTH, stdin);
    sscanf(secret, "%d", &secretInt);
    if (secretInt <= 0){
        fprintf(stderr, "Error when reading the secret. Are you sure you entered no more than %d integers?\n", MAX_SECRET_LENGTH);
        exit(1);
    }
    fprintf(stderr, "\nSaving secret: %d\n", secretInt);
    
    // open input file
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }
    
    // get saved image data
    struct PPM *imageData = getPPM(inptr);
 
    if (strcmp(flag, "e") == 0) {
        // prompt for a message
        fprintf(stderr, "\nPlease enter the enter the message (restricted to %d characters)\n", MAX_MSG_LENGTH);
        fgets(message, sizeof(message), stdin);
        fprintf(stderr, "Saving message: %s\n\n", message);
        
        // encode message in the image and print it to stdout
        struct PPM * encodedImg = encode(imageData, message, strlen(message), secretInt);
        if (!encodedImg) {
            fprintf(stderr, "Unable to encode the message");
            exit(1);
        }
        showPPM(encodedImg);
        freePPM(imageData);
        
    } else if (strcmp(flag, "d") == 0) {
        printf("\nDecoding message...\n");
        char * returnedMsg = decode(imageData, secretInt);
        
        fprintf(stderr, "Retrieved message:\n%s", returnedMsg);
        free(returnedMsg);
        freePPM(imageData);
        
    }
    
    // close infile
    fclose(inptr);
    
    return 0;
}
