#include "fuse/fuse_ops.h"

int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &fuse_oper, NULL);
}
