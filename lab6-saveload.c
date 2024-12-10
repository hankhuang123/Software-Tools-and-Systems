#include <stdio.h>
#include "point.h"

ssize_t save_point_array(const char *pathname, size_t n, const point *a)
{
  int suc_writer;
  FILE *openfile = fopen(pathname , "wb");
    //open pathname pointer file and then store at openfile
  if(openfile != NULL){
    suc_writer = fwrite(a, sizeof(point), n ,openfile); //write n stuff from a to c
  }
  else{
    fprintf(stderr, "error cuz nothing inside the file.");
    return -1;
  }
  
  fclose(openfile);
  return suc_writer;
}

ssize_t load_point_array(const char *pathname, size_t n, point *a)
{
  FILE *openfile = fopen(pathname , "rb");
  int suc_reader;
  if(openfile != NULL){
    suc_reader = fread(a, sizeof(point), n ,openfile);
  }
  else{
    fprintf(stderr, "error it is empty in the open file.");
    return -1;
  }
  fclose(openfile);
  return suc_reader;
}


