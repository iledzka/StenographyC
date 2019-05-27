
/**
 * ppm.h
 *
 * Iza Ledzka
 *
 * PPM-related data types and functions.
 */

#include <stdint.h>


/**
 * RGBTRIPLE
 *
 * This structure describes a color consisting of relative intensities of
 * red, green, and blue, which forms a single pixel.
 *
 */
typedef struct
{
    int  red;
    int  green;
    int  blue;
}
RGBTRIPLE;

/**
 * PPM
 *
 * The PPM structure contains information about the type:
 * 2 letter code, width, height, max colour value, pointer to 2D array containing the pixels.
 *
 */
struct PPM
{
    char*  code;  // 2 bytes P6 (or P3) code
    unsigned int    width; // pointer to char so various length can be saved
    unsigned int    height; // as above
    unsigned int    maxVal; // max colour value, usually 255
    char**  comments; // 2D array of comments
    int noOfComments; // helper var to store the number of comments - useful when freeing memory
    RGBTRIPLE** image; // 2D array of pixels
};

/*
 * return a C structure PPM image from file “f”
 */
struct PPM * getPPM(FILE * f) {
    // read infile's information
    struct PPM *imgData = (struct PPM *)malloc(sizeof(struct PPM));
    if (!imgData) {
        fprintf(stderr, "Error allocating memory\n");
        exit(1);
    }
    // get code by copiyng first 2 bytes of the file
    imgData->code=malloc(4*sizeof(char));
    if (!fgets(imgData->code, sizeof(imgData->code), f)) {
        fprintf(stderr, "Could not read file code.\n");
        return NULL;
    }
    // remove NL
    imgData->code[strcspn(imgData->code, "\n")] = 0;
    //check the image format
    if (imgData->code[0] != 'P' || imgData->code[1] != '3') {
        fprintf(stderr, "Invalid image format (must be 'P3')\n");
        fclose(f);
        exit(1);
    }
    
    fprintf(stderr, "imgData.code %s.\n", imgData->code);
    
    // check for comments
    int c = getc(f);
    int commentsNum = 0;
    imgData->comments = 0;
    while (c == '#') {
        commentsNum++;
        char** arrayOfComments = (char**)realloc(imgData->comments, commentsNum*sizeof(char*));
        if (!arrayOfComments) {
            // needs to free comments
            fprintf(stderr, "Couldn't allocate memory for image comments.\n");
            exit(1);
        }
        imgData->comments = arrayOfComments;
        imgData->comments[commentsNum-1] = (char*)malloc(256*sizeof(char));
        if (!imgData->comments[commentsNum-1]) {
            // needs to free numbers
            exit(1);
        }
        if (!fgets(imgData->comments[commentsNum-1], 256, f)) {
            fprintf(stderr, "Error when reading file comments.\n");
            fclose(f);
            exit(1);
        }
        fprintf(stderr, "imageData->comment %i %s", commentsNum-1, imgData->comments[commentsNum-1]);
        c = getc(f);
    }
    ungetc(c, f);
    
    // remember how many comments are saved
    imgData->noOfComments = commentsNum;
    
    // get width and height
    if (fscanf(f, "%d %d", &imgData->width, &imgData->height) != 2) {
        fprintf(stderr, "Invalid image size (error loading infile)\n");
        exit(1);
    }
    
    // get max rgb val
    if (fscanf(f, "%d", &imgData->maxVal) != 1) {
        fprintf(stderr, "Invalid rgb max value (error loading infile)\n");
        exit(1);
    }
    
    fprintf(stderr, "imgData->width %d.\n", imgData->width);
    fprintf(stderr, "imgData->height %d.\n", imgData->height);
    fprintf(stderr, "imgData->maxVal %d.\n", imgData->maxVal);
    
    // skip new line
    while (fgetc(f) != '\n') ;
    
    
    // allocate memory for image data
    imgData->image = malloc(imgData->height * sizeof(RGBTRIPLE*));
    if (!imgData->image) {
        fprintf(stderr, "Couldn't allocate memory for image data.\n");
        exit(1);
    }
    
    // write image data to memory
    unsigned int i, j;
    for (i = 0; i < imgData->height; i++) {
        // allocate space for new row
        imgData->image[i] = malloc(imgData->width * sizeof(RGBTRIPLE));
        
        for (j = 0; j < imgData->width; j++) {
            // temporary storage
            RGBTRIPLE triple;
            
            char buf[13] = { 0 };
            //fprintf(stderr, "size of buffer: %lu.\n", sizeof(buf));
            if (fgets(buf, sizeof(buf), f) && !feof(f)){
                if (ferror(f)) fprintf(stderr, "Error in fget function`; Input error.\n");
                if (feof(f)) fprintf(stderr, "Error in fget function: EOF.\n");
                sscanf(buf, "%d %d %d", &triple.red, &triple.green, &triple.blue);
                // save it to memory
                imgData->image[i][j] = triple;
            }
        }
    }
    return imgData;
}

/*
 * display the PPM image “im” as text
 */
void showPPM(struct PPM * im) {
    //write the header file for a new file
    //image format
    printf("%s\n", im->code);
    
    //comments
    if (im->comments && im->noOfComments){
        for (int i = 0; i < im->noOfComments; i++) {
            printf("#%s", im->comments[i]);
        }
    }
    
    //image size
    printf("%d %d\n",im->width,im->height);
    
    // rgb component depth
    printf("%d\n",im->maxVal);
    
    unsigned int i, j;
    // read image data from memory
    for (i = 0; i < im->height; i++) {
        for (j = 0; j < im->width; j++) {
            // write RGB triple to outfile
            RGBTRIPLE triple = im->image[i][j];
            printf("%d %d %d\n", triple.red, triple.green, triple.blue);
        }
    }
}

/*
 * Determine pixel position (uses srand() and rand())
 * Pixel position is hardcoded to be between 1 and 11.
 */
int pixelPosition(int seed){
    srand(seed);
    unsigned int rnum = rand() % 10;
    
    return rnum+1;
}

/*
 * return a modified copy of PPM image “im” with “message” characters hidden in its pixels using “secret”
 * “mSize” is the size of “message”
 */
struct PPM * encode(struct PPM * im, char * message, unsigned int mSize, unsigned int secret) {
    if (mSize > im->width*im->height) {
        fprintf(stderr, "Provided image is too small to encode a message of size %d", mSize);
        exit(1);
    }

    unsigned int pxlPosition = pixelPosition(secret);
    unsigned int h = 0, w = pxlPosition; // domentions of image 2d array
    unsigned int y, z;
    unsigned char bit;
    // iterate over each character in a message
    for (y = 0; y < mSize; y++) {
        //iterate over 8 bits in a char
        for (z = 0; z < 8; z += 3){
            bit = (message[y] >> z) & 1;
            //iterate over pixels in image date
            RGBTRIPLE triple = im->image[h][w];
            int red = triple.red;
            int green = triple.green;
            int blue = triple.blue;
            // change LSB in rgbValue
            red = (red & ~(1 << 0)) | (bit << 0);
            bit = (message[y] >> (z+1)) & 1;
            green = (green & ~(1 << 0)) | (bit << 0);
            bit = (message[y] >> (z+2)) & 1;
            blue = (blue & ~(1 << 0)) | (bit << 0);
            triple.red = red;
            triple.green = green;
            triple.blue = blue;
            
            im->image[h][w] = triple;
            
            // update image 2d array indices
            w += pxlPosition;
            if (w >= im->width) {
                h++;
                w = 0;
            }
        }
    }
    return im;
}

/*
 * return the message hidden in the pixels of PPM image “im” using secret
 */
char * decode(struct PPM * im, unsigned int secret) {
    char* message = (char*)malloc(256*sizeof(char));
    if (message == NULL){
        fprintf(stderr, "Error allocating message space in decode.");
        exit(1);
    }
    
    unsigned int pxlPosition = pixelPosition(secret);
    
    unsigned int h = 0, w = pxlPosition; // dimentions of image 2d array

    // buffer used to save LSB bits representation
    unsigned char buffer[9] = { 0 };
    // this char is used to store characted that wad read from buffer
    unsigned char ch = 0;
    unsigned  msgLen = 0;
    // iterate over pixels until NL is read (always at the end of message!)
    while (ch != 10) {
        // iterate over bits in a char
        for (int y = 0; y < 8; y += 3) {
            RGBTRIPLE triple = im->image[h][w];
            int red = triple.red;
            int green = triple.green;
            int blue = triple.blue;
            // build up the buffer holding all bits of a single char
            buffer[y] = (red >> 0) & 1;
            buffer[y+1] = (green >> 0) & 1;
            buffer[y+2] = (blue >> 0) & 1;
            
            // check if a char has been found
            if (y == 6) {
                // save bits to char
                for (int z = 7; z >= 0; z--){
                    ch += (buffer[z] << z);
                }
                if (ch == 10){
                    message[msgLen] = '\0';
                    break; // end of message
                }
                message[msgLen] = ch;
                msgLen++;
                ch = 0;
            }
            // update image 2d array indices
            w += pxlPosition;
            if (w >= im->width) {
                h++;
                w = 0;
            }
        }
    }
    return message;
}

/*
 * free malloc'ed memory
 */
void freePPM(struct PPM *im)
{
    for (unsigned int i = 0; i < im->height; i++){
        free(im->image[i]);
    }
    free(im->image);
    free(im->code);
    for (int y = 0; y < im->noOfComments; y++) {
        free(im->comments[y]);
    }
    free(im->comments);
    free(im);
}
