#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
  pid_t pid, ppid;
  int a = 0;

    
  for(int i = 0; i < 6;i = i + 1){
  
    fork();
  //
  // If the new process is created successfully,
  // two processes start working in pseudo-parallel from this point: the old and the new one.
  //
  // Before the following expression is executed,
  // the value of the variable a in both processes is 0.
  //
    a = a+1;
  }
  pid  = getpid();
  ppid = getppid();

  printf("My pid = %d,\tmy ppid = %d,\tresult = %d\n", (int)pid, (int)ppid, a);

  return 0;
}
