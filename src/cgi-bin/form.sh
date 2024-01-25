#!/bin/bash

read input_data

# Redirect all output to stderr
exec 1>&2

# Print headers
echo "Content-type: text/plain"
echo ""

# Print received data
echo "Received data:"
echo "$input_data"

# Print environment variables
echo "Environment variables:"
env

# Parse form data
name=$(echo "$input_data" | grep -oP "name=\K[^\&]*" | sed 's/%20/ /g')

# HTML response
echo "hello, $name"

# Print error message to stderr
echo "An error occurred"
