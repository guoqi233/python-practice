import sys

from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer

DEFAULT_HOST = "127.0.0.1"
DEFAULT_PORT = "8080"


class RequestHandler(BaseHTTPRequestHandler):

    def do_GET(self):
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()

        self.wfile.write("Hello from server")
        return


class CustomerHTTPServer(HTTPServer):
    def __init__(self, host, port):
        HTTPServer.__init__(self, (host, port), RequestHandler)


def run_server(port):
    server = CustomerHTTPServer(DEFAULT_HOST, port)
    try:
        print "Customer Http Server Running on port {0}".format(port)
        server.serve_forever()
    except Exception as error:
        print error
    except KeyboardInterrupt:
        print "Server is Down"
        server.socket.close()


if __name__ == '__main__':
    run_server(8080)


