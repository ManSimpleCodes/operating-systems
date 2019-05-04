

#include "memory.h"


int main(void)
{

	print(LOG, "STARTING PROGRAM");
	
   initialize();  //init data structures....
	
   FILE *file = stdin;	
   if ( file != NULL )
   {
      char line [ 128 ]; /* or other suitable maximum line size */
      while ( fgets ( line, sizeof line, file ) != NULL ) /* read a line */
      {
        print(LOG, "***********************************");
		print(LOG, cat("Reading Virtual Address: ", line));
 		int page = access(atoi(line));
      }
      fclose ( file );
   }
   else
   {
	  print(ERROR, "ERROR: Could not open input file");  
   }
	

    print(NOTIFY, "***********************************");
	print(NOTIFY, cat("Number of Hits: ", itoa(hits)));
	print(NOTIFY, cat("Number of Faults: ", itoa(faults)));
	
	print(LOG, "ENDING PROGRAM");

	
    return 0;
}