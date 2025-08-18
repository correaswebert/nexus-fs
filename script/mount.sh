root=$(dirname $PWD)

fuse_mount=$root/build/fuse_mount

# mount filesystem
$root/build/src/nexus-fuse $fuse_mount

echo Listing contents of the mounted filesystem:
ls -l $fuse_mount

echo

echo Printing the contents of a file in the mounted filesystem:
files=$(find $fuse_mount -maxdepth 1 -type f)
cat $files

# unmount filesystem
fusermount3 -u $fuse_mount
