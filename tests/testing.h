#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int compare_file_to_buffer(const char *filename,
                           const void *buffer,
                           size_t buffer_len) {

    int result = -1; // -1 for error, 0 for match, 1 for mismatch

    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        int fd = open(filename, O_CREAT | O_WRONLY, 0644);
        size_t written = 0;
        while (written < buffer_len) {
            written += write(fd, buffer+written, buffer_len - written);
        }
        fprintf(stderr, "GoldTesting: No reference found, locking file %s with %zu bytes.\n", filename, written);
        return 0;
    }

    // get file size
    struct stat st;
    if (fstat(fd, &st) == -1) { close(fd); return -1; }
    off_t file_size = st.st_size;

    // 3. Handle size mismatch early
    if (file_size != buffer_len) {
        fprintf(stderr, "Sizes mismatch: File size = %lld, Buffer size = %zu\n",
               (long long)file_size, buffer_len);
        close(fd);
        return 1; // Mismatch
    }

    void *file_map = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_map == MAP_FAILED) { close(fd); return -1; }

    result = (memcmp(file_map, buffer, buffer_len) == 0) ? 0 : 1;

    // for (int i = 0; i < len; i++) {
    //     fprintf(stderr, "lines...\n");

    //     if (*bppm == '\0' || *gold == '\0') {
    //         fprintf(stderr, "One file ended at %d bytes and the other sill has more input.", i);
    //         return 1;
    //     }

    //     if (*bppm != *gold) {
    //         fprintf(stderr, "Mismatch at byte %d: %c != %c\n", i, *bppm, *gold);
    //         return 1;
    //     }
    //     bppm++;
    //     gold++;
    // }

    munmap(file_map, file_size);
    close(fd);

    return result;
}

int compare_to_file(graphi_canvas_t canvas, const char* filename) {
    char *gold;

    size_t count = graphi_write_ppm(canvas, &gold);

    int ret = compare_file_to_buffer(filename, gold, count);

    free(gold);

    return ret;
}

// fprintf(stderr, "Drawing lines...\n");
