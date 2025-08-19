root=$(dirname $PWD)

fuse_mount=$root/build/fuse_mount

# mount filesystem
$root/build/src/nexus-fuse $fuse_mount

echo Listing contents of the mounted filesystem:
ls -l $fuse_mount

echo

echo Printing the contents of a file in the mounted filesystem:
cat $fuse_mount/etc/passwd

# unmount filesystem
fusermount3 -u $fuse_mount
