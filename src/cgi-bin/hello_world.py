#!/Users/smorphet/.brew/bin/python3

import os
from urllib.parse import unquote
import sys

print("HTTP/1.1 200 OK")
print("Content-type: text/html\r\n\r\n")
print("<html>")
print("<head>")
print("<title>Hello - First CGI Program</title>")
print("</head>")
print("<body>")
print("<h1>Hello, World!</h1>")
print("</body>")
print("</html>")
