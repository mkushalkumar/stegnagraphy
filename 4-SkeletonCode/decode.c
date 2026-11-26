#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"
#include "types.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (strstr(argv[2], ".bmp") != NULL)
    {
        decInfo->stego_image_fname = argv[2];
        if (argv[3] != NULL)
        {
            decInfo->secret_fname = argv[3];
            return e_success;
        }
        else
        {
            decInfo->secret_fname = "data";
            return e_success;
        }
    }
    else
    {
        return e_failure;
    }
}

Status do_decoding(DecodeInfo *decInfo)
{

    if (decode_open_files(decInfo) == e_success)
    {
        printf("file opened successfully\n");
    }
    else
    {
        return e_failure;
    }

    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    if (decode_magic_string_size(decInfo) == e_success)
    {
        printf("Magic string size decoded successfully\n");
    }
    else
    {
        printf("Magic string size decode error");
        return e_failure;
    }

    if (decode_magic_string(decInfo) == e_success)
    {
        printf("Decoded the magic string successfully\n");
    }
    else
    {
        printf("magic string decode error\n");
        return e_failure;
    }

    printf("Enter the magic string\n");
    scanf(" %s", decInfo->check_magic_string);

    if (strcmp(decInfo->magic_string, decInfo->check_magic_string) == 0)
    {
        printf("magic string matched\n");
    }
    else
    {
        printf("Wrong magic string\n");
        return e_failure;
    }

    if (decode_extension_size(decInfo) == e_success)
    {
        printf("decoded the extension size\n");
    }
    if (decode_extension(decInfo) == e_success)
    {
        printf("decoded the extension\n");
    }

    /*to add extension type in secret file and also open the file */
    char secret_file[100];
    strcpy(secret_file, decInfo->secret_fname);
    strcat(secret_file, decInfo->extension);
    decInfo->secret_fname = secret_file;

    if (secret_open_file(decInfo) == e_success)
    {
        printf("secret data storage file is created with sitable extension\n");
    }

    /*to copy the secret data into file*/
    if (decode_secret_data_size(decInfo) == e_success)
    {
        printf("decoded the secret data size\n");
    }

    if (decode_secret_data(decInfo) == e_success)
    {
        printf("decoded the secret data size\n");
    }
    return e_success;
}

Status decode_open_files(DecodeInfo *decInfo)
{
    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

        return e_failure;
    }

    return e_success;
}

Status secret_open_file(DecodeInfo *decInfo)
{
    // secret file
    decInfo->secret_data = fopen(decInfo->secret_fname, "wb");
    // Do Error handling
    if (decInfo->secret_data == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);

        return e_failure;
    }

    return e_success;
}

Status decode_magic_string_size(DecodeInfo *decInfo)
{
    char arr[32];
    decInfo->magic_string_size = 0;
    fread(arr, 32, 1, decInfo->fptr_stego_image);
    decode_lsb_to_magic_size(decInfo, arr);

    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
    decode_image_to_data(decInfo, decInfo->magic_string_size);
    return e_success;
}

Status decode_lsb_to_magic_size(DecodeInfo *decInfo, char *image_buffer)
{
    int binary = 1;

    for (int i = 0; i < 32; i++)
    {
        int temp = image_buffer[i] & 1;
        decInfo->magic_string_size = temp * binary + decInfo->magic_string_size;
        binary = binary * 2;
    }
    return e_success;
}

Status decode_image_to_data(DecodeInfo *decInfo, int size)
{
    char arr[8];
    int i;
    for (i = 0; i < size; i++)
    {
        fread(arr, 8, 1, decInfo->fptr_stego_image);
        decInfo->magic_string[i] = decode_lsb_to_byte(decInfo, arr);
    }
    decInfo->magic_string[i] = '\0';
    return e_success;
}

char decode_lsb_to_byte(DecodeInfo *decInfo, char *image_buffer)
{
    unsigned char ch = 0;
    int binary = 1;

    for (int i = 0; i < 8; i++)
    {
        int temp = image_buffer[i] & 1;
        ch = ch + (temp * binary);
        binary = binary * 2;
    }

    return ch;
}

Status decode_extension_size(DecodeInfo *decInfo)
{
    char arr[32];
    decInfo->extension_size = 0;
    fread(arr, 32, 1, decInfo->fptr_stego_image);
    decode_lsb_to_extension_size(decInfo, arr);

    return e_success;
}

Status decode_lsb_to_extension_size(DecodeInfo *decInfo, char *image_buffer)
{
    int binary = 1;

    for (int i = 0; i < 32; i++)
    {
        int temp = image_buffer[i] & 1;
        decInfo->extension_size = temp * binary + decInfo->extension_size;
        binary = binary * 2;
    }
    return e_success;
}

Status decode_extension(DecodeInfo *decInfo)
{
    decode_image_to_extension_data(decInfo, decInfo->extension_size);

    return e_success;
}

Status decode_image_to_extension_data(DecodeInfo *decInfo, int size)
{
    char arr[8];
    int i;
    for (i = 0; i < size; i++)
    {
        fread(arr, 8, 1, decInfo->fptr_stego_image);
        decInfo->extension[i] = decode_lsb_to_byte(decInfo, arr);
    }
    decInfo->extension[i] = '\0';

    return e_success;
}

Status decode_secret_data_size(DecodeInfo *decInfo)
{
    char arr[32];
    decInfo->decode_data_size = 0;
    fread(arr, 32, 1, decInfo->fptr_stego_image);
    decode_lsb_to_secret_data_size(decInfo, arr);

    return e_success;
}

Status decode_lsb_to_secret_data_size(DecodeInfo *decInfo, char *image_buffer)
{
    int binary = 1;

    for (int i = 0; i < 32; i++)
    {
        int temp = image_buffer[i] & 1;
        decInfo->decode_data_size = temp * binary + decInfo->decode_data_size;
        binary = binary * 2;
    }
    return e_success;
}

Status decode_secret_data(DecodeInfo *decInfo)
{
    decode_image_to_secret_data(decInfo, decInfo->decode_data_size);
    return e_success;
}

Status decode_image_to_secret_data(DecodeInfo *decInfo, int size)
{
    char arr[8];
    for (int i = 0; i < size; i++)
    {
        fread(arr, 8, 1, decInfo->fptr_stego_image);
        decInfo->character = decode_lsb_to_byte(decInfo, arr);
        decInfo->sec_data[0] = decInfo->character;
        fwrite(decInfo->sec_data, 1, 1, decInfo->secret_data);
    }
    return e_success;
}
