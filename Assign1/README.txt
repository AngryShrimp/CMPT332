CMPT332 - Assignment 1
Keenan Johnstone - kbj182 - 11119412
Kyle Scheasby - kms123 - 11112400

To run xv6 with the added getpcount functionality:
	-Run the make file in the xv6 folder
	-run make qemu-nox

To run the osh for part B:
	-Run the make file in the PartB folder
	-run 'osh' in the command line

	NOTE: The osh is missing the ability to ouput redirect in piping via the '<' command, the '|' character still allows for multiple commands to be run however
	NOTE: There is a pedantic warning for variable inPipe = o; that is never used. The code using it was incomplete and commented out as we plan to finish the code in the near future.

	NOTE: At the very last second we noticed it wasnt compiling or running properly, I grabbed the previous git version but Tuxworld was not letting me login, due to the last second panic, I hope you understand why I submitted the shell.c outside of the tar

	Should you request it I will login to tuxworld when it;s up
	