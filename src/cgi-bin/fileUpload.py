#!/usr/bin/python3

print("Content-type: text/html\n\n")
import cgi
import os

form = cgi.FieldStorage()

# Check if the file field is present in the form
if "file" in form:
    file_item = form["file"]

    # Check if the file was successfully uploaded
    if file_item.filename:
 
        upload_dir = "src/cgi-bin/uploads"
        
        # Save the file to the server
        file_path = os.path.join(upload_dir, os.path.basename(file_item.filename))
        with open(file_path, "wb") as file:
            file.write(file_item.file.read())

        print(f"File '{file_item.filename}' uploaded successfully.")
    else:
        print("No file was uploaded.")
else:
    print("File field not found in the form.")
