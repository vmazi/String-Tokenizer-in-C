This tokenizer works by using the 3 functions, TKCreate,TKGetNextToken&printToken.

TKGetNextToken reads the input stream char by char and constructs the token by only 
adding characters of the same tokentype and updating the tokentype when nessecary.
This then converts the char by char list into a null terminated string. Furthermore,
this method has the parameter of a pointer of a pointer, allowing it to advance the
input stream pointer.

TKCreate takes in a string token, figures out what tokentype it is, and allocates memory
to store the token and its tokentype.

printToken takes a token, and uses its enumerated tokentype to determine its print behavior.


DeleteToken then deletes the token.

by Vamsi Kurkal and Armin Grossrieder