#ifndef _TOKEN_ENUM_
#define _TOKEN_ENUM_

enum class TokenKind : int {
    TK_KEYWORD,     // reserved keywords
    TK_PUNCTUATOR,  // punctuators
    TK_OPERATOR,    // operators
    TK_IDENT,       // Identifiers
    TK_NUM,         // Numeric literals
    TK_STRING,      // String literals
    TK_PROGRAM,     // start program
    TK_EOF,         // End of input
};

/*
When update or fix TokenDetail, must fix below functions.
setTokenKeywords(), setPunctuators(), setOperators() in "LexicalAnalysis/reserved_keywords.cpp"
*/
enum class TokenDetail : int {
    // keywords
    DEF_FUNCTION,           // function
    DEF_INT32,              // int32
    DEF_STRING,             // string
    DEF_VOID,               // void
    DEF_RETURN,             // return
    IF,                     // if
    ELSE,                   // else
    TRUE,                   // TRUE
    FALSE,                  // FALSE

    // punctuators
    LEFT_PARENTHESE,        // (
    RIGHT_PARENTHESE,       // )
    LEFT_BRACKET,           // {
    RIGHT_BRACKET,          // }
    SEMICOLON,              // ;
    COMMA,                  // ,
    DOUBLE_QUOTATION,       // "

    // operators
    RETURN_TYPE,            // ->
    MOD,                    // MOD
    AND,                    // AND
    OR,                     // OR
    NOT,                    // NOT
    CMP_EQUAL,              // ==
    CMP_NOT_EQUAL,          // !=
    SMALLER_EQUAL,          // <=
    LARGER_EQUAL,           // >=
    EQUAL,                  // =
    NOT_SYM,                // !
    PLUS,                   // +
    MINUS,                  // -
    TIMES,                  // *
    DIVISION,               // /
    LEFT_ANGLE_BRACKET,     // <
    RIGHT_ANGLE_BRACKET,    // >

    // nothing
    NOTHING,
};

#endif
