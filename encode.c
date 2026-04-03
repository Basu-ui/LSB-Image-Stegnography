#include <stdio.h>
#include "encode.h"
#include "types.h"
#include"common.h"
#include<string.h>

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
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");   //tell the rason why file is not opened
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
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
Status read_and_validate_encode_args(char*argv[],EncodeInfo *encInfo)
{
   if(argv[2]==NULL || argv[3]==NULL )
    {
       printf("For Encoding ,please pass minimum 4 arguments like -e,beautiful.bmp,secret.txt\n");
      return e_failure;
    }

    //1.check argv[2] is .bmp file?using strstr//if(strstr(argv[2],".bmp")!==NULL)
    //1.check argv[2] is not .bmp file?
    //if(argc>3) 
    if((strstr(argv[2],".bmp"))== NULL)
      {
        return e_failure;           //yes->return failure;
      }
 
    //2.check argv[3] is not "."
    if((strstr(argv[3],".txt"))==NULL) 
       
       {
         //yes->return failure
         return e_failure;
       } 


    //3.check argv[4] is present
    if(argv[4]!=NULL)
      {
        //yes-> store argv[4] to stego_image_fname;
        encInfo->stego_image_fname=argv[4];
      }
      else
      {
        //n0->store default name "stego.bmp"
        encInfo->stego_image_fname="stego.bmp";
      }

       // 4.store argv[2] src_image_fname;
       encInfo->src_image_fname=argv[2];

        //store argv[3]-> secret_fname;
        encInfo->secret_fname=argv[3];

      strcpy(encInfo->extn_secret_file,strstr(encInfo->secret_fname,".")); 

      
      // 5.return success
      return e_success;
    
}
Status check_capacity(EncodeInfo *encInfo)
{
  int size=get_image_size_for_bmp(encInfo->fptr_src_image);
     
   encInfo->size_secret_file  = get_file_size(encInfo->fptr_secret);


  if(size >= 54 + strlen(MAGIC_STRING)*8 + 32 + strlen(encInfo->extn_secret_file)*8 + 32 + encInfo->size_secret_file *8)
  {
    return e_success;
  }

  else
  {
    return e_failure;
  }
}

uint get_file_size(FILE*fptr)
{
  //find the file size
  //use fseek to move the file pointer to the last position
  fseek(fptr,0L,SEEK_END);

  //use ftell to find the size
  long int res=ftell(fptr);
  return res;
}

Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_dest_image)
{
  char buffer[54];
  rewind(fptr_src_image);

  //read the first 54 bytes from src_image
  fread(buffer,54,1,fptr_src_image);
  
  //write the 54 bytes to dest_image
  fwrite(buffer,54,1,fptr_dest_image);

  return e_success;
}

Status encode_magic_string(char *magic_string,EncodeInfo *encInfo)
{
  //call a generic function

  encode_data_to_image(magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image);//file pointers
}

Status encode_data_to_image(char *data,int size,FILE *fptr_src_image,FILE *fptr_stego_image)
{
  char buffer[8];
  //run a loop for size times
  for(int i=0;i<size;i++)
  {
  //use fread to read 8 bytes from the src_image
  fread(buffer,8,1,fptr_src_image);

  //call encode_byte_to_lsb(data[0],buffer)
  encode_byte_to_lsb(data[i],buffer);

  //use fwrite to write the 8 bytes to stego_image
  fwrite(buffer,8,1,fptr_stego_image);
  }
}

Status encode_byte_to_lsb(char data,char *image_buffer)
{
  for(int i=0;i<8;i++)
  {
    //1.clear the lsb bit
    image_buffer[i]=image_buffer[i]&~1;
  //2.get the msb bit
  int get_msb=((unsigned)(data & 1<<7-i)) >> 7-i;
  
  //3. 1. | 2.
  image_buffer[i]=image_buffer[i] | get_msb;

  }
  return e_success;
}

Status encode_size_to_lsb(long extn_size,char *image_buffer)
{
  //run a loop for 32 times
  for(int i=0;i<32;i++)
  {
    //1. clear the lsb bits
    image_buffer[i]=image_buffer[i]&~1;

    //2.get the msb bit
    
      // image_buffer[i] = ( image_buffer[i]   |(( unsigned)(extn_size & 1) << (31 - i)) >> (31 - i));
      image_buffer[i] = ( image_buffer[i]   |(( unsigned)(extn_size >> (31-i))&1 ));
  
     //3. 1. | 2.
    //   image_buffer[i]=image_buffer[i] | get_msb;

  }
  return e_success;
}

Status encode_file_extn_size(long extn_size,EncodeInfo *encInfo)
{
  //step1. char image_buffer[32]
  char image_buffer[32];

  //printf("%ld\n",extn_size);
  
  //printf("1 -> %ld\n",ftell(encInfo -> fptr_stego_image));
  // printf("2 -> %ld\n",ftell(encInfo -> fptr_src_image));
  //step2. read the 32 bytes from sr_image
  fread(image_buffer,32,1,encInfo->fptr_src_image);

  //step3. call size_to_lsb(extn_size,image_buffer)
   encode_size_to_lsb(extn_size,image_buffer);

  //step4. write the 32 bytes to the stego image
  fwrite(image_buffer,32,1,encInfo->fptr_stego_image);
    //printf("1 -> %ld\n",ftell(encInfo -> fptr_stego_image));
  // printf("2 -> %ld\n",ftell(encInfo -> fptr_src_image));

  return e_success;
}

Status encode_secret_file_extn(char *extn,EncodeInfo *encInfo)
{
  encode_data_to_image(extn,strlen(extn),encInfo->fptr_src_image,encInfo->fptr_stego_image);//those file pointers
  return e_success;
}

Status encode_secret_file_size(long file_size,EncodeInfo *encInfo)
{
  //step1. char image_buffer[32]
  char image_buffer[32];

  //step2. read 32 bytes from src_image
   fread(image_buffer,32,1,encInfo->fptr_src_image);

  //step3. call a size_to_lsb
  encode_size_to_lsb(file_size,image_buffer);

  fwrite(image_buffer,32,1,encInfo->fptr_stego_image);

  return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
  char data[encInfo->size_secret_file + 1];
  rewind(encInfo->fptr_secret);
  fread(data,encInfo->size_secret_file,1,encInfo->fptr_secret);
  data[encInfo->size_secret_file]='\0';
  printf("data = %s ",data);

  encode_data_to_image(data,encInfo->size_secret_file,encInfo->fptr_src_image,encInfo->fptr_stego_image);

  return e_success;
}


Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
  char arr;
  while(fread(&arr,1,1,fptr_src))
    {
	    fwrite(&arr,1,1,fptr_dest);
    }
    return e_success;
}


Status do_encoding(EncodeInfo *encInfo)
{
  if(open_files(encInfo)==e_failure)
  {
    //print error message
    printf("File not opened!!\n");
    return e_failure;
  }   
  else
  {
    //printf("success message")
    printf("Files are opened Successfully.\n\n");
  }

  if(check_capacity(encInfo)==e_failure)
  {
    printf("Capacity Check failure!!\n");
    return e_failure;
  }
  else
  {
    printf("Checking for capacity Successfully.\n\n");
  }

  if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
  {
    //printf(error message)
    printf("image header not copied!!\n");
    return e_failure;
  }
else
{
  //printf(success message)
  printf("Cpoing image header Successfully.\n\n");
}

  if(encode_magic_string(MAGIC_STRING,encInfo)==e_failure)
  {
    //printf(errrer message)
    printf("Magic string is not encoded!!\n");
    return e_failure;
  }
  else
  {
    //success message
    printf("Encoded Magic string Successfully.\n\n");
  }

  if(encode_file_extn_size(strlen(encInfo->extn_secret_file),encInfo)==e_failure)
  {
    //print error message
    printf("Encoded file extn size failure!!\n");
    return e_failure;
  }
  else
  {
    //print success message
    printf("Encoded file extn size Successfully.\n\n");
  }

  if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_failure)
  {
  //print error message
  printf("Encodeed secret file extn failure!!\n");
  return e_failure;
  }
  else
  {
    //print success message
    printf("Encoded secret file extn Successfully.\n\n");
  }

  if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_failure)
  {
    //print error message
    printf("Encoded secret file size failure!!\n");
    return e_failure;
  }
  else
  {
    //print sueess message
    printf("Encoded secret file size Successfully.\n\n");

  }

  if(encode_secret_file_data(encInfo)==e_failure)
  {
    //print error message
    printf("Encoded secret file data failure!!\n");
    return e_failure;
  }
  else
  {
    //print success message
    printf("Encoded secret file data Successfully.\n\n");
  }

  if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
  {
    //print error message
    printf("copied remaining image data failure!!\n");
    return e_failure;
  }
  else
  {
    //print success message
    printf("Copied remaining image data Successfully.\n");
  }
  return e_success;
}