#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 128

// Function for the parent process to read data from the pipe and write it to a file
void parent_process(int read_fd, const char *destination_file) {
    FILE *file = fopen(destination_file, "w");
    if (!file) {
        perror("fopen");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Read from the pipe and write to the destination file
    while ((bytes_read = read(read_fd, buffer, BUFFER_SIZE)) > 0) {
        fwrite(buffer, 1, bytes_read, file);
    }

    fclose(file);
    close(read_fd);  // Close the read end of the pipe after reading
}

// Function for the child process to execute an external program (e.g., "cat") and send file data to the parent
void child_process(int write_fd, const char *file_name) {
    // Open the source file to read from
    FILE *file = fopen(file_name, "r");
    if (!file) {
        perror("fopen");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    // Read from the file and write to the pipe
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (write(write_fd, buffer, bytes_read) == -1) {
            perror("write to pipe");
            exit(1);
        }
    }

    fclose(file);
    close(write_fd);  // Close the write end of the pipe after writing
}

int main() {
    int pipe_fd[2];  // File descriptors for the pipe

    // Create the pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid > 0) {  // Parent process
        // Close the write end of the pipe since the parent only reads
        close(pipe_fd[1]);

        // The parent process will execute a program (e.g., "cat") to read the file
        parent_process(pipe_fd[0], "output.txt");

        // Wait for the child process to finish
        wait(NULL);
    } else {  // Child process
        // Close the read end of the pipe since the child only writes
        close(pipe_fd[0]);

        // The child process will execute an external program using exec()
        // For demonstration, we execute "cat" to read from the source file
        execlp("cat", "cat", "/usr/include/stdio.h", NULL);

        // If exec fails, handle the error
        perror("execlp failed");
        exit(1);
    }

    return 0;
}
