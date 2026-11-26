#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encode.h"
#include "types.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_success)
    {
        printf("file opened successfully\n");
    }
    else
    {
        return e_failure;
    }

    if (check_capacity(encInfo) == e_success)
    {
        printf("Has capacity to store the secret message \n");
    }
    else
    {
        printf("error ! no capacity\n");
        return e_failure;
    }
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("header file is copied successfully\n");
    }
    if (encode_magic_string_size(encInfo) == e_success)
    {
        printf("magic string size is encoded\n");
    }
    if (encode_magic_string(encInfo) == e_success)
    {
        printf("magic string is encoded\n");
    }
    if (encode_secret_file_extn_size(encInfo) == e_success)
    {
        printf("extension size is encoded\n");
    }
    if (encode_secret_file_extn(encInfo) == e_success)
    {
        printf("extension is encoded\n");
    }
    if (encode_secret_file_size(encInfo) == e_success)
    {
        printf("secret data size is encoded\n");
    }
    if (encode_secret_file_data(encInfo) == e_success)
    {
        printf("secret data is encoded\n");
    }
    if (copy_remaining_img_data(encInfo) == e_success)
    {
        printf("Remaining data copied successfully\n");
    }
    return e_success;
}

Status copy_remaining_img_data(EncodeInfo *encInfo)
{
    while (fread(encInfo->image_data, 8, 1, encInfo->fptr_src_image) == 1)
    {
        fwrite(encInfo->image_data, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_magic_string_size(EncodeInfo *encInfo)
{
    char arr[32];

    fread(arr, 32, 1, encInfo->fptr_src_image);

    encode_size_to_lsb(strlen(encInfo->magic_string), arr);

    fwrite(arr, 32, 1, encInfo->fptr_stego_image);

    return e_success;
}

Status encode_size_to_lsb(int data, char *image_buffer)
{
    for (int i = 0; i < 32; i++)
    {
        image_buffer[i] = image_buffer[i] & ~1;
        int temp = data & (1 << i);
        temp = temp >> i;
        image_buffer[i] = image_buffer[i] | temp;
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        image_buffer[i] = image_buffer[i] & ~1;
        int temp = data & (1 << i);
        temp = temp >> i;
        image_buffer[i] = image_buffer[i] | temp;
    }
    return e_success;
}

Status encode_magic_string(EncodeInfo *encInfo)
{
    encode_data_to_image(encInfo->magic_string, strlen(encInfo->magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char arr[8];

    for (int i = 0; i < size; i++)
    {

        fread(arr, 8, 1, fptr_src_image);
        encode_byte_to_lsb(data[i], arr);
        fwrite(arr, 8, 1, fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_extn_size(EncodeInfo *encInfo)
{
    char arr[32];

    fread(arr, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(strlen(encInfo->extn_secret_file), arr);
    fwrite(arr, 32, 1, encInfo->fptr_stego_image);

    return e_success;
}

Status encode_secret_file_extn(EncodeInfo *encInfo)
{
    encode_data_to_image(encInfo->extn_secret_file, strlen(encInfo->extn_secret_file), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_size(EncodeInfo *encInfo)
{
    char arr[32];
    fread(arr, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(encInfo->size_secret_file, arr);
    fwrite(arr, 32, 1, encInfo->fptr_stego_image);

    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
    char arr[8];

    while (fread(&ch, 1, 1, encInfo->fptr_secret) == 1)
    {
        fread(arr, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(ch, arr);
        fwrite(arr, 8, 1, encInfo->fptr_stego_image);
    }

    return e_success;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    fseek(fptr_src_image, 0, SEEK_SET);
    char arr[60];
    fread(arr, 54, 1, fptr_src_image);
    fwrite(arr, 54, 1, fptr_dest_image);
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    printf("Enter the magic string : ");
    scanf("%s", encInfo->magic_string);

    fseek(encInfo->fptr_secret, 0, SEEK_END);
    encInfo->size_secret_file = ftell(encInfo->fptr_secret);
    int size_of_info = (4 + strlen(encInfo->magic_string) + 4 + strlen(encInfo->extn_secret_file) + 4 + encInfo->size_secret_file);
    fseek(encInfo->fptr_secret, 0, SEEK_SET);
    if (encInfo->image_capacity > size_of_info * 8)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

OperationType check_operation_type(char *argv[])
{
    if (strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if (strstr(argv[2], ".bmp") != NULL)
    {
        encInfo->src_image_fname = argv[2];
        if (strstr(argv[3], ".txt") != NULL)
        {
            encInfo->secret_fname = argv[3];
            strcpy(encInfo->extn_secret_file, strstr(argv[3], "."));
            if (argv[4] != NULL)
            {

                if (strstr(argv[4], ".bmp") != NULL)
                {
                    encInfo->stego_image_fname = argv[4];
                    return e_success;
                }
            }
            else
            {
                encInfo->stego_image_fname = "stego.bmp";
                return e_success;
            }
        }
    }
    return e_failure;
}
