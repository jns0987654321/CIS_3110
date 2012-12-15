Name: Junior Samaroo
Student ID# 0663108
CIS3110 Assignment 2

scavenger.c:
	Scheduling facility for spawning and managing background processes.
	
A1_docs.txt:
	Brief outline in the coding of scavenger.
	
makefile:
	Makefile used to compile scavenger.
	
	
	
COMPILATION:
	type "make" "make all" or "make scavenger" within working 
	directory to run the makefile. Scavenger executable is created.
	
EXECUTION:
	After compilation the user must run:
	"scavenger IDLE=<idleoad> BUSY=<busyload> USER=<usertime> pname args" 
	where: idleload = minimum loadvalues
		   busyload = maximum loadvalues
		   userime  = required pause time by an interactive user before it can be considered inactive
		   pname    = program name to exec into background
		   args     = arguments to pname
