#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#define BUFFER_LENGTH 256
#define NUM_THREADS 2      //defining no.of threads 

static char receive[BUFFER_LENGTH];

void *createThread(void *arg)
{
   char stringToSend[BUFFER_LENGTH];
   long tno;
   int ret, fd;
   tno = (long)arg;
   printf("Running Thread [%d] \n", tno);
   fd = open("/dev/kernelDriver", O_RDWR);             // Open the device with read/write access
   if (fd < 0)
      perror("Failed to open the device...\n");  
   printf("[thread %d]Type in a short string to send to the kernel module:\n", tno);
   scanf("%[^\n]%*c", stringToSend);                // Read in a string (with spaces)
   printf("[thread %d]",tno);
   printf("Writing message to the device [%s].\n", stringToSend);
   ret = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM
   if (ret < 0)
      perror(" Failed to write the message to the device.\n");
   printf("[thread %d] Press ENTER to read back from the device...\n", tno);
   getchar();
   printf("[thread %d] Reading from the device...\n", tno);
   ret = read(fd, receive, BUFFER_LENGTH);   // Read the response from the LKM
   if (ret < 0)
      perror(" Failed to read the message from the device.\n");
   printf("[thread %d]",tno);
   printf(" The received message is: [%s]\n", receive);
   int err = close(fd);
   if (err < 0)
      perror("Error closing file.");
   pthread_exit(NULL);
}

int main()
{
   pthread_t threads[NUM_THREADS];
   int rc;
   long count;
   printf("Starting device test code example...\n");
   /* creates two threads each of which will execute the above function */
   for(count=0;count<NUM_THREADS;count++){
     rc = pthread_create(&threads[count], NULL, createThread, (void *)count);
     if (rc) {
       printf("ERROR : return code from pthread_create() is %d\n", rc);
       exit(-1);
     }
   }
   /* The pthread_join() will suspend execution of the calling thread until the target thread terminates */
   for(count=0;count<NUM_THREADS;count++){
     rc = pthread_join(threads[count], NULL);
     if (rc) {
       printf("ERROR : return code from pthread_join() is %d\n", rc);
       exit(-1);
     }
   printf("End of the program\n");
   return 0;
   pthread_exit(NULL);
   }
}
