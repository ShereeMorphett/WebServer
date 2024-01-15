#!/Users/smorphet/.brew/bin/python3
import os
from urllib.parse import unquote
import sys

def get_parameter_value(key, query_string):
    key_value_pairs = query_string.split('&')
    for pair in key_value_pairs:
        k, v = pair.split('=')
        if k == key:
            return v

def main():
    query_string = os.environ.get("QUERY_STRING")
    decoded_string = unquote(query_string)

    # Extract values for "name" and "lastName" from the query string
    name = get_parameter_value("name", decoded_string)
    last_name = get_parameter_value("lastName", decoded_string)

    # Detailed CSS content in an external file
    css_content = """
    body {
        font-family: 'Arial', sans-serif;
        background-color: #f2f2f2;
    }
	h1 {
		color: rgb(0, 128, 128);
		display: inline-block; 
		margin-right: 10px; 
	}
    p {
        color: black;
        font-size: 40px;
    }
    """
	
    # Image source URL or file path
    # image_src = "/Users/smorphet/Desktop/WebServer/src/cgi-bin/partPenguin.jpeg"

    # Basic HTML response displaying the extracted values with linked CSS and an image
    html_content = f"<!DOCTYPE html>\n<html>\n<head>\n<title>Hello There!</title>\n<style>{css_content}</style>\n</head>\n<body>\n<h1>Welcome to our site {name} {last_name}</h1>\n</body>\n</html>\r\n\r\n"

    response = f"HTTP/1.1 200 OK\r\nContent-Length:{len(html_content)}\r\nContent-Type: text/html\r\n\r\n{html_content}"

    # Send the response to the browser
    sys.stdout.write(response + html_content)

if __name__ == '__main__':
    main()
