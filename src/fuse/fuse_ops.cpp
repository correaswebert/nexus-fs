#define _DEFAULT_SOURCE
#define FUSE_USE_VERSION 31

#define UNUSED(x) (void)x

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse3/fuse.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int getattr_callback(const char *path, struct stat *stbuf,
                            struct fuse_file_info *fi) {
    UNUSED(fi);

    int res = lstat(path, stbuf);
    if (res == -1) {
        return -errno;
    }

    return 0;
}

static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler,
                            off_t offset, struct fuse_file_info *fi,
                            enum fuse_readdir_flags flags) {
    UNUSED(offset);
    UNUSED(fi);
    UNUSED(flags);

    DIR *dp = opendir(path);
    if (dp == NULL) {
        return -errno;
    }

    struct dirent *de;
    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0, (fuse_fill_dir_flags)0)) {
            break;
        }
    }

    closedir(dp);
    return 0;
}

static int open_callback(const char *path, struct fuse_file_info *fi) {
    int res = open(path, fi->flags);
    if (res == -1) {
        return -errno;
    }

    fi->fh = res;
    return 0;
}

static int read_callback(const char *path, char *buf, size_t size, off_t offset,
                         struct fuse_file_info *fi) {
    UNUSED(path);

    int res = pread(fi->fh, buf, size, offset);
    if (res == -1) {
        return -errno;
    }
    return res;
}

static int write_callback(const char *path, const char *buf, size_t size,
                          off_t offset, struct fuse_file_info *fi) {
    UNUSED(path);

    int res = pwrite(fi->fh, buf, size, offset);
    if (res == -1) {
        return -errno;
    }

    return res;
}

static int release_callback(const char *path, struct fuse_file_info *fi) {
    UNUSED(path);

    close(fi->fh);
    return 0;
}

static int create_callback(const char *path, mode_t mode,
                           struct fuse_file_info *fi) {
    int res = open(path, fi->flags | O_CREAT | O_TRUNC, mode);
    if (res == -1) {
        return -errno;
    }

    fi->fh = res;
    return 0;
}

static int mkdir_callback(const char *path, mode_t mode) {
    int res = mkdir(path, mode);
    if (res == -1) {
        return -errno;
    }

    return 0;
}

static int rmdir_callback(const char *path) {
    int res = rmdir(path);
    if (res == -1) {
        return -errno;
    }

    return 0;
}

static int unlink_callback(const char *path) {
    int res = unlink(path);
    if (res == -1) {
        return -errno;
    }

    return 0;
}

static int rename_callback(const char *from, const char *to,
                           unsigned int flags) {
    if (flags) {
        return -EINVAL;
    }

    int res = rename(from, to);
    if (res == -1) {
        return -errno;
    }

    return 0;
}

static int truncate_callback(const char *path, off_t size,
                             struct fuse_file_info *fi) {
    int res;
    if (fi != NULL) {
        res = ftruncate(fi->fh, size);
    } else {
        res = truncate(path, size);
    }

    if (res == -1) {
        return -errno;
    }

    return 0;
}

static int fsync_callback(const char *path, int isdatasync,
                          struct fuse_file_info *fi) {
    UNUSED(path);

    int res;
    if (isdatasync) {
        res = fdatasync(fi->fh);
    } else {
        res = fsync(fi->fh);
    }

    if (res == -1) {
        return -errno;
    }

    return 0;
}

struct fuse_operations fuse_oper = {
    .getattr = getattr_callback,
    .mkdir = mkdir_callback,
    .unlink = unlink_callback,
    .rmdir = rmdir_callback,
    .rename = rename_callback,
    .truncate = truncate_callback,
    .open = open_callback,
    .read = read_callback,
    .write = write_callback,
    .release = release_callback,
    .fsync = fsync_callback,
    .readdir = readdir_callback,
    .create = create_callback,
};
