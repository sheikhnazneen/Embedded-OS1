#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <string.h>
#include <semaphore.h>
#include <errno.h>

#define BUFFER_SIZE 128

// Semaphore operations for synchronization
void semaphore_wait(sem_t *sem) {
    if (sem_wait(sem) == -1) {
        perror("sem_wait failed");
        exit(1);
    }
}

void semaphore_signal(sem_t *sem) {
    if (sem_post(sem) == -1) {
        perror("sem_post failed");
        exit(1);
    }
}

// Parent process: Reads from pipe and writes to a file
void parent_process(int pipe_fd, sem_t *sem, const char *destination_file) {
    FILE *file = fopen(destination_file, "w");
    if (!file) {
        perror("fopen");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Wait for the child to finish writing to the pipe
    semaphore_wait(sem);

    // Parent reads from pipe and writes to file
    while ((bytes_read = read(pipe_fd, buffer, BUFFER_SIZE)) > 0) {
        fwrite(buffer, 1, bytes_read, file);
    }

    fclose(file);
    close(pipe_fd);  // Close the pipe after reading
}

// Child process: Reads data from a file and writes to the pipe
void child_process(int pipe_fd, sem_t *sem, const char *file_name) {
    FILE *file = fopen(file_name, "r");
    if (!file) {
        perror("fopen");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    // Read from file and write to pipe
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (write(pipe_fd, buffer, bytes_read) == -1) {
            perror("write to pipe");
            exit(1);
        }
    }

    fclose(file);
    close(pipe_fd);  // Close the pipe after writing

    // Signal the parent process that the child is done writing
    semaphore_signal(sem);
}

int main() {
    const char *source_file = "/usr/include/stdio.h";  // Source file (can be changed)
    const char *destination_file = "output.txt";       // Destination file
    const char *sem_name = "/my_semaphore";            // Named semaphore

    int pipe_fd[2];  // Pipe file descriptors

    // Create the pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(1);
    }

    // Open a named semaphore
    sem_t *sem = sem_open(sem_name, O_CREAT, 0666, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid > 0) {  // Parent process
        // Close the write end of the pipe, parent will only read
        close(pipe_fd[1]);

        // Parent reads from pipe and writes to destination file
        parent_process(pipe_fd[0], sem, destination_file);

        // Wait for child to finish
        wait(NULL);
    } else {  // Child process
        // Close the read end of the pipe, child will only write
        close(pipe_fd[0]);

        // Child reads from file and writes to pipe
        child_process(pipe_fd[1], sem, source_file);
    }

    // Close the semaphore
    if (sem_close(sem) == -1) {
        perror("sem_close failed");
        exit(1);
    }

    // Unlink the semaphore
    if (sem_unlink(sem_name) == -1) {
        perror("sem_unlink failed");
        exit(1);
    }

    return 0;
}
