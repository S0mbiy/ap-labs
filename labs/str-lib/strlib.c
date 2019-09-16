

int mystrlen(char *str){
	int i;
	for (i = 0; str[i] != '\0'; i++);
    return i;
}

char *mystradd(char *origin, char *addition){
	int i;
	int j = 0;
	for (i = mystrlen(origin); addition[j] != '\0'; i++){
		origin[i] = addition[j++];
	}
	origin[i]='\0';
    return origin;
}

int mystrfind(char *origin, char *substr){
	int i;
	int j = 0;
	for (i = 0; origin[i] != '\0'; i++){
		if(origin[i] == substr[j++]){
			if(j==mystrlen(substr)){
				return 1;
			}
		}else{
			j = 0;
		}
	}
    return 0;
}
