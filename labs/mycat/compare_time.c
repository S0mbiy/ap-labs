#include <stdio.h>
#include <string.h>
 #include <time.h>

int main()
{
    clock_t start, end;
     double cpu_time_used, cpu_time_used2;
    char command[50];
   strcpy( command, "cat Makefile" );
   start = clock();
   system(command);
   end = clock();
   cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
   strcpy( command, "./cat2 Makefile" );
   start = clock();
   system(command);
   end = clock();
   cpu_time_used2 = ((double) (end - start)) / CLOCKS_PER_SEC;
   printf("%lf:%lf\n",cpu_time_used, cpu_time_used2);
   return 0;
}
