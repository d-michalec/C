/*******************************************************************************
/ PROGRAM:     myshell.c
/ AUTHOR:      *** Damian Michalec ***
/ DESCRIPTION: Acts as a simple command line interpreter.  It reads commands
/              from standard input entered from the terminal and executes them. 
/              The shell does not include any provisions for control structures,
/              redirection, background processes, environmental variables,
/              pipes, or other advanced properties of a modern shell. All 
/              commands are implemented internally and do not rely on external
/              system programs.
/
*******************************************************************************/
#include <pwd.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFFER_SIZE          256
#define MAX_PATH_LENGTH      256
#define MAX_FILENAME_LENGTH  256

static char buffer[BUFFER_SIZE] = {0};
static char filename[MAX_FILENAME_LENGTH] = {0};

// Implements various UNIX commands using POSIX system calls
// Each must return an integer value indicating success or failure
int do_cat(const char* filename);
int do_cd(char* dirname);
int do_ls(const char* dirname);
int do_mkdir(const char* dirname);
int do_pwd(void);
int do_rm(const char* filename);
int do_rmdir(const char* dirname);
int do_stat(char* filename);
int execute_command(char* buffer);
  
// Remove extraneous whitespace at the end of a command to avoid parsing problems
void strip_trailing_whitespace(char* string) {
  int i = strnlen(string, BUFFER_SIZE) - 1;
  
  while(isspace(string[i]))
    string[i--] = 0;
}

// Display a command prompt including the current working directory
void display_prompt(void) {
  char current_dir[MAX_PATH_LENGTH];
  
  if (getcwd(current_dir, sizeof(current_dir)) != NULL)
    // Outputs the current working directory in bold green text (\033[32;1m)
    // \033 is the escape sequence for changing text, 32 is green, 1 is bold
    fprintf(stdout, "myshell:\033[32;1m%s\033[0m> ", current_dir);
}

int main(int argc, char** argv) {
  while (true) {
    display_prompt();
    
    // Read a line representing a command to execute from stdin into 
    // a character array
    if (fgets(buffer, BUFFER_SIZE, stdin) != 0) {
      
      // Clean up sloppy user input
      strip_trailing_whitespace(buffer);
      
      //Reset filename buffer after each command execution
      bzero(filename, MAX_FILENAME_LENGTH);     
      
      // As in most shells, "cd" and "exit" are special cases that need
      // to be handled separately
      if ((sscanf(buffer, "cd %s", filename) == 1) ||
	  (!strncmp(buffer, "cd", BUFFER_SIZE))) 
	do_cd(filename);
      else if (!strncmp(buffer, "exit", BUFFER_SIZE)) 
	exit(EXIT_SUCCESS);
      else 
	execute_command(buffer);
    }
  }
  
  return EXIT_SUCCESS;
}

// Changes the current working directory
int do_cd(char* dirname) {
  struct passwd *p = getpwuid(getuid());

  // If no argument, change to current user's home directory
  if (strnlen(dirname, MAX_PATH_LENGTH) == 0)
      strncpy(dirname, p->pw_dir, MAX_PATH_LENGTH);

  // Otherwise, change to directory specified and check for error
  if (chdir(dirname) < 0) {
    fprintf(stderr, "cd: %s\n", strerror(errno));
    return -1;
  }
  
  return 0;
}

// Lists the contents of a directory
int do_ls(const char* dirname) {

  if (dirname == NULL) {
    dirname = getcwd(NULL, 0);

    if (dirname == NULL) {
      perror("getcwd failed");
      exit(1);
    }
  }

  // Use the opendir() function to open the directory
  DIR *dp = opendir(dirname);

  // Check if the directory was opened successfully
  if (dp == NULL) {
    perror("opendir failed");
    exit(1);
  }

  // Use the readdir() function to read the entries in the directory
  struct dirent *entry;
  while ((entry = readdir(dp)) != NULL) {
    // Print the name of the entry
    printf("%s\n", entry->d_name)
  }

  // Close the directory stream
  closedir(dp);

  return 1;
}

// Outputs the contents of a single ordinary file
int do_cat(const char* filename) {
  int fd, ret;
  char buf[1024];
  if ((fd = open(filename, O_RDONLY)) < 0)
    {
        fprintf(stderr, "could not open file %s: No such file or directory.\n", filename);
        exit(1);
    }
    else
    {
        while ((ret = read(fd, buf, sizeof(buf) - 1)) > 0)
        {
            buf[ret] = 0x00;
        }
        close(fd);
    }
    printf("%s", buf);
    
  return 1;
}

// Creates a new directory 
int do_mkdir(const char* dirname) {

  int check;
  check = mkdir(filename, 0777);
 
  // check if directory is created or not
  if (check){
    printf("Unable to create directory\n");
    exit(1);
  }
  return 1;
}

// Removes a directory as long as it is empty
int do_rmdir(const char* dirname) {
  int ret = 0;
  ret = rmdir(dirname);

  if (ret != 0)
    printf("Unable to remove directory %s\n", dirname);

  return 1;
}

// Outputs the current working directory
int do_pwd(void) {
  char *buf;
  buf=(char *)malloc(100*sizeof(char));
  getcwd(buf,100);
  printf("%s \n",buf);
  return 1;
}

// Removes (unlinks) a file
int do_rm(const char* filename) {
  int check;
  check = unlink(filename);

  if(check != 0)
    printf("Unable to remove (unlink) a file");
  
  return 1;
}

// Outputs information about a file
int do_stat(char* filename) {

  struct stat sb;
  if (stat(filename, &sb) == -1) {
    perror("stat");
    return 1;
  }

  printf("File size: %lld bytes\n", (long long) sb.st_size);
  printf("Last accessed: %s", ctime(&sb.st_atimespec.tv_sec));
  printf("Device ID: %ld\n", (long) sb.st_dev);
  printf("Inode number: %ld\n", (long) sb.st_ino);

  return 0;
}

// Executes a shell command with a file or directory name argument (as
// appropriate) passed as a parameter. Checks for invalid commands.
int execute_command(char* buffer)  {
  if (sscanf(buffer, "cat %s", filename) == 1) {
    return do_cat(filename);
  }
  
  if (sscanf(buffer, "stat %s", filename) == 1) {
    return do_stat(filename);
  }
  
  if (sscanf(buffer, "mkdir %s", filename) == 1) {
    return do_mkdir(filename);
  }
  
  if (sscanf(buffer, "rmdir %s", filename) == 1) {
    return do_rmdir(filename);
  }
  
  if (sscanf(buffer, "rm %s", filename) == 1) {
    return do_rm(filename);
  }
 
  if ((sscanf(buffer, "ls %s", filename) == 1) ||
      (!strncmp(buffer, "ls", BUFFER_SIZE))) {
    if (strnlen(filename, BUFFER_SIZE) == 0)
      sprintf(filename, ".");

    return do_ls(filename);
  }

  if (!strncmp(buffer, "pwd", BUFFER_SIZE)) {
    return do_pwd();
  }

  // Invalid command
  if (strnlen(buffer, BUFFER_SIZE) != 0)
    fprintf(stderr, "myshell: %s: No such file or directory\n", buffer);

  return -1;
}
