
#include "utils.h"
#include "memory.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int ERRORflag = FALSE;
char buffer[16];
int trace = FALSE;

void reverse(char s[]);


void print(int display, char const* str)
{
	if(display == ERROR)
		perror(str);
	else if (trace == TRUE || display > 0)
		printf("%s\n",str);
}

//Given two strings, concatenate them together
char* cat(char* s1,char* s2)
{
	char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    
	//in real code you would check for errors in malloc here
    
	strcpy(result, s1);
    strcat(result, s2);
    return result;
}


/* reverse:  reverse string s in place - used by itoa */
 void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }

 
//Convert an integer to a string
char* itoa(int n)
 {
	
     int i, sign;
 
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         buffer[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         buffer[i++] = '-';
     buffer[i] = '\0';
     reverse(buffer);
	 return buffer;
 }


