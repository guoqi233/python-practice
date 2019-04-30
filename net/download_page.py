from http import client
import sys


SERVER_HOST = "www.baidu.com"


class HttpClient(object):
    def __init__(self, host):
        self.host = host

    def fetch(self, path):
        http_client = client.HTTPConnection(self.host)

        http_client.putrequest('GET', path)
        http_client.putheader('User-Agent', __file__)
        http_client.putheader('host', self.host)
        http_client.putheader('Accept', '/')
        http_client.endheaders()

        try:
            response = http_client.getresponse()
            sys.stdout.write("\n\n")
        except Exception as error:
            print(error)
            return
        else:
            print('Got homepage from {}'.format(self.host))

        return response.read()


if __name__ == '__main__':
    obj = HttpClient(SERVER_HOST)
    print(obj.fetch('/'))

