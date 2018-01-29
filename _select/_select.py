import socket
import select
import sys
import array
import json

SERVER_HOST = "localhost"


class Server(object):
    def __init__(self, port, backlog=5):
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
        self.server.bind((SERVER_HOST, port))
        print 'Server listening to port: %s ...' % port
        self.server.listen(backlog)
        self.outputs = []

    def run(self):
        inputs = [self.server, sys.stdin]
        running = True
        while running:
            try:
                readable, writeable, exceptional = select.select(inputs, self.outputs, [])
            except KeyboardInterrupt:
                running = False
                continue
            for sock in readable:
                if sock is self.server:
                    connect, address = self.server.accept()
                    print address
                    inputs.append(connect)
                    break
                if sock is sys.stdin:
                    result = sock.readline().strip()
                    if result == "exit":
                        running = False
                    else:
                        print "your input is:{}".format(result)
                    break
                else:
                    fp = array.array("u", " "*4096)
                    sock.recv_into(fp, 4096)
                    info = fp.tostring()
                    if not info.strip():
                        inputs.remove(sock)
                        break
                    print info.strip()
                    data = {
                            "code": "Success",
                        }
                    sock.sendall(json.dumps(data))
        else:
            sys.stderr.write("\nserver is down\n")


if __name__ == '__main__':
    server = Server(7457)
    server.run()
