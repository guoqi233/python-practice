import socket
import select
import sys
import os
import array
import json
from multiprocessing import Process

SERVER_HOST = "localhost"
RECV_MAX_SIZE = 4096


class Server(object):
    def __init__(self, port, backlog=5):
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
        self.server.bind((SERVER_HOST, port))
        print('Server listening to port: {0} ...'.format(port))
        self.server.listen(backlog)
        self.outputs = []

    def start(self):
        pths = []
        for i in range(4):
            p = Process(target=self.run)
            pths.append(p)
            p.start()
        for item in pths:
            item.join()

    def run(self):
        print("pid is {0}".format(os.getpid()))
        inputs = [self.server, ]
        running = True
        while running:
            try:
                readable, writeable, exceptional = select.select(
                    inputs, self.outputs, [])
            except KeyboardInterrupt:
                running = False
                continue
            for sock in readable:
                if sock is self.server:
                    connect, address = self.server.accept()
                    print(address)
                    inputs.append(connect)
                    break
                if sock is sys.stdin:
                    result = sock.readline().strip()
                    if result == "exit":
                        running = False
                    else:
                        print("your input is:{}".format(result))
                    break
                else:
                    recv_data = sock.recv(RECV_MAX_SIZE)
                    if not recv_data:
                        print("leave")
                        inputs.remove(sock)
                        continue
                    print("pid {0} recv: {1}".format(
                        os.getpid(), recv_data.decode("utf-8").strip()))
                    data = {
                        "code": "Success",
                    }
                    send_data = "{0}\n".format(json.dumps(data, indent=4))
                    sock.sendall(send_data.encode())
        else:
            sys.stderr.write("\nserver is down\n")


if __name__ == '__main__':
    server = Server(7457)
    # server.run()
    server.start()
