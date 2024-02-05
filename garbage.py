# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    garbage.py                                         :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jmykkane <jmykkane@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/05 14:38:14 by jmykkane          #+#    #+#              #
#    Updated: 2024/02/05 14:50:20 by jmykkane         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

import socket
import os

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

host = 'localhost'
port = 8080

s.connect((host, port))
http_end = '\r\n\r\n'

for i in range(0, 1000):
	random_bytes = os.urandom(512)
	s.send(random_bytes)
	s.send(http_end.encode())
	# response = s.recv(4096)
	# print(response.decode('utf-8'))
	print(f'sent request: {i}')



s.close()