#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 128

void child_process(int pipe_fd, const char *source_file) {
    // Open the source file for reading
    FILE *file = fopen(source_file, "r");
    if (!file) {
        perror("fopen");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    // Read data from the source file and write to the pipe
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (write(pipe_fd, buffer, bytes_read) != bytes_read) {
            perror("write to pipe");
            exit(1);
        }
    }

    fclose(file);
    close(pipe_fd);  // Close the write end of the pipe
}

void parent_process(int pipe_fd, const char *destination_file) {
    // Open the destination file for writing
    FILE *file = fopen(destination_file, "w");
    if (!file) {
        perror("fopen");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Read data from the pipe and write to the destination file
    while ((bytes_read = read(pipe_fd, buffer, BUFFER_SIZE)) > 0) {
        if (fwrite(buffer, 1, bytes_read, file) != bytes_read) {
            perror("fwrite");
            exit(1);
        }
    }

    fclose(file);
    close(pipe_fd);  // Close the read end of the pipe
}

int main() {
    const char *source_file = "source.txt";  // Source file (can be changed)
    const char *destination_file = "destination.txt";  // Destination file

    // Create a pipe
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(1);
    }

    // Create the child process using fork()
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid > 0) {  // Parent process
        // Close the write end of the pipe since we only need to read
        close(pipe_fd[1]);

        // Parent reads from the pipe and writes to the destination file
        parent_process(pipe_fd[0], destination_file);

        // Wait for the child process to finish
        wait(NULL);
    } else {  // Child process
        // Close the read end of the pipe since we only need to write
        close(pipe_fd[0]);

        // Child reads from the source file and writes to the pipe
        child_process(pipe_fd[1], source_file);
    }

    return 0;
}
