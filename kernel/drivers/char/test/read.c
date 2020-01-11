#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

/*
flags:
O_RDONLY 以只读方式打开文件
O_WRONLY 以只写方式打开文件
O_RDWR 以可读写方式打开文件. 上述三种旗标是互斥的, 也就是不可同时使用, 但可与下列的旗标利用OR(|)运算符组合.
O_CREAT 若欲打开的文件不存在则自动建立该文件.
O_EXCL 如果O_CREAT 也被设置, 此指令会去检查文件是否存在. 文件若不存在则建立该文件, 否则将导致打开文件错误. 此外, 若O_CREAT 与O_EXCL 同时设置, 并且欲打开的文件为符号连接, 则会打开文件失败.
O_NOCTTY 如果欲打开的文件为终端机设备时, 则不会将该终端机当成进程控制终端机.
O_TRUNC 若文件存在并且以可写的方式打开时, 此旗标会令文件长度清为0, 而原来存于该文件的资料也会消失.
O_APPEND 当读写文件时会从文件尾开始移动, 也就是所写入的数据会以附加的方式加入到文件后面.
O_NONBLOCK 以不可阻断的方式打开文件, 也就是无论有无数据读取或等待, 都会立即返回进程之中.
O_NDELAY 同O_NONBLOCK.
O_SYNC 以同步的方式打开文件.
O_NOFOLLOW 如果参数pathname 所指的文件为一符号连接, 则会令打开文件失败.
O_DIRECTORY 如果参数pathname 所指的文件并非为一目录, 则会令打开文件失败

mode:
S_IRWXU00700 权限, 代表该文件所有者具有可读、可写及可执行的权限.
S_IRUSR 或S_IREAD, 00400 权限, 代表该文件所有者具有可读取的权限.
S_IWUSR 或S_IWRITE, 00200 权限, 代表该文件所有者具有可写入的权限.
S_IXUSR 或S_IEXEC, 00100 权限, 代表该文件所有者具有可执行的权限.
S_IRWXG 00070 权限, 代表该文件用户组具有可读、可写及可执行的权限.
S_IRGRP 00040 权限, 代表该文件用户组具有可读的权限.
S_IWGRP 00020 权限, 代表该文件用户组具有可写入的权限.
S_IXGRP 00010 权限, 代表该文件用户组具有可执行的权限.
S_IRWXO 00007 权限, 代表其他用户具有可读、可写及可执行的权限.
S_IROTH 00004 权限, 代表其他用户具有可读的权限
S_IWOTH 00002 权限, 代表其他用户具有可写入的权限.
S_IXOTH 00001 权限, 代表其他用户具有可执行的权限.
*/

int readn(int fd, char *buf, int size)
{
    int count, left, sum = 0;
    char *ptr;

    left = size;
    ptr = buf;
    while(left) {
        count = read(fd, ptr, left);
        if (count < 0) {
            if (errno == EINTR) {
                printf("Interrupted by signal!\n");
                break;
            } else {
                printf("read error!\n");
                return -1;
            }
        } else if (count == 0) {
            printf("File closed\n");
            break;
        }
        ptr += count;
        sum += count;
        left -= count;
    }
    return sum;
}

int main(int argc, char *argv[])
{
    char buf[16] = {0};

    int fd, ret;

    fd = open("/dev/ramchar", O_RDWR);
    if (fd < 0) {
        printf("Open file error, %s\n", strerror(errno));
        return -1;
    }
    ret = readn(fd, buf, 10);
    if (ret < 0) {
        printf("read file error, %s\n", buf);
        return -1;
    }

    printf("Read success:%s length:%d\n", buf, ret);
    close(fd);
    return 0;
}