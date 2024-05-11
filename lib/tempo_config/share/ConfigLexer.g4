lexer grammar ConfigLexer;

channels { COMMENTS }

// comment styles

SingleLineComment           : '//' .*? '\n' -> channel(COMMENTS) ;
MultiLineComment            : '/*' .*? '*/' -> channel(COMMENTS) ;


// reserved marker pairs

BracketOpen                 : '[' ;
BracketClose                : ']' ;
CurlyOpen                   : '{' ;
CurlyClose                  : '}' ;


// a number literal is any sequence of characters enclosed in double-quotes

fragment
HexDigit                    : ('0'..'9'|'a'..'f'|'A'..'F') ;

fragment
Exponent                    : ('e'|'E') ('+'|'-')? ('0'..'9')+ ;

fragment
WholeNumber                 : ('0' | '1'..'9' '0'..'9'*) ;

HexInteger                  : '-'? '0' ('x'|'X') HexDigit+ ;

DecimalInteger              : '-'? WholeNumber ;

OctalInteger                : '-'? '0' ('0'..'7')+ ;

HexFloat                    : '-'? ('0x' | '0X') (HexDigit )*
                              ('.' (HexDigit)*)?
                              ( ( 'p' | 'P' )
                                ( '+' | '-' )?
                                HexDigit+)?
                            ;

DecimalScientificFloat      : '-'? WholeNumber '.' ('0'..'9')* Exponent
                            | '-'? '.' ('0'..'9')+ Exponent
                            | '-'? WholeNumber Exponent
                            ;

DecimalFixedFloat           : '-'? WholeNumber '.' ('0'..'9')*
                            | '-'? '.' ('0'..'9')+
                            ;


// a char literal is a single character or escape sequence enclosed in single-quotes

CharLiteral                 : '\'' ( EscapeSequence | ~('\\'|'"') ) '\'' ;


// a string literal is any sequence of characters (including escape sequences) enclosed in double-quotes

StringLiteral               : '"' ( EscapeSequence | ~('\\'|'"') )* '"' ;

// a url literal is any sequence of characters (including escape sequences) enclosed in backticks

UrlLiteral                  : '`' ( EscapeSequence | ~('\\'|'`') )* '`' ;


fragment
EscapeSequence              : '\\' ('b'|'t'|'n'|'f'|'r'|'"'|'\''|'\\')
                            | Hex1ByteEscape
                            | HexVariableEscape
                            | Unicode2ByteEscape
                            | Unicode4ByteEscape
                            | OctalEscape
                            ;

fragment
OctalEscape                 : '\\' ('0'..'3') ('0'..'7') ('0'..'7')
                            | '\\' ('0'..'7') ('0'..'7')
                            | '\\' ('0'..'7')
                            ;

fragment
Hex1ByteEscape              : '\\' 'x' HexDigit HexDigit? ;

fragment
HexVariableEscape           : '\\' 'x' '{' HexDigit+ '}' ;

fragment
Unicode2ByteEscape          : '\\' 'u' HexDigit HexDigit HexDigit HexDigit ;

fragment
Unicode4ByteEscape          : '\\' 'U' HexDigit HexDigit HexDigit HexDigit HexDigit HexDigit HexDigit HexDigit ;


// reserved sigils

ColonOperator               : ':' ;
CommaOperator               : ',' ;


// reserved keywords

TrueKeyword                 : 'true' ;
FalseKeyword                : 'false' ;
NullKeyword                 : 'null' ;


// an identifier is can be a variable, function, or state symbol

fragment
IdentifierStart             : [a-z] | [A-Z] | '_' ;

fragment
IdentifierChar              : IdentifierStart | [0-9] ;

Identifier                  : IdentifierStart IdentifierChar* ;


/** other definitions */

EXPRWS                      : [ \t\r\n]+ -> skip ;  // skip whitespace

mode COMMENT;
CommentLine             : ( ~[\r\n]+ )? '\r'? '\n' -> popMode ;      // match characters until after the newline
