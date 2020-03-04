#include "lab3a.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

struct ext2_super_block super_block; /* global super block strcutre, updated in readSuperblock() */
unsigned int block_size = 0; /* global block size, updated in readSuperblock() */
int img_fd = 0; /* blobal image file descriptor, updated in main() */

/* error message for incorrect arguments */
static char* error_message = "usage\n  FILENAME name of image file to be analyzed\n";


/* safe version of open() */
int m_open(const char *__file, int __oflag) {
    int rc = open(__file, __oflag);
    if (rc == -1) {
        fprintf(stderr, "ERROR failed opening file '%s': %s\n",
            __file,
            strerror(errno)
        );

        exit(1);
    }
    else
        return rc;
}

void m_pread (int __fd, void *__buf, size_t __nbytes, __off_t __offset) {
    long rc = pread(__fd, __buf, __nbytes, __offset);
    if (rc < 0) { /* there's corruption */
        fprintf(stderr, "CORRUPTION DETECTED pread() return negative: %s, exiting...\n", 
            strerror(errno)
        );
        exit(2);
    }
}


int main(int argc, char** argv) {
    /* open image file */
    if (argc != 2) {
        fprintf(stderr, "%s", error_message);
    }
    else {
        img_fd = m_open(argv[1], O_RDONLY);
    }

    readSuperBlock();
    readGroupDescriptor();

    close(img_fd);

    return 0;
}
