To build this project use cmake and then make

The generated executable will be called: rum

To run it in file mode use: rum filename

To run it in interactive mode use: rum

There is an included test file: test.rum

To exit the interpreter use: exit$

The interpreter will stop looking for input and proceed to tokenize when a $ is the last character on a line. 
Note: It will only tokenize up to a $ even if it is not at the end of a line.
