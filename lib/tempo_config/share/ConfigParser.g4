parser grammar ConfigParser;

options { tokenVocab = ConfigLexer; }


root                : node ;

node                : object
                    | array
                    | literal
                    ;


// object

memberKey           : StringLiteral ;
memberValue         : node;
member              : memberKey ColonOperator memberValue ;
object              : CurlyOpen ( member ( CommaOperator member )* )? CurlyClose ;


// array

element             : node ;
array               : BracketOpen ( element ( CommaOperator element )* )? BracketClose ;


// literal structures

literal             : numberLiteral | stringLiteral | trueLiteral | falseLiteral | nullLiteral ;

integer             : DecimalInteger ;
fixedFloat          : DecimalFixedFloat ;
scientificFloat     : DecimalScientificFloat ;
numberLiteral       : integer
                    | fixedFloat
                    | scientificFloat
                    ;

stringLiteral       : StringLiteral ;

trueLiteral         : TrueKeyword ;
falseLiteral        : FalseKeyword ;
nullLiteral         : NullKeyword ;


// comment

comment             : SingleLineComment | MultiLineComment ;

