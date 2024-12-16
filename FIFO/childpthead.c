/*Implement the following requirements using Operating system (use FIFO)
11. System has two processes P1 and P2
12. P1 opens a file “/usr/include/stdio.h”
13. P1 reads 128 bytes of data from the file and sends it to P2
14. P2 calculates the occurrence of the letter ‘A’ and ‘S’ in the incoming data from
P1 using two threads.
15. P2 prints the number of occurrences of ‘A’ and ‘S’ letters.
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_PATH "/tmp/myfifo"
#define BUFFER_SIZE 128

// Shared structure to store counts of 'A' and 'S'
typedef struct {
    int count_A;
    int count_S;
    char data[BUFFER_SIZE];
} SharedData;

SharedData shared_data;

// Thread function to count occurrences of 'A'
void* count_A(void* arg) {
    int count_A = 0;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (shared_data.data[i] == 'A') {
            count_A++;
        }
    }
    shared_data.count_A = count_A;
    return NULL;
}

// Thread function to count occurrences of 'S'
void* count_S(void* arg) {
    int count_S = 0;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (shared_data.data[i] == 'S') {
            count_S++;
        }
    }
    shared_data.count_S = count_S;
    return NULL;
}

int main() {
    // Step 1: Open FIFO for reading
    int fifo_fd = open(FIFO_PATH, O_RDONLY);
    if (fifo_fd == -1) {
        perror("open FIFO");
        exit(1);
    }

    // Step 2: Read the data from FIFO
    ssize_t bytesRead = read(fifo_fd, shared_data.data, BUFFER_SIZE);
    if (bytesRead == -1) {
        perror("read FIFO");
        exit(1);
    }

    close(fifo_fd);

    printf("P2: Data received from FIFO.\n");

    // Step 3: Create two threads to count 'A' and 'S'
    pthread_t thread_A, thread_S;
    pthread_create(&thread_A, NULL, count_A, NULL);
    pthread_create(&thread_S, NULL, count_S, NULL);

    // Step 4: Wait for both threads to finish
    pthread_join(thread_A, NULL);
    pthread_join(thread_S, NULL);

    // Step 5: Print the results
    printf("P2: Occurrences of 'A': %d\n", shared_data.count_A);
    printf("P2: Occurrences of 'S': %d\n", shared_data.count_S);

    return 0;
}
