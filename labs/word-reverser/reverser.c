#include <stdio.h>
#define LEN 100000
int print(int p, int list[]){

  for(int i = LEN-p; i < LEN; i++){
    putchar(list[i]);
  }
  printf("\n");
  printf("\n");
  return 0;
}

int main(){
  int list[LEN];
  int c;
  int p = 0;
  while ((c = getchar()) != EOF){
    if(c == 10){
      print(p, list);
      p = 0;
    }
    else{
      p++;
      list[LEN-p]=c;
    }
  }
  return 0;
}
