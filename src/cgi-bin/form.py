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

    # Basic HTML response displaying the extracted values
    response = f"Content-Type: text/html\r\n\r\n<!DOCTYPE html>\n<html>\n<head>\n<title>Query String</title>\n</head>\n<body>\n<p>Name: {name}</p>\n<p>Last Name: {last_name}</p>\n</body>\n</html>"

    # Send the response to the browser
    sys.stdout.write(response)

if __name__ == '__main__':
    main()
