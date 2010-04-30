# This script is supposed to be run inside a new namespace using the newns binary.
# Example invocation: /media/internal/newns mount_xterm.sh
#


JAIL_DIR=/media/internal/jail
XLIB_DIR=/media/cryptofs/apps/usr/palm/applications/org.webosinternals.xlib
XTERM_BIN=/media/cryptofs/apps/usr/palm/applications/org.webosinternals.xterm-bin/xterm

mount -o remount,rw /
mkdir -p /usr/local/lib
mkdir -p /usr/local/bin
touch /usr/local/bin/xterm
mount -o remount,ro /

# Get proc going in the new namespace
mount -t proc /proc

mkdir -p $JAIL_DIR
mount --bind $JAIL_DIR $JAIL_DIR

mkdir -p $JAIL_DIR/bin
mkdir -p $JAIL_DIR/usr
mkdir -p $JAIL_DIR/lib
mkdir -p $JAIL_DIR/sbin
mkdir -p $JAIL_DIR/sys
mkdir -p $JAIL_DIR/etc
mkdir -p $JAIL_DIR/home/root
mkdir -p $JAIL_DIR/dev
mkdir -p $JAIL_DIR/mnt
mkdir -p $JAIL_DIR/proc
mkdir -p $JAIL_DIR/tmp

mount --bind /bin $JAIL_DIR/bin
mount --bind /usr $JAIL_DIR/usr
mount --bind /lib $JAIL_DIR/lib
mount --bind /sbin $JAIL_DIR/sbin
mount --bind /sys $JAIL_DIR/sys
mount --bind /dev $JAIL_DIR/dev
mount --bind /tmp $JAIL_DIR/dev/mapper


mount --bind $XLIB_DIR $JAIL_DIR/usr/local/lib
mount --bind $XTERM_BIN $JAIL_DIR/usr/local/bin/xterm
pivot_root $JAIL_DIR $JAIL_DIR/mnt
cd /
umount -l /mnt

echo "proc /proc proc defaults 0 0" > /etc/fstab
echo "devpts /dev/pts devpts defaults 0 0" >> /etc/fstab
echo "tmpfs /tmp tmpfs defaults 0 0" >> /etc/fstab
mount -t proc /proc
mount -t devpts /dev/pts
mount -t tmpfs /tmp

hostname "container1"
export DISPLAY=:0.0
/usr/local/bin/xterm -geometry 52x36+0+0
