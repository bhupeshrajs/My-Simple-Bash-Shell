My Simple Bash Shell

Compilation :
	lex shell.l
	gcc -c lex.yy.c -lfl
	gcc -c myshell.c
	gcc myshell.o lex.yy.o -o myshell -lfl
	
Running :
	./myshell
	
Regular commands like ls, grep can be executed.
cd command can be executed.
commands with multiple piping and input/output redirection can be executed.


Refer design.pdf for more detailed design of the Shell.
