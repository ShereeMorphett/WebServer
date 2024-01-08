#!/usr/bin/python3

print("Content-type: text/html\n\n")
import cgi

# Get form data
form = cgi.FieldStorage()

# Retrieve values from the form
name = form.getvalue('name')
lastName = form.getvalue('lastName')
textcontent = form.getvalue('textcontent')  # Add this line to retrieve textcontent

# HTML response
print("<html>")
print("<head>")
print("<img src='/Users/smorphet/Desktop/WebServer/src/cgi-bin/penguinPlaceholder.jpg' alt='Image Description'>")
print("<title>Registration Confirmation</title>")
# Embedding an image
print("<style>")
print("  /* Style for the card container */")
print("  .card {")
print("    width: 300px;")
print("    padding: 20px;")
print("    margin: 20px auto;")
print("    border: 1px solid #ccc;")
print("    border-radius: 5px;")
print("    text-align: center;")
print("  }")
print("</style>")
print("</head>")
print("<body>")
print("<div class='card'>")
print("<h2>Registration Confirmation</h2>")

if name is not None:
    print("<p>Name: " + name + "</p>")
else:
    print("<p>Name not provided</p>")

if lastName is not None:
    print("<p>Last Name: " + lastName + "</p>")
else:
    print("<p>Last Name not provided</p>")

if textcontent is not None:
    print("<p>Text Content: " + textcontent + "</p>")
else:
    print("<p>Text Content not provided</p>")
print("</div>")
print("</body>")
print("</html>")
