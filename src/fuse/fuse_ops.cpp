#define _DEFAULT_SOURCE
#define FUSE_USE_VERSION 31

#define UNUSED(x) (void)x

#include <errno.h>
#include <fcntl.h>
#include <fuse3/fuse.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static const char *hello_path = "/hello.txt";
static const char *hello_str = "Hello, FUSE!\n";

static int getattr_callback(const char *path, struct stat *stbuf,
                            struct fuse_file_info *fi) {
    UNUSED(fi);
    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }

    if (strcmp(path, hello_path) == 0) {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(hello_str);
        return 0;
    }

    return -ENOENT;
}

static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler,
                            off_t offset, struct fuse_file_info *fi,
                            enum fuse_readdir_flags flags) {
    UNUSED(offset);
    UNUSED(fi);
    UNUSED(flags);

    if (strcmp(path, "/") != 0) {
        return -ENOENT;
    }

    filler(buf, ".", NULL, 0, (fuse_fill_dir_flags)0);
    filler(buf, "..", NULL, 0, (fuse_fill_dir_flags)0);
    filler(buf, hello_path + 1, NULL, 0, (fuse_fill_dir_flags)0);

    return 0;
}

static int open_callback(const char *path, struct fuse_file_info *fi) {
    if (strcmp(path, hello_path) != 0) {
        return -ENOENT;
    }

    if ((fi->flags & 3) != O_RDONLY) {
        return -EACCES;
    }

    return 0;
}

static int read_callback(const char *path, char *buf, size_t size, off_t offset,
                         struct fuse_file_info *fi) {
    UNUSED(fi);

    if (strcmp(path, hello_path) != 0) {
        return -ENOENT;
    }

    size_t len = strlen(hello_str);
    if (offset < len) {
        if (offset + size > len) {
            size = len - offset;
        }
        memcpy(buf, hello_str + offset, size);
    } else {
        size = 0;
    }

    return size;
}

struct fuse_operations fuse_oper = {
    .getattr = getattr_callback,
    .open = open_callback,
    .read = read_callback,
    .readdir = readdir_callback,
};
