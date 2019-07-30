# 使用python2运行
import urllib
import urllib2
import random
import io
import requests
import mimetools

def general_boundary():
    prefix = "----------------------------"
    num_range = range(10)
    num_list = list()
    for i in range(24):
        random.shuffle(num_range)
        num_list.append(num_range[0])
    nums = "".join(["{0}".format(i) for i in num_list ])
    boundary = "".join([prefix, nums])
    return boundary

def post_upload():
    # boundary = general_boundary()
    # header = {
    #     'User-Agent':'PostmanRuntime/7.15.0',
    #     'Accept':'*/*',
    #     'Accept-Encoding':'gzip, deflate',
    #     'Cache-Control':'no-cache',
    #     'Content-Type': 'multipart/form-data; boundary={0}'.format(boundary)
    # }
    # content = io.BytesIO()
    # content.write("{0}\r\n".format(boundary))
    # content.write("{0}\r\n".format('Content-Disposition: form-data; name="file"; filename="main.cpp"'))
    # content.write("\r\n")
    # with open("/home/guoqi/ftproot/code/main.cpp", "rb") as fp:
    #     data = fp.read()
    # content.write("{0}\r\n".format(data))
    # content.write("{0}--\r\n".format(boundary))
    
    # _url = "http://127.0.0.1:8080/upload/"
    # req = urllib2.Request(url=_url, data=content.getvalue(), headers=header)
    # f = urllib2.urlopen(req, timeout=5)
    # print f.read()
    fields = ("fields", "ha   ha")
    with open("/home/guoqi/ftproot/code/main.cpp", "rb") as fp:
        data = fp.read()
    files = ("pic", "main.cpp", data)

    content_type, body = encode_multipart_formdata(fields=[fields, ], files=[files, ])

    _url = "http://127.0.0.1:8080/upload/"
    req = urllib2.Request(url=_url, data=body, headers={"Content-Type": content_type})
    f = urllib2.urlopen(req, timeout=5)
    print f.read()

def request_upload():
    url = "http://127.0.0.1:8080/upload/"
    resp = requests.post(url=url, files={
        "file": open("/home/guoqi/ftproot/code/main.cpp", "rb")
    })
    print resp


def encode_multipart_formdata(fields=None, files=None): 
    """ 
    fields is a sequence of (name, value) elements for regular form fields. 
    files is a sequence of (name, filename, value) elements for data to be uploaded as files 
    Return (content_type, body) ready for httplib.HTTP instance 
    这个函数来源于 https://www.cnblogs.com/yizhenfeng168/p/7078550.html
    """ 
    BOUNDARY = general_boundary() 
    CRLF = '\r\n' 
    L = [] 
    for (key, value) in fields: 
        L.append('--' + BOUNDARY) 
        L.append('Content-Disposition: form-data; name="%s"' % key) 
        L.append('') 
        L.append(value) 
    for (key, filename, value) in files: 
        L.append('--' + BOUNDARY) 
        L.append('Content-Disposition: form-data; name="%s"; filename="%s"' % (key, filename)) 
        # L.append('Content-Type: %s' % get_content_type(filename)) 
        L.append('') 
        L.append(value) 
    L.append('--' + BOUNDARY + '--') 
    L.append('') 
    body = CRLF.join(L) 
    content_type = 'multipart/form-data; boundary=%s' % BOUNDARY 
    return content_type, body 

if __name__ == "__main__":
    post_upload()
