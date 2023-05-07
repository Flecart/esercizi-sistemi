#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

long getLenght(FILE *fp) {
  fseek(fp, 0L, SEEK_END);
  long sz = ftell(fp);
  fseek(fp, 0L, SEEK_SET);
  return sz;
}

char *f1_name;
char *f2_name;

int proc(int fract, int current) {
  FILE *f1 = fopen(f1_name, "rb");
  FILE *f2 = fopen(f2_name, "rb");
  if (f1 < 0 || f2 < 0)
    return 1;
  long f_len = getLenght(f1);
  if (f_len != getLenght(f2))
    return 1;
  long from = (f_len / fract) * (current);
  long to = (f_len / fract) * (current + 1);
  if (current + 1 == fract) {
    to = f_len;
  }

  int buf_size = 256;
  char buff1[buf_size], buff2[buf_size];
  fseek(f1, from, SEEK_SET);
  fseek(f2, from, SEEK_SET);

  int done = 0;
  for (long i = from; i < to; i += buf_size) {
    int rsize = buf_size;
    if (to - i < buf_size) {
      rsize = to - i;
    }
    fread(buff1, sizeof(char), rsize, f1);
    fread(buff2, sizeof(char), rsize, f2);
    for (int i = 0; i < rsize; i++) {
      done |= buff1[i] != buff2[i];
    }
    if (done == 1)
      break;
  }

  fclose(f1);
  fclose(f2);
  _exit(done);
  return 0;
}

int main(int argc, char *argv[]) {
  if (!(argc == 3 || argc == 5))
    return 1;

  int f1 = 1;
  int f2 = 2;
  int option_value = 2;
  if (argc == 5) {
    option_value = atoi(argv[2]);
    f1 = 3;
    f2 = 4;
  }

  f1_name = argv[f1];
  f2_name = argv[f2];

  pid_t values[option_value];
  for (int i = 0; i < option_value; i++) {
    pid_t c = fork();
    if (c == 0) {
      proc(option_value, i);
    }
    values[i] = c;
  }

  int faild = 0;
  int i = 0;
  for (; i < option_value; i++) {
    int st;
    pid_t c1 = wait(&st);
    faild |= WEXITSTATUS(st);
    if (faild == 1) {
      break;
    }
  }

  for (int j = 0; faild == 1 && j < option_value; j++) {
    kill(values[i], SIGTERM);
  }

  for (; i < option_value; i++) {
    int s;
    wait(&s);
  }

  if (faild)
    printf("%s %s differ", f1_name, f2_name);
  return 0;
}

