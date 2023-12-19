#!/usr/bin/python3

from __future__ import print_function
import cgi
import sys
import os

print("HTTP/1.1 200 OK")
print("Content-type: text/html\r\n\r\n")
print("<html>")
print("<head>")
print("<title>Hello - First CGI Program</title>")
print("</head>")
print("<body>")
print("<h1>CGI Form Example</h1>")
print("<form method='post' action='hello_world.py'>") 
print("  <label for='first_name'>First Name:</label>")
print("  <input type='text' id='first_name' name='first_name' required>")
print("  <br>")
print("  <label for='last_name'>Last Name:</label>")
print("  <input type='text' id='last_name' name='last_name' required>")
print("  <br>")
print("  <input type='submit' value='Submit'>")
print("</form>")

# # Embedding an image
# print("<img src='/home/sheree/Desktop/WebServer/src/cgi-bin/penguinPlaceholder.jpg' alt='Image Description'>")

form = cgi.FieldStorage()
print(" PRINTING THE FORM FORMAT")
for key in form.keys():
    print("{}: {}".format(key, form[key].value))

if "first_name" in form and "last_name" in form:
    first_name = form["first_name"].value
    last_name = form["last_name"].value
    print("<p>Submitted First Name: {}</p>".format(first_name))
    print("<p>Submitted Last Name: {}</p>".format(last_name))

print("</body>")
print("</html>")
