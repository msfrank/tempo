parser grammar ConfigParser;

options { tokenVocab = ConfigLexer; }


root                : node ;

node                : object
                    | array
                    | literal
                    ;


// object

memberKey           : StringLiteral
                    | numberLiteral
                        { notifyErrorListeners("number cannot be an object key"); }
                    | trueLiteral
                        { notifyErrorListeners("'true' cannot be an object key"); }
                    | falseLiteral
                        { notifyErrorListeners("'false' cannot be an object key"); }
                    | nullLiteral
                        { notifyErrorListeners("'null' cannot be an object key"); }
                    ;
memberValue         : node;
member              : memberKey ColonOperator memberValue ;
firstMember         : CommaOperator member
                        { notifyErrorListeners("Leading ',' before first object member"); }
                    | member
                    ;
remainingMembers    : CommaOperator CommaOperator member
                        { notifyErrorListeners("Extra ',' between object members"); }
                    | CommaOperator member
                    ;
memberSequence      : firstMember remainingMembers* CommaOperator? ;
object              : CurlyOpen CurlyClose
                    | CurlyOpen memberSequence CurlyClose
                    | CurlyOpen memberSequence CurlyClose CurlyClose
                        { notifyErrorListeners("Extra '}' closing object"); }
                    | CurlyOpen memberSequence
                        { notifyErrorListeners("Missing '}' closing object"); }
                    ;


// array

element             : node ;
firstElement        : CommaOperator element
                        { notifyErrorListeners("Leading ',' before first array element"); }
                    | element
                    ;
remainingElements   : CommaOperator CommaOperator element
                        { notifyErrorListeners("Extra ',' between array elements"); }
                    | CommaOperator element
                    ;
elementSequence     : firstElement remainingElements* CommaOperator? ;
array               : BracketOpen BracketClose
                    | BracketOpen elementSequence BracketClose
                    | BracketOpen elementSequence BracketClose BracketClose
                        { notifyErrorListeners("Extra ']' closing array"); }
                    | BracketOpen elementSequence
                        { notifyErrorListeners("Missing ']' closing array"); }
                    |
                    ;


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

