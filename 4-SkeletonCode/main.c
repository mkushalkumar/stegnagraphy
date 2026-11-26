/*documentation :-

Name :- kushal kumar
start date :- 1/2/2026
description :-  the project name is lsb steganography.In this we secret store data in lsb byte i. is encoding.
                We decode data that is store in lsb and open in suitable extension.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    // check whether the minimum number of input is there or not
    if (argc < 3)
    {
        printf("Error ! Enter the minimum number of input\n");
        exit(EXIT_SUCCESS);
    }

    if (check_operation_type(argv) == e_encode)
    {
        EncodeInfo encInfo;
        if (read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            if (do_encoding(&encInfo) == e_success)
            {
                printf("ENCODED SUCCESSFULLY\n");
            }
        }
        else
        {
            printf("Error ! Enter the valid file extension \n");
            exit(EXIT_SUCCESS);
        }
    }
    else if (check_operation_type(argv) == e_decode)
    {
        DecodeInfo decInfo;

        if (read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            if (do_decoding(&decInfo) == e_success)
            {
                printf("DECODED SUCCESSFULLY\n");
            }
            else
            {
                printf("error\n");
                exit(EXIT_SUCCESS);
            }
        }
        else
        {
            printf("Error ! Enter the valid file extension \n");
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        printf("Error ! enter the correct operation type\n");
        exit(EXIT_SUCCESS);
    }
}
