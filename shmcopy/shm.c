#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 128

// Structure to store shared memory data and synchronization flag
typedef struct {
    char buffer[BUFFER_SIZE];  // Buffer to hold data
    int done;                  // Flag to indicate child has written data
} SharedData;

void parent_process(int shm_id, const char *destination_file) {
    // Attach to the shared memory segment
    SharedData *shm_data = (SharedData *)shmat(shm_id, NULL, 0);
    if (shm_data == (SharedData *)-1) {
        perror("shmat");
        exit(1);
    }

    FILE *file = fopen(destination_file, "w");
    if (!file) {
        perror("fopen");
        exit(1);
    }

    // Wait for the child to write data into the shared memory
    while (!shm_data->done) {
        sleep(1);  // Sleep for a while before checking again
    }

    // Parent writes data from shared memory to the output file
    fwrite(shm_data->buffer, 1, strlen(shm_data->buffer), file);

    fclose(file);
    shmdt(shm_data);  // Detach from shared memory
}

void child_process(int shm_id, const char *file_name) {
    // Attach to the shared memory segment
    SharedData *shm_data = (SharedData *)shmat(shm_id, NULL, 0);
    if (shm_data == (SharedData *)-1) {
        perror("shmat");
        exit(1);
    }

    FILE *file = fopen(file_name, "r");
    if (!file) {
        perror("fopen");
        exit(1);
    }

    // Read from the file and write to shared memory
    size_t bytes_read = fread(shm_data->buffer, 1, BUFFER_SIZE, file);
    if (bytes_read < 0) {
        perror("fread");
        exit(1);
    }

    fclose(file);

    // Signal the parent that data is ready
    shm_data->done = 1;

    shmdt(shm_data);  // Detach from shared memory
}

int main() {
    const char *source_file = "/usr/include/stdio.h";  // Source file (can be changed)
    const char *destination_file = "output.txt";       // Destination file

    // Create a shared memory segment
    key_t key = ftok("shmfile", 65);  // Generate a unique key
    int shm_id = shmget(key, sizeof(SharedData), 0666 | IPC_CREAT);
    if (shm_id < 0) {
        perror("shmget");
        exit(1);
    }

    // Create the child process
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid > 0) {  // Parent process
        // Initialize the flag as 0 before waiting
        SharedData *shm_data = (SharedData *)shmat(shm_id, NULL, 0);
        shm_data->done = 0;
        shmdt(shm_data);

        // Parent reads from shared memory and writes to destination file
        parent_process(shm_id, destination_file);

        // Wait for child to finish
        wait(NULL);
    } else {  // Child process
        // Initialize the flag as 0 before writing
        SharedData *shm_data = (SharedData *)shmat(shm_id, NULL, 0);
        shm_data->done = 0;
        shmdt(shm_data);

        // Child reads from the source file and writes to shared memory
        child_process(shm_id, source_file);
    }

    // Destroy the shared memory segment
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(1);
    }

    return 0;
}
