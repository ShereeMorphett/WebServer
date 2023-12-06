#ifndef CONSTANTS_HPP
# define CONSTANTS_HPP

// Status codes
# define	OK 			200;
# define	NOT_FOUND	404;

// Response formatting lines
# define	HTTP_HEADER	"HTTP/1.1 "
# define 	END_HEADER	"\r\n\r\n"
# define	NEW_VALUE	"\r\n"

// Should include all possible files for GET
# define 	TYPE_HTML	"text/html"
# define 	TYPE_CSS	"text/css"

# define	EXT_HTML	".html";
# define	EXT_CSS		".css";

// Default file paths
# define	ERROR_PAGE	"/src/defaults/404.html"

// All methods server supports
# define	DELETE		'D'
# define	POST		'P'
# define	GET			'G'

#endif // !CONSTANTS_HPP
