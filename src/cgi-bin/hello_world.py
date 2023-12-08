#!/usr/bin/env python


#this is a test case, from an example 

from __future__ import print_function
import cgi
import sys
import os

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get data from fields
first_name = form.getvalue('first_name')
last_name = form.getvalue('last_name')
# query_string = os.environ.get('QUERY_STRING', 'No QUERY_STRING provided')
# print(os.environ["QUERY_STRING"], file=sys.stderr)
# print("HTTP/1.1 200 OK")
print("Content-type: text/html\r\n\r\n")
print("<html>")
print("<head>")
print("<title>Hello - Second CGI Program</title>")
print("<html>")
print("<head>")
print("<body>")
print("<h2>Hello %s %s</h2>" % (first_name, last_name))
print("</body>")
print("</html>")