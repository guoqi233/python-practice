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


def fork_fifo():
    pass


if __name__ == '__main__':
    fork_pipe()
