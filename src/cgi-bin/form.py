#!/usr/bin/python3
 
print("Content-type: text/html\n\n")
import cgi
 
# Get form data
form = cgi.FieldStorage()

for key in form.keys():
    print(key, form.getvalue(key))
# Retrieve values from the form
name = form.getvalue('name')
lastName = form.getvalue('lastName')

 
# HTML response
print("<html>")
print("<head>")
print("<title>Registration Confirmation</title>")
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
# Check if the 'name' field has a value
if name is not None:
    print("<p>Name: " + name + "</p>")
else:
    print("<p>Name not provided</p>")

# Check if the 'lastName' field has a value
if lastName is not None:
    print("<p>Last Name: " + lastName + "</p>")
else:
    print("<p>Last Name not provided</p>")
print("</div>")
print("</body>")
print("</html>")
