#ifndef CONSTANTS_HPP
# define CONSTANTS_HPP

// Status codes
# define	ERRORS			400

# define	OK 				200
# define	CREATED			201
# define	UNAUTHORIZED	401
# define	FORBIDDEN		403
# define	NOT_FOUND		404
# define	INT_ERROR		500

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
