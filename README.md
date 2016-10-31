To build this project:
- Open the project folder in terminal
- run `cmake .`
- run `make`

The generated executable will be called: `rum`

To run it in file mode use: `rum filename`

To run it in interactive mode use: `rum`

There is an included test file `test.rum`. The parse tree generated from `test.rum` is available as
`test_rum_parse_tree.png`.

To exit the interpreter use: exit$

The interpreter will stop looking for input and proceed to tokenize when a $ is the last character on a line.