
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

#define BUFFER_LENGTH 4                ///< The buffer length
static char receive[BUFFER_LENGTH];     ///< The receive buffer from the LKM

int main(){
   int ret, fd,i;
   char stringToSend[BUFFER_LENGTH];
   do{
         printf("Type in a four bit string to send to the led module and -1 to exit:\n");
         scanf("%[^\n]%*c", stringToSend);
         if(stringToSend[0]=='-'&&stringToSend[1]=='1'){
          continue;
         }
         if(strlen(stringToSend)>BUFFER_LENGTH||strlen(stringToSend)<BUFFER_LENGTH||(stringToSend[0]!='0'&&stringToSend[0]!='1')||(stringToSend[1]!='0'&&stringToSend[1]!='1')||
                (stringToSend[2]!='0'&&stringToSend[2]!='1')||(stringToSend[3]!='0'&&stringToSend[3]!='1')){// //if string is anything other than '0' or '1' (ie.,indicating wrong string)   
                     printf("wrong string \n");
                    continue;
          }else{
                 fd = open("/dev/kernelDriver", O_RDWR);             // Open the device with read/write access
                 if (fd < 0)
                        perror("Failed to open the device...");
                // printf("Writing message to the device [%s].\n", stringToSend);

                 ret = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM
                 if (ret < 0)
                        perror("Failed to write the message to the device.");
                 int err = close(fd);                                                               // Close the device
                 if (err < 0)
                        perror("Error closing file.");

                  }
   }while(strcmp(stringToSend,"-1")!=0);   //exit when string '-1' is entered
   printf("End of the program\n");
   return 0;
}


