#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 128

// Read-Write Lock for synchronization
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

// Parent process: Reads from pipe and writes to a file
void parent_process(int pipe_fd, const char *destination_file) {
    FILE *file = fopen(destination_file, "w");
    if (!file) {
        perror("fopen");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Wait for the child to finish writing to the pipe
    pthread_rwlock_rdlock(&rwlock);  // Lock for reading

    // Parent reads from pipe and writes to file
    while ((bytes_read = read(pipe_fd, buffer, BUFFER_SIZE)) > 0) {
        fwrite(buffer, 1, bytes_read, file);
    }

    fclose(file);
    close(pipe_fd);  // Close the pipe after reading
}

// Child process: Reads data from a file and writes to the pipe
void child_process(int pipe_fd, const char *file_name) {
    FILE *file = fopen(file_name, "r");
    if (!file) {
        perror("fopen");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    // Child reads from file and writes to pipe
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (write(pipe_fd, buffer, bytes_read) == -1) {
            perror("write to pipe");
            exit(1);
        }
    }

    fclose(file);
    close(pipe_fd);  // Close the pipe after writing

    // Signal the parent process that the child is done writing
    pthread_rwlock_wrlock(&rwlock);  // Lock for writing (allow parent to read after)
    pthread_rwlock_unlock(&rwlock);  // Unlock after writing
}

int main() {
    const char *source_file = "/usr/include/stdio.h";  // Source file (can be changed)
    const char *destination_file = "output.txt";       // Destination file
    int pipe_fd[2];                                    // Pipe file descriptors

    // Create the pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(1);
    }

    // Create the child process
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid > 0) {  // Parent process
        // Close the write end of the pipe, parent will only read
        close(pipe_fd[1]);

        // Wait for the child to finish writing to the pipe
        parent_process(pipe_fd[0], destination_file);

        // Wait for child process to finish
        wait(NULL);
    } else {  // Child process
        // Close the read end of the pipe, child will only write
        close(pipe_fd[0]);

        // Child reads from file and writes to pipe
        child_process(pipe_fd[1], source_file);
    }

    // Destroy the read-write lock
    pthread_rwlock_destroy(&rwlock);

    return 0;
}
