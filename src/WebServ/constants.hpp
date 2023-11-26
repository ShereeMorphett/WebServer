#ifndef CONSTANTS_HPP
# define CONSTANTS_HPP

# define	OK 			200;
# define	NOT_FOUND	404;

# define 	END_HEADER 	"\r\n\r\n"

# define 	TYPE_HTML	"text/html"
# define 	TYPE_CSS		"text/css"

enum FileType {
	UNKNOWN,
	HTML,
	CSS,
	PNG,
	JS,
};

#endif // !CONSTANTS_HPP
