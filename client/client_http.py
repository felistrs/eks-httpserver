#!/usr/bin/env python

import http.client


# address = "example.com"
address = "127.0.0.1:1234"

conn = http.client.HTTPConnection(address)
conn.request("GET", "/")
r = conn.getresponse()
print(r.status, r.reason)

data1 = r.read()
print(data1)

# conn.request("GET", "/")
# r = conn.getresponse()
# while not r.closed:
#     l = r.read(200)
#     if len(l) == 0:
#         break
#     print(l)  # 200 bytes
