#!/usr/bin/python3
import os
from urllib.parse import unquote
import sys

def get_parameter_value(key, query_string):
    key_value_pairs = query_string.split('&')
    for pair in key_value_pairs:
        if '=' in pair:
            k, v = pair.split('=')
            if k == key:
                return v
    return None

def decode_url_encoded(value):
	# Replace '+' with space and then replace '%XX' with corresponding characters
	return value.replace('+', ' ').replace('%20', ' ').replace('%21', '!').replace('%22', '"').replace('%23', '#').replace('%24', '$').replace('%25', '%').replace('%26', '&').replace('%27', "'").replace('%28', '(').replace('%29', ')').replace('%2A', '*').replace('%2B', '+').replace('%2C', ',').replace('%2D', '-').replace('%2E', '.').replace('%2F', '/').replace('%3A', ':').replace('%3B', ';').replace('%3C', '<').replace('%3D', '=').replace('%3E', '>').replace('%3F', '?').replace('%40', '@').replace('%5B', '[').replace('%5C', '\\').replace('%5D', ']').replace('%5E', '^').replace('%5F', '_').replace('%60', '`').replace('%7B', '{').replace('%7C', '|').replace('%7D', '}').replace('%7E', '~')

def main():
    query_string = os.environ.get("QUERY_STRING")
    decoded_string = decode_url_encoded(query_string)

    # Extract values for "name" and "lastName" from the query string
    text = get_parameter_value("textcontent", decoded_string)
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
	
    # Basic HTML response displaying the extracted values with linked CSS and an image
    html_content = f"<!DOCTYPE html>\n<html>\n<head>\n<title>Hello There!</title>\n<style>{css_content}</style>\n</head>\n<body>\n<h1>{text}</h1>\n</body>\n</html>\r\n\r\n"

    response = f"HTTP/1.1 200 OK\r\nContent-Length:{len(html_content)}\r\nContent-Type: text/html\r\n\r\n{html_content}"

    # Send the response to the browser
    sys.stdout.write(response + html_content)

if __name__ == '__main__':
    main()
