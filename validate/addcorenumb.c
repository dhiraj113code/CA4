#include <stdio.h>
#include <stdlib.h>

//Purpose of this hopenfully tiny program is to add core number in front of a file

main()
{
   int read_trace_element(FILE *inFile, unsigned *access_type, unsigned *addr);

   FILE *input, *output;
   input = fopen("tex.trace", "r");
   output = fopen("texMC.trace", "w");

   int result;
   char c;
   int coreId = 7;
   unsigned access_type, addr;
   
   while(read_trace_element(input, &access_type, &addr))
   {
      if(access_type == 2) access_type = 0;
      fprintf(output, "%d %d %x\n", coreId, access_type, addr);
   }
   
  fclose(input);
  fclose(output);
}


/************************************************************/
int read_trace_element(FILE *inFile, unsigned *access_type, unsigned *addr)
{
  int result;
  char c;

  result = fscanf(inFile, "%u %x%c", access_type, addr, &c);
  while (c != '\n') {
    result = fscanf(inFile, "%c", &c);
    if (result == EOF)
      break;
  }
  if (result != EOF)
    return(1);
  else
    return(0);
}
