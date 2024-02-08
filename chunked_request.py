import socket

def send_chunked_request(host, port, path, file_path):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((host, port))

        # Read the file contents
        with open(file_path, 'rb') as f:
            body = f.read()

        # Send the request line
        s.sendall(f"POST {path} HTTP/1.1\r\n".encode())

        # Send the Transfer-Encoding header
        s.sendall(b"Transfer-Encoding: chunked\r\n")
        s.sendall(b"Content-Type: plain/text\r\n")

        # Send an empty line to indicate the start of the headers
        s.sendall(b"\r\n")

        # Send the body in chunks
        for i in range(0, len(body), 10):
            chunk = body[i:i+10]
            chunk_size = hex(len(chunk)).encode()
            s.sendall(b"%s\r\n" % chunk_size)
            s.sendall(chunk)
            s.sendall(b"\r\n")

        # Send the final chunk with size 0
        s.sendall(b"0\r\n\r\n")

        # Receive the response
        response = b""
        while True:
            data = s.recv(10)
            if not data:
                break
            response += data
    print(response)
    return response


send_chunked_request("localhost", 8080, "/test/index.html", "./test/Second/test.txt")