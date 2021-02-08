#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[])
{
  
    printf("argc: %d", argc);
    //puts(argc);
  for(int i = 0; i < argc;i = i + 1){
  
    printf(" argv[]:");
    printf(" %d - %d", i, argv[i]);
    puts(argv[i]);
  }
  for(int i = 0; i < argc;i = i + 1){
    
    printf(" envp[]:");
    printf(" %d - %d", i, envp[i]);
    puts(envp[i]);
  }


  

  return 0;
}
