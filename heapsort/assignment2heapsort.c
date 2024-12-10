#include "heapsort.h"
#include "customer.h"

#include <stdio.h>
#include <string.h>

#define FR(a, b, c, d) fread(a, b, c, d)
#define FS(a, b, c) fseek(a, b, c)

void swapAB(FILE *read_file, int a, int b);
void heapify_func(FILE *read_file, int i, int n);
int heapsort(const char *filename);

// Testing code
// int main() { heapsort("sample.dat"); }

int heapsort(const char *filename) {

  FILE *read_file = fopen(filename, "r+b");

  if (read_file == NULL) {
    fprintf(stderr, "Error opening file");
    return 0;
  }

  FS(read_file, 0, SEEK_END);
  int fz = ftell(read_file);
  int cus_sz = fz / sizeof(customer);
  FS(read_file, 0, SEEK_SET);

  int i = cus_sz / 2;
  while (i--) {
    heapify_func(read_file, i, cus_sz);
  }
  i = cus_sz;
  while (i--) {
    swapAB(read_file, 0, i);
    heapify_func(read_file, 0, i); // n -> i
  }

  fclose(read_file);
  return 1;
}

void heapify_func(FILE *read_file, int i, int n) {
  int mx = i, ll = 1 + i * 2, rr = 2 + i * 2;

  //       curr
  customer start_customer;
  customer ll_customer;
  customer rr_customer;
  customer mx_customer;

  // pointer to ith node in the file
  FS(read_file, sizeof(customer) * i, SEEK_SET);
  FR(&start_customer, sizeof(customer), 1, read_file);

  // has ll child
  if (ll < n) {
    // pointer to the ll child node in the file
    FS(read_file, sizeof(customer) * ll, SEEK_SET);
    // fread(&ll_customer, sizeof(customer), 1, read_file);
    FR(&ll_customer, sizeof(customer), 1, read_file);

    int cmp = strcmp(ll_customer.name, start_customer.name);

    if ((ll_customer.loyalty > start_customer.loyalty) ||
        (ll_customer.loyalty == start_customer.loyalty && cmp > 0)) {
      mx = ll;
    }
  }

  // has rr child
  if (rr < n) {
    //pointer to the rr 
    FS(read_file, sizeof(customer) * rr, SEEK_SET);
    FR(&rr_customer, sizeof(customer), 1, read_file);

    // pointer to the mx node (i or ll) in the file
    FS(read_file, sizeof(customer) * mx, SEEK_SET);
    FR(&mx_customer, sizeof(customer), 1, read_file);

    int cmp = strcmp(rr_customer.name, mx_customer.name);

    if ((rr_customer.loyalty > mx_customer.loyalty) ||
        (rr_customer.loyalty == mx_customer.loyalty && cmp > 0)) {
      mx = rr;
    }
  }

  if (mx != i) {
    swapAB(read_file, i, mx);
    heapify_func(read_file, mx, n);
  }
}
void swapAB(FILE *read_file, int a, int b) {
  customer c1;
  customer c2;

  FS(read_file, sizeof(customer) * a, SEEK_SET);
  FR(&c1, sizeof(customer), 1, read_file);

  FS(read_file, sizeof(customer) * b, SEEK_SET);
  FR(&c2, sizeof(customer), 1, read_file);

  FS(read_file, sizeof(customer) * a, SEEK_SET);
  fwrite(&c2, sizeof(customer), 1, read_file);

  FS(read_file, sizeof(customer) * b, SEEK_SET);
  fwrite(&c1, sizeof(customer), 1, read_file);
}