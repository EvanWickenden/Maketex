
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


int main(int argc, char **argv)
{

  if (argc < 2 || argc > 3)
  {
    fprintf(stderr, "usage: ./maketex <filename.tex | filename> [text-editor]\n");
    exit(1);
  }

  int fd, n, out;

  /* __EDITOR__ defined in Makefile */
  char * const editor = (argc == 2) ? __EDITOR__  : argv[3];

  char * const namebuf = malloc(strlen(argv[1]) + 1);
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

  *dot = '\0'; /* trim extension */
  pdflatex_out_buf = malloc(20 + strlen(namebuf));
  if (pdflatex_out_buf == NULL)
    die("malloc() returned NULL");
  n = sprintf(pdflatex_out_buf, "pdflatex-out-%s", namebuf);
  pdflatex_out_buf[n] = '\0';
  *dot = '.'; /* return extension */

  out = open(pdflatex_out_buf, O_CREAT | O_WRONLY | O_TRUNC);
  if (out < 0)
    die("open() out failed");

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
    logger("an event has occured");
    if (event_buf[0].flags & EV_ERROR) break;
    if (!(event_buf[0].flags & EV_EOF))
    {
      int pid, fd[2];

      if (pipe(fd) < 0)
        die("pipe() failed");

      pid = fork();
      if (pid < 0)
        die("fork() failed"); /* more graceful exit */

      if (pid == 0)
      {
        char *script = "/Users/evanwynnwickenden/maketex/maketex.sh";

        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        lseek(out, 0, SEEK_SET); /* overwrite current file-data */
        dup2(out, STDOUT_FILENO);
        dup2(out, STDERR_FILENO);
        *dot = '\0'; /* trim extension */
  
        logger("about to execl");

        execl(script, script, namebuf, (char *) 0);
        die("execl() returned");
      }

      /* parent */
      close(fd[0]);
      write(fd[1], "q\r\n", 4); /* kill pdflatex if error; no effect if no error */
      close(fd[1]);
      waitpid(pid, NULL, 0);
    }
  }


  waitpid(pid, NULL, 0);

  logger("we are terminating\n");

  unlink(pdflatex_out_buf);
  free(pdflatex_out_buf);

  printf("terminating maketex session.\n");

  /* cleanup and exit */
  return 0;
}



void logger(const char *msg)
{
#ifdef __LOG__
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
  fprintf(fp, "%s\n", msg);
  fflush(fp);
#endif
}


void die(const char *msg)
{
  logger(msg);
//  fprintf(stderr, "%s\n", msg);
  exit(1);
}

