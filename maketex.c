
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/event.h>


void die(const char *msg);
void logger(const char *msg);


#ifdef __LOG__
#define LOGGER(x)   logger(x)
#else
#define LOGGER(x)   ;
#endif


int main(int argc, char **argv)
{

  int fd, n, out;

  /* __EDITOR__ provided in Makefile */
#ifdef __EDITOR__
  if (argc < 2 || argc > 3)
  {
    fprintf(stderr, "usage: ./maketex <filename.tex | filename> [text-editor]\n");
    exit(1);
  }
  char * const editor = (argc == 2) ? __EDITOR__  : argv[2];
#else
  if (argc != 3)
  {
      fprintf(stderr, "No editor provided in build, must provide command line argument;\n"
              "usage: ./maketex <filename.tex | filename> <text-editor>\n");
      exit(1);
  }
  char * const editor = argv[2];
#endif


  char * const namebuf = malloc(strlen(argv[1]) + 5);
  if (namebuf == NULL)
    die("malloc() returned NULL");

  char *dot = namebuf, *pdflatex_out_buf;

  {
    int i = 0;
    while ((*dot++ = *(argv[1] +i++)) != '\0'); /* strcpy */
    dot = strstr(namebuf, ".tex"); 
    if (dot == NULL) 
    {
      char *tex = ".tex";
      int j = 0;
      dot = namebuf + i - 1;
      while ((*(dot + j++) = *tex++) != '\0'); /* copy over extension */
    }
  }
  /* namebuf is always filename.tex; dot is pointer to dot in ".tex" */


  /* open beforehand to ensure permissions */
  fd = open(namebuf, O_CREAT | O_EVTONLY, 0644); 
  if (fd < 0)
    die("open() failed");
  unlink(namebuf); /* allow vim user to set permanence of this file */


  *dot = '\0'; /* trim extension */
  pdflatex_out_buf = malloc(20 + strlen(namebuf));
  if (pdflatex_out_buf == NULL)
    die("malloc() returned NULL");
  n = sprintf(pdflatex_out_buf, "pdflatex-out-%s", namebuf);
  pdflatex_out_buf[n] = '\0';
  *dot = '.'; /* return extension */


  out = open(pdflatex_out_buf, O_CREAT | O_WRONLY | O_TRUNC, 0444);
  if (out < 0)
    die("open() out failed");
  unlink(pdflatex_out_buf); /* destroy file even in case of crash */


  pid_t pid = fork();
  if (pid < 0)
    die("fork() failed");
  if (pid == 0) 
  {
    close(fd);
    close(out);
    execlp(editor, editor, namebuf, pdflatex_out_buf, (char *) 0);
    die("execl() returned");
  }

  
  /* parent */
  int kq, err;
  struct kevent event[2], event_buf[2]; 

  EV_SET(event, pid, EVFILT_PROC, EV_ADD | EV_CLEAR, NOTE_EXIT, 0, NULL);
  EV_SET(event + 1, fd, EVFILT_VNODE, EV_ADD | EV_CLEAR, NOTE_WRITE, 0, NULL);

  kq = kqueue();
  if (kq < 0)
    die("kqueue() failed");

  while ((err = kevent(kq, event, 2, event_buf, 2, NULL)) > 0)
  {
    LOGGER("an event has occured");
    if (event_buf[0].flags & EV_ERROR) break;
    if (!(event_buf[0].flags & EV_EOF))
    {
      int pid;

      pid = fork();
      if (pid < 0)
        die("fork() failed"); /* more graceful exit */

      if (pid == 0)
      {
        char *script = "/Users/evanwynnwickenden/maketex/maketex.sh";

        lseek(out, 0, SEEK_SET); /* overwrite current file-data */
        dup2(out, STDOUT_FILENO);
        dup2(out, STDERR_FILENO);
        *dot = '\0'; /* trim extension */
  
        LOGGER("about to execl");

        execl(script, script, namebuf, (char *) 0);
        die("execl() returned");
      }

      waitpid(pid, NULL, 0);
    }
  }


  waitpid(pid, NULL, 0);

  LOGGER("we are terminating\n");

  free(pdflatex_out_buf);

  printf("terminating maketex session.\n");

  return 0;
}



void logger(const char *msg)
{
  static int init = 0;
  static FILE *fp;
  if (init == 0)
  {
    init = 1;
    fp = fopen("log-maketex.log", "a+");

    if (fp == NULL)
      fprintf(stderr, "logger could not open");

    fprintf(fp, 
        "--------------------------------\n"
        "new log session\n");
  }
  fflush(fp);
}


void die(const char *msg)
{
  LOGGER(msg);
//  fprintf(stderr, "%s\n", msg);
  exit(1);
}

