#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_PATH "/tmp/myfifo"
#define BUFFER_SIZE 128

int main() {
    // Step 1: Create FIFO if it doesn't exist
    if (mkfifo(FIFO_PATH, 0666) == -1) {
        perror("mkfifo");
        exit(1);
    }

    // Step 2: Open file and read 128 bytes
    FILE *file = fopen("/usr/include/stdio.h", "r");
    if (!file) {
        perror("fopen");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    size_t bytesRead = fread(buffer, 1, BUFFER_SIZE, file);
    fclose(file);

    if (bytesRead == 0) {
        perror("fread");
        exit(1);
    }

    // Step 3: Open FIFO for writing
    int fifo_fd = open(FIFO_PATH, O_WRONLY);
    if (fifo_fd == -1) {
        perror("open FIFO");
        exit(1);
    }

    // Step 4: Write the data to the FIFO
    write(fifo_fd, buffer, bytesRead);
    close(fifo_fd);

    printf("P1: Data written to FIFO.\n");

    return 0;
}
