import os
import sys


def fork_pipe():
    r, w = os.pipe()            # 必须在fork之前创建管道
    pid = os.fork()
    if pid == -1:               # -1 代表fork失败，直接退出
        sys.exit(-1)
    elif pid == 0:              # 0 代表当前是子进程
        os.close(w)
        r = os.fdopen(r)
        string = r.read()
        print(string)
        print("this is child process {0}, parent pid is {1}".format(os.getpid(), os.getppid()))
    else:                       # 在父进程中，返回的是子进程的pid，是一个大于0的数字。
        os.close(r)
        w = os.fdopen(w, "w")
        w.write("hello from pipe")
        w.close()
        os.wait()
        print("this is parent process {0}, child is {1}.".format(os.getpid(), pid))


FIFO = "/home/guoqi/fifo/tmp"


def fork_fifo():
    try:
        os.mkfifo(FIFO, mode=0o0600)
    except FileExistsError:
        pass

    pid = os.fork()
    if pid == -1:
        sys.exit(-1)
    elif pid == 0:
        f = os.open(FIFO, os.O_RDONLY)
        fp = os.fdopen(f, "r")
        while True:
            line = fp.readline()
            if line:
                print(line, end="")
            else:
                break

        print("\nthis is child process {0}, parent pid is {1}".format(os.getpid(), os.getppid()))
    else:
        f = os.open(FIFO, os.O_SYNC | os.O_CREAT | os.O_RDWR)
        w = os.fdopen(f, "w")
        w.write("hello from fifo 1\n")
        w.write("hello from fifo 2\n")
        w.write("hello from fifo 3\n")
        w.close()
        os.wait()
        print("this is parent process {0}, child is {1}.".format(os.getpid(), pid))


if __name__ == '__main__':
    # fork_pipe()
    fork_fifo()
