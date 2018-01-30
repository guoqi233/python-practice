import httplib
import sys


SERVER_HOST = "www.baidu.com"


class HttpClient(object):
    def __init__(self, host):
        self.host = host

    def fetch(self, path):
        http = httplib.HTTP(self.host)

        http.putrequest('GET', path)
        http.putheader('User-Agent', __file__)
        http.putheader('host', self.host)
        http.putheader('Accept', '/')
        http.endheaders()

        try:
            code, msg, headers = http.getreply()
            print code, msg, headers
            sys.stdout.write("\n\n\n\n\n\n")
        except Exception as error:
            print error
        else:
            print 'Got homepage from {}'.format(self.host)

        fp = http.getfile()
        return fp.read()


if __name__ == '__main__':
    obj = HttpClient(SERVER_HOST)
    print obj.fetch('/')

