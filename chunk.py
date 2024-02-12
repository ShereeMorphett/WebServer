import http.client
import random
import time
import json

def send_chunked_request(host, path, chunks):
    connection = http.client.HTTPConnection(host)
    connection.putrequest('POST', path)
    connection.putheader('Transfer-Encoding', 'chunked')
    connection.putheader('Content-Type', 'text/plain')
    connection.endheaders()

    for chunk in chunks:
		# create fake delay for some chunks
        # time.sleep(random.uniform(0, 3))

        # Convert the chunk to bytes and send its length in hexadecimal
        chunk_bytes = chunk.encode('utf-8')
        connection.send(hex(len(chunk_bytes))[2:].encode('utf-8') + b'\r\n')
        connection.send(chunk_bytes + b'\r\n')

    
    # Send a zero-length chunk to indicate the end of the request
    connection.send(b'0\r\n\r\n')
    
    response = connection.getresponse()
    print('Status:', response.status, response.reason)
    print('Response:', response.read().decode())

# Example usage
host = 'http://localhost:8080/' # Change this to the target host
path = '/uploadas' # Change this to the appropriate path
chunks = [
    "This is the first chunk",
    "This is second",
    "aaaand third",
    "final chunk, over!"
]

send_chunked_request(host, path, chunks)
