#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 128

// Semaphore operations for synchronization
void semaphore_wait(int semid) {
    struct sembuf sem_op;
    sem_op.sem_num = 0;  // Semaphore number
    sem_op.sem_op = -1;  // Wait (decrement)
    sem_op.sem_flg = 0;
    if (semop(semid, &sem_op, 1) == -1) {
        perror("semop wait failed");
        exit(1);
    }
}

void semaphore_signal(int semid) {
    struct sembuf sem_op;
    sem_op.sem_num = 0;  // Semaphore number
    sem_op.sem_op = 1;   // Signal (increment)
    sem_op.sem_flg = 0;
    if (semop(semid, &sem_op, 1) == -1) {
        perror("semop signal failed");
        exit(1);
    }
}

// Parent process: Reads from pipe and writes to a file
void parent_process(int pipe_fd, int semid, const char *destination_file) {
    FILE *file = fopen(destination_file, "w");
    if (!file) {
        perror("fopen");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Wait for the child to finish writing to the pipe
    semaphore_wait(semid);

    // Parent reads from pipe and writes to file
    while ((bytes_read = read(pipe_fd, buffer, BUFFER_SIZE)) > 0) {
        fwrite(buffer, 1, bytes_read, file);
    }

    fclose(file);
    close(pipe_fd);  // Close the pipe after reading

    // Remove the semaphore
    semctl(semid, 0, IPC_RMID);
}

// Child process: Reads data from a file and writes to the pipe
void child_process(int pipe_fd, int semid, const char *file_name) {
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
    semaphore_signal(semid);
}

int main() {
    const char *source_file = "/usr/include/stdio.h";  // Source file (can be changed)
    const char *destination_file = "output.txt";       // Destination file
    key_t sem_key = 1234;                              // Semaphore key
    int pipe_fd[2];                                    // Pipe file descriptors

    // Create a semaphore set with one semaphore
    int semid = semget(sem_key, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget failed");
        exit(1);
    }

    // Initialize the semaphore to 0 (child process writes first)
    if (semctl(semid, 0, SETVAL, 0) == -1) {
        perror("semctl init failed");
        exit(1);
    }

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
        // Close the write end of the pipe, parent will only read
        close(pipe_fd[1]);

        // Parent reads from pipe and writes to destination file
        parent_process(pipe_fd[0], semid, destination_file);

        // Wait for child to finish
        wait(NULL);
    } else {  // Child process
        // Close the read end of the pipe, child will only write
        close(pipe_fd[0]);

        // Child reads from file and writes to pipe
        child_process(pipe_fd[1], semid, source_file);
    }

    return 0;
}
