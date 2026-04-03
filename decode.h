#ifndef DECODE_H
#define DECODE_H


#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _decodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char secret_fname[20];
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;
    char *file_extension;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    char *data;

   
    

} DecodeInfo;
 

 /* Encoding function prototype */

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_decode_file(DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/*encode_data_to_image*/
Status decode_image_to_data(char *data,int size,FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
Status decode_lsb_to_byte(char *buffer);

/* Encode secret file size extenstion */
Status decode_secret_extn_size(DecodeInfo *decInfo);

/* Encode secret file extenstion */
Status decode_secret_extn(int file_extn_size,DecodeInfo *decInfo);

/* Encode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Encode secret file data*/
Status decode_secret_file_data(int file_size,DecodeInfo *decInfo);


#endif