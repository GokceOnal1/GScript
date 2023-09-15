# GScript (version 2)
## Todo:
Add support for reference parameters <br>
Add standard functions for list handling <br>
Standardize error system
## Error Codes:

```
GS.b.01 - Failed reading from entry input stream
GS.b.02 - Internal null pointer 
GS.101 - Unexpected token %s (of type %d)
GS.102 - Unsupported character %s
GS.103 - Expected '=' after '!'
GS.104 - Unrelated "elsif" caught
GS.105 - Unrelated "else" caught
GS.201 - Uncaught statement of type %d
GS.301 - Undefined identifier %s
GS.302 - Variable '%s' already exists in the current scope
GS.303 - Variable '%s' does not exist in the current scope
GS.501 - No arguments provided where needed
GS.502 - Function '%s' does not take %d arguments
GS.503 - Function '%s' does not exist in the current scope
GS.504 - Illegal argument to function %s
GS.505 - Function '%s' already exists in the current scope
GS.506 - Expected keyword "param" at function definition
GS.601 - Dividing by zero is illegal
GS.602 - Invalid syntax for a number
GS.603 - Operator '+' only supports num+num or str+str
```
## Supports:
- [x] Strings
- [x] Math
- [x] Conditional Expressions
- [x] Variables
- [x] Functions
- [ ] Lists
- [ ] Objects
- [ ] Classes
- [x] Loops