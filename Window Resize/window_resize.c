/*****************************************************************************
/ PROGRAM NAME: assignment6.c
/ AUTHOR:       Damian Michalec
/ COURSE:       CS431 - Operating Systems
/ *****************************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

int main(int argc, char** argv) {
  int  fd;
  struct winsize ws;
  
  fd = open("/dev/ttys000", O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "/dev/ttys000 %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

	ioctl(fd, TIOCGWINSZ, &ws);
  printf("First ioctl call, and values returned:\n");
	printf ("Columns: %d\n", ws.ws_col);
  printf ("Rows: %d\n", ws.ws_row);
  printf ("xpixel: %d\n", ws.ws_xpixel);
  printf ("ypixel: %d\n", ws.ws_ypixel);

  ws.ws_col = 110;
  ws.ws_row = 20;
  ws.ws_xpixel = 10;
  ws.ws_ypixel = 10;

  printf("\nUpdating values to:\n");
  printf ("Columns: %d\n", ws.ws_col);
  printf ("Rows: %d\n", ws.ws_row);
  printf ("xpixel: %d\n", ws.ws_xpixel);
  printf ("ypixel: %d\n", ws.ws_ypixel);

  printf("Calling ioctl again...\n");
  ioctl(fd, TIOCSWINSZ, &ws);


  printf("\nResetting struct winsize...\n");
  ws = (const struct winsize){ 0 };
  printf ("Columns: %d\n", ws.ws_col);
  printf ("Rows: %d\n", ws.ws_row);
  printf ("xpixel: %d\n", ws.ws_xpixel);
  printf ("ypixel: %d\n", ws.ws_ypixel);

  printf("\nCalling ioctl for the third time, and printing values returned...\n");
  ioctl(fd, TIOCGWINSZ, &ws);
  printf ("Columns: %d\n", ws.ws_col);
  printf ("Rows: %d\n", ws.ws_row);
  printf ("xpixel: %d\n", ws.ws_xpixel);
  printf ("ypixel: %d\n", ws.ws_ypixel);
  
  close(fd);
    
  return EXIT_SUCCESS;
}
