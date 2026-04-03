#include<stdio.h>
#include<string.h>
#include "common.h"
#include "decode.h"
#include "types.h"

Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo)
{
    if(argv[2]==NULL)
    {
       printf("For Decoding ,please pass minimum 3 arguments like -d,stego.bmp,output.bmp\n");
       return e_failure;
    }

    if(strchr(argv[2],'.')!=NULL)
    {
    if(strcmp(strchr(argv[2],'.'),".bmp")==0)
    {
	decInfo->stego_image_fname=argv[2];
    }
    else
    {
	//printf("Error:pass valid output file\n");
	return e_failure;
    }
    }
    else
    {
	//printf("Error:no extension given for output file\n");
	return e_failure;
    }
    if(argv[3]!=NULL)
    {
	strcpy(decInfo->secret_fname,strtok(argv[3],"."));
    }
    else
    {
	strcpy(decInfo->secret_fname,"final");
    }
    return e_success;
}

Status open_decode_file(DecodeInfo *decInfo)
{
{
    decInfo->fptr_stego_image=fopen(decInfo->stego_image_fname,"r");
    if(decInfo->fptr_stego_image==NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
	return e_failure;
    }
    return e_success;
}
    // Src Image file
  /* decInfo->fptr_src_image = fopen(decInfo->src_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->src_image_fname);

	return e_failure;
    }

    // Secret file
  //  decInfo->fptr_secret = fopen(decInfo->secret_fname, "r");
    // Do Error handling
   if (decInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);

	return e_failure;
    }

    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "w");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

	return e_failure;
    }

    // No failure return e_success
    //return e_success;*/
}
Status decode_magic_string(DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_stego_image,54,SEEK_SET);
    int len=strlen(MAGIC_STRING);
    char buf[len];
   // printf("b magic string %ld\n",ftell(decInfo->fptr_stego_image));
    decode_image_to_data(buf,len,decInfo->fptr_stego_image);
   // printf("a magic string %ld\n",ftell(decInfo->fptr_stego_image));
    if(strcmp(buf,MAGIC_STRING)==0)
    {
	// printf("Magic string=%s\n",buf);
	return e_success;
    }
    return e_failure;
}
Status decode_image_to_data(char *data,int size,FILE *fptr_stego_image)
{
    char buff[8];
    int i=0;
    for( i=0;i<size;i++)
    {
	fread(buff,8,1,fptr_stego_image);
	data[i]=decode_lsb_to_byte(buff);
    }
    data[i]='\0';
}
Status decode_lsb_to_byte(char *buffer)
{
    char data=0;
    for(int i=0;i<8;i++)
    {
	data=buffer[i]&0x01|data;
	if(i<7)
	{
	    data=data<<1;
	}

    }
    return data;
}
Status decode_secret_extn_size(DecodeInfo *decInfo)
{
    char image_buffer[32];
  //  int file_size = 0;
    // printf("b secret extn size => %ld\n",ftell(decInfo->fptr_stego_image));
    fread(image_buffer,1,32,decInfo->fptr_stego_image);
       int  data=0;
    for(int i=0;i<32;i++)
    {
	         data = ((image_buffer[i] & 1 )<<(31-i)) | data;

    }
  // printf("%d\n",data);
   // printf("len ----> %d\n",file_size);
   // printf("a secret extn size => %ld\n",ftell(decInfo->fptr_stego_image));
    if(decode_secret_extn(data,decInfo)==e_success)
    {
	printf("Decoded of secret extn sucessfully.\n\n");
    }
    else
    {
	printf("decode of secret extn failed\n");
	return e_failure;
    }

    return e_success;
}
Status decode_secret_extn(int file_extn_size,DecodeInfo *decInfo)
{
    char file_extn[file_extn_size];
   // printf("file_extn_size=%d\n",file_extn_size);
   // printf("b decode secret extn %ld\n",ftell(decInfo->fptr_stego_image));
    decode_image_to_data(file_extn, file_extn_size, decInfo->fptr_stego_image);
   // printf("a decode secret extn %ld\n",ftell(decInfo->fptr_stego_image));
    // printf("file_extn_size ----- %d\n",file_extn_size);
    // printf("file_extn ------ %s\n",file_extn);
    decInfo->file_extension = file_extn;
     printf("name=%s\n",decInfo->secret_fname);
    char *pt;
    //strcpy(decInfo->secret_fname,strcat(decInfo->secret_fname,decInfo->file_extension));
    strcat(decInfo->secret_fname,file_extn);
     printf("final =%s\n\n",decInfo->secret_fname);
   //strcat(decInfo->secret_fname,file_extn);


  //open_decode_file(decInfo);

    return e_success;
}
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char image[32];
    int file_size=0;
    fread(image,1,32,decInfo->fptr_stego_image);
    for(int i=0;i<32;i++)
    {
	file_size=image[i] & 0x01|file_size;
	if(i<31)
	{
	    file_size=file_size<<1;
	}
    }

    if(decode_secret_file_data(file_size,decInfo)==e_success)
    {
	printf("Decoded of secret data Sucessfully.\n\n");
    }
    else
    {
	printf("decode of secret data failed\n");
	return e_failure;
    }

    return e_success;
}
Status decode_secret_file_data(int file_size,DecodeInfo *decInfo)
{
    char file_data[file_size];
   // printf("b secret file data %ld\n",ftell(decInfo->fptr_stego_image));
    decode_image_to_data(file_data,file_size,decInfo->fptr_stego_image);
   // printf("a secret file data %ld\n",ftell(decInfo->fptr_stego_image));
   // printf("file size=%d\n",file_size);
    printf("file data=%s\n",file_data);
    decInfo->fptr_secret=fopen(decInfo->secret_fname,"w");
    fwrite(file_data,1,file_size,decInfo->fptr_secret);
    return e_success;
}
Status do_decoding(DecodeInfo *decInfo)
{
   if(open_decode_file(decInfo)==e_success)
    {
	printf("Files are opened Sucessfully.\n\n");
	if(decode_magic_string(decInfo)==e_success)
	{
	    printf("Decoded  magic string Sucessfully.\n\n");
	    if(decode_secret_extn_size(decInfo)==e_success)
	    {
		printf("Decoded secret extn size  Sucessfully.\n\n");
		if(decode_secret_file_size(decInfo)==e_success)
		{
		    printf("Decoded secret file size Sucessfully.\n\n");
		}
		else
		{
		    printf("decode of secret file size failed!!\n");
		}

	    }
	    else
	    {
		printf("decode of secret extn size is failed!!\n");
		return e_failure;
	    }
	}
	else
	{
	    printf("decode of magic string failed!!\n");
	    return e_failure;
	}


    }
    else
    {
	printf("Files are not opened!!\n");
	return e_failure;
    }
    
    return e_success;
}
