
int infof(const char *);
int warnf(const char *);
int errorf(const char *);
int panicf(const char *);
void setLevel(int);
int main() {
    setLevel(0);
    if(infof("my info")<0)
    return 1;
    if(warnf("my warn")<0)
    return 1;
    if(errorf("my error")<0)
    return 1;
    if(panicf("my panic")<0)
    return 1;
    return 0;
}
