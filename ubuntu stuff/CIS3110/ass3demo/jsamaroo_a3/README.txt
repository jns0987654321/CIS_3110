Name: Junior Samaroo
Student ID# 0663108
CIS3110 Assignment 3

resed.c:
	Server providing time, file management and remote execution services.
	
resec.c:
	Client requesting time, file management and remote execution services.
	
A3_docs.txt:
	Brief outline in the coding of resed and resec
	
makefile:
	Makefile used to compile resed.c and resec.c
	
	
	
COMPILATION:
	type "make" "make all" or "make rese" within working 
	directory to run the makefile. "resed" and "resec" executables are created.
	
EXECUTION:
	After compilation user must first run the server executable "resed"
	
	The user must then run the client executable "resec" to request appropriate services.
	The following is the usage of the client where "host" is the hostname of server:
	
	resec -t host
		request time from server on host

	resec -f host command [filename] [perm]
		request file service from server on host; command is one of the following:
			list
				obtain list of all files in remote file directory; output the list to stdout
			get filename
				transfer filename from remote server and store it in the local working directory
			put filename perm
				transfer filename to remote file directory, saving it with specified permissions (perm); 
				permissions should be provide as a 1-digit octal value (e.g. 7 == rwx, 5 = r-x, 1 = --x, 0 == ---)

	resec -e host "command"
		request remote execution service from server on host; command to be executed is given by the literal string "command"; 
		the output of the remote command is sent back to the client and outputs on stdout 
