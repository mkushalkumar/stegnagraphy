#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

typedef struct _DecodeInfo
{
    /*stego file*/
    FILE *fptr_stego_image;
    char *stego_image_fname;

    /*magic string info*/
    int magic_string_size;
    char magic_string[50];
    char check_magic_string[50];

    /*to store the decoded character*/
    char character;

    /*extension info*/
    int extension_size;
    char extension[5];

    /*decode data info*/
    int decode_data_size;
    char sec_data[1];
    FILE *secret_data;
    char *secret_fname;

} DecodeInfo;

/* Read and validate decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/*Perform the decoding*/
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status decode_open_files(DecodeInfo*decInfo);

/*open  secret file  */
Status secret_open_file(DecodeInfo*decInfo);

/*decode a lsb into magic string size  */
Status decode_lsb_to_magic_size(DecodeInfo*decInfo, char *image_buffer);

/*decode a lsb into size of image data array */
char decode_lsb_to_byte(DecodeInfo*decInfo, char *image_buffer);

/*store magic string size*/
Status decode_magic_string_size(DecodeInfo *decInfo);

/* store magic string */
Status decode_magic_string(DecodeInfo *decInfo);

/*decode a lsb into extension size*/
Status decode_lsb_to_extension_size(DecodeInfo*decInfo, char *image_buffer);

/*store extension size */
Status decode_extension_size(DecodeInfo *decInfo);

/* store extension */
Status decode_extension(DecodeInfo *decInfo);

/*store extension data*/
Status decode_image_to_extension_data(DecodeInfo *decInfo, int size);

/*store the secret data size*/
Status decode_secret_data_size(DecodeInfo*decInfo);

/**/
Status decode_lsb_to_secret_data_size(DecodeInfo*decInfo, char *image_buffer);


/*store decoded secret data.*/
Status decode_secret_data(DecodeInfo*decInfo);

/* decode function, which does the real decoding */
Status decode_image_to_secret_data(DecodeInfo*decInfo ,int size);

Status decode_image_to_data(DecodeInfo *decInfo, int size);

#endif