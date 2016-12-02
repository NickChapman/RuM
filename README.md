To build this project
---
- Open the project folder in terminal
- run `cmake .`
- run `make`

The generated executable will be called: `rum`

To run this project
---

To run it in file mode use: `rum filename`

To run it in interactive mode use: `rum`

There is an included test file `test.rum`. The parse tree generated from `test.rum` is available as
`test_rum_parse_tree.png`.

To exit the interpreter use: exit$

The interpreter will stop looking for input and proceed to tokenize when a $ is the last character on a line.

Implementation notes
----
- Variable assignments, shallow and deep are working
- Control structures such as if/else, while, and functions are working
- Function scoping is working with appropriate runtime stack and garbage collection
- Boolean expressions can be evaluated such as `TRUE && TRUE`
    - Note that this RuM implementation allows `TRUE && TRUE && TRUE`
- The `print` function is working
- Objects have not been attempted here because they messed up everything
- As lists are objects they also are not present in this build
