#ifndef CONSTANTS_HPP
# define CONSTANTS_HPP

# define	BUFFER_SIZE		10240

// Status codes
# define	ERRORS			400
# define 	REQUEST_TIMEOUT 408
# define	OK 				200
# define	CREATED			201
# define	UNAUTHORIZED	401
# define	FORBIDDEN		403
# define	NOT_FOUND		404
# define	TOO_LARGE		413
# define	INT_ERROR		500
# define	ACCEPTED		202
# define	NO_CONTENT		204

// Request status
# define	REQUEST_DONE			0
# define	REQUEST_NOT_DONE		1
# define	REQUEST_ERROR			2
# define	REQUEST_CLIENT_CLOSED	3
# define	REQUEST_TOO_BIG			4

// Response formatting lines
# define	HTTP_HEADER	"HTTP/1.1 "
# define 	END_HEADER	"\r\n\r\n"
# define	NEW_VALUE	"\r\n"

// Should include all possible files for GET
# define 	TYPE_HTML	"text/html"
# define 	TYPE_CSS	"text/css"
# define	TYPE_PNG	"image/png"
# define	TYPE_JPEG	"image/jpeg"

# define	EXT_HTML	"html"
# define	EXT_CSS		"css"
# define	EXT_PNG		"png"
# define	EXT_JPEG	"jpeg"

// Default file paths
# define	NOT_FOUND_PAGE		"/src/defaults/404.html"
# define	FORBIDDEN_PAGE		"/src/defaults/403.html"
# define	UNAUTHORIZED_PAGE	"/src/defaults/401.html"
# define	SERVER_PAGE			"/src/defaults/500.html"

// All methods server supports
# define	DELETE	'D'
# define	POST	'P'
# define	GET		'G'

#endif // !CONSTANTS_HPP
