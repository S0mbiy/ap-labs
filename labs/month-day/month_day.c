#include <stdio.h>
#include <stdlib.h>

/* month_day function's prototype*/
void month_day(int year, int yearday, int *pmonth, int *pday);
char *month_name(int n);

int main(int argc, char **argv) {
  int pmonth = 0;
  int pday;
  month_day(atoi(argv[1]), atoi(argv[2]), &pmonth, &pday);
  printf("%s %2d, %d\n", month_name(pmonth), pday, atoi(argv[1]));
  return 0;
}

void month_day(int year, int yearday, int *pmonth, int *pday) {
  int leap = (year%4==0 && year%100 != 0)||(year%400==0);
  static char daytab[2][13] = {
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
  };
  int day = 0;
  int month;
  for (month=1; day<yearday; month++){
      if(day+daytab[leap][month] < yearday){
        day+=daytab[leap][month];
      }
      else{
        *pmonth = month;
        *pday = yearday-day;
        break;
      }
  }
}

char *month_name(int n) {

   static char *name[] = {
       "Illegal month",
       "January", "February", "March",
       "April", "May", "June",
       "July", "August", "September",
       "October", "November", "December"
   };

   return (n < 1 || n > 12) ? name[0] : name[n];
}
