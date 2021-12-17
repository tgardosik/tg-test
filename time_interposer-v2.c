//Example of a library interposer: interpose on gettimeofday(). 
// Build and use this interposer as following:
//
// gcc -Wall -O2 -fpic -shared -ldl -m32 -o time_interposer.so time_interposer-v2.c
// setenv LD_PRELOAD $cwd/time_interposer.so 
// run the app 
// unsetenv LD_PRELOAD 
//
// 30.Dec, 2016
// 	Correct seconds in year calculation for Leap Year
// 	Add 32-bit stack compile option
//

#include <sys/time.h> 
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

// this routine returns the current time, but always the same time
// just make it Sept 15th, 2000

int gettimeofday(struct timeval *tv, struct timezone *tz){

   printf ("Entering GetTimeOfDay_Interposer\n");

// Sept 15th, 2000
   tv->tv_sec = 969045217;
   tv->tv_usec = 0;

   return(0);
} 


// This routine normalises time to the year 2000
// All times are returned as current time & day, but the
// year is always 2000
//	

time_t time(time_t *adj_time) {
  void 		*handle;
  time_t	(*orig_time)(time_t);
  time_t 	curr_time, time_return;
  long 		secs_in_year = (long) (365*24*60*60);
  long      current_year;
  long      delta_year;
  char 		*error;

//` Open the library that contains time
  handle = dlopen("/lib/libc.so.6", RTLD_LAZY);
  if (!handle) {
    fputs (dlerror(), stderr);
    exit(EXIT_FAILURE);
  }
//get a pointer to the original function
  orig_time = dlsym(handle, "time");
  if ((error = dlerror()) != NULL){
    fprintf (stderr, "%s\n", error);
    exit(EXIT_FAILURE);
  }

  curr_time = orig_time(NULL);
  current_year = (curr_time / secs_in_year) + 1970;
  delta_year = current_year - 2000;

// Normalise time to year 2000 

  time_return = curr_time - (delta_year * secs_in_year);

   if(adj_time!= NULL ) {
      *adj_time = time_return;
   }
   return time_return;
}

