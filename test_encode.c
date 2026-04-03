#include <stdio.h>
#include "decode.h"
#include "encode.h"
#include "types.h"
#include<string.h>
#include "common.h"

int main(int argc,char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    int ret,ret2;
    ret=check_operation_type(argc,argv);   //ret=call check operation type()
    
    if(ret==e_encode)  //check ret==e_encode
      {   
         
            //yes->ret2=call read_and_validate_encode_args

        if(read_and_validate_encode_args(argv,&encInfo)==e_success)
        {
          printf("INFO : Selected Encoding,Encoding Started.\n\n");

            printf("INFO : Read and validate is successfully completed.\n\n");
           if(do_encoding(&encInfo)==e_success)
           {
            printf("\n\n---------------------");
            printf("\nINFO : Encoding Completed Successfully.");
            printf("\n-----------------------");
           }
        }
      }

    if(ret==e_decode)    //check ret==e_decode
    {
        //yes->//call read_and_validate_decode_args()
       if (read_and_validate_decode_args(argv,&decInfo)==e_success);
       {
         printf("INFO : Selected Decoding,Decoding Started.\n\n");

        printf("INFO : Read and validate is successfully completed.\n\n");
         if(do_decoding(&decInfo)==e_success)
          {
             printf("\n---------------------");
             printf("\nINFO : Decoding Completed Successfully.");
             printf("\n-----------------------\n\n");
          }
       }
    }
        
    if(ret==e_unsupported)
    {
        return e_failure;
    }
    
}

OperationType check_operation_type(int argc,char *argv[])
{ 
    if(argv[1]==NULL)
    {
              printf("INFO : please pass valid arguments.\n");
              printf("INFO : Encoding - minimum 4 arguments.\n");
              printf("INFO : Decoding - minimum 3 arguments\n");
    }

        //check argc is non 0?
        //check first arg is -e?
       else if(strstr("-e",argv[1]))
         {
          //printf("encode");
           return e_encode;
         }
          //check first arg is -d
          else if(strstr("-d",argv[1]))
          {
            //printf("decode");
             return e_decode;
          }
         else
          {
              printf("INFO : please pass valid arguments.\n");
              printf("INFO : Encoding - minimum 4 arguments.\n");
              printf("INFO : Decoding - minimum 3 arguments\n");
          }
              return e_unsupported;
   
 
}