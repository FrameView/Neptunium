#include <stdio.h>
#include <string.h>

#include "scanner.h"

void initScanner(Scanner* s, const char* source) {
    s->start = source;
    s->current = source;
    s->line = 1;
}

static bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            c == '_';
}

static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

static char peek(Scanner* s) {
    return *s->current;
}

static bool isAtEnd(Scanner* s) {
    return peek(s) == '\0';
}

static char peekNext(Scanner* s) {
    if (isAtEnd(s)) return '\0';
    return s->current[1];
}

static char advance(Scanner* s) {
    s->current++;
    return s->current[-1];
}

static bool match(Scanner* s, char expected) {
    if (isAtEnd(s)) return false;
    if (peek(s) != expected) return false;
    s->current++;
    return true;
}

static void skipWhitespace(Scanner* s) {
    for (;;) {
        char c = peek(s);
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance(s); break;
            case '\n':
                s->line++; advance(s); break;     
            case '/':
                if (peekNext(s) == '/') {
                    while (peek(s) != '\n' && !isAtEnd(s)) {
                        advance(s);
                    }
                } else {
                    return;
                }
                break;    
            default:
                return;    
        }
    }
} 

static Token makeToken(Scanner* s, TokenType type) {
    Token token;
    token.type = type;
    token.start = s->start;
    token.length = (int)(s->current - s->start);
    token.line = s->line;
    return token;
}

static Token errorToken(Scanner* s, const char* message){
    Token token;
    token.type = TOKEN_ERR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = s->line;
    return token;
}

static Token string(Scanner* s) {
    while(peek(s) != '"' && !isAtEnd(s)) {
        if (peek(s) == '\n') s->line++;
        advance(s);
    }

    if (isAtEnd(s)) return errorToken(s, "Unterminated string.");

    advance(s);
    return makeToken(s, TOKEN_STRING);
}

static Token number(Scanner* s) {
    while (isDigit(peek(s))) advance(s);

    if (peek(s) == '.' && isDigit(peekNext(s))) {
        advance(s);
        while (isDigit(peek(s))) advance(s);
    }

    return makeToken(s, TOKEN_NUMBER);
}

static TokenType checkKeyword(Scanner* s, int start, int length,
                              const char* rest, TokenType type) {
    if (s->current - s->start == start + length &&
        memcmp(s->start + start, rest, length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}

static TokenType identifierType(Scanner* s) {
    switch (s->start[0]) {
        case 'a': return checkKeyword(s, 1, 2, "nd", TOKEN_AND);
        case 'b': return checkKeyword(s, 1, 3, "ase", TOKEN_BASE);
        case 'e':
            if (s->current - s->start > 1) {
                switch (s->start[1]) {
                    case 'c': return checkKeyword(s, 2, 2, "ho", TOKEN_ECHO);
                    case 'l': return checkKeyword(s, 2, 2, "se", TOKEN_ELSE);
                }
            }
            break;
        case 'f':
            if (s->current - s->start > 1) {
                switch (s->start[1]) {
                    case 'a': return checkKeyword(s, 2, 3, "lse", TOKEN_FALSE);
                    case 'n': return checkKeyword(s, 2, 0, "", TOKEN_FN);
                    case 'o': return checkKeyword(s, 2, 1, "r", TOKEN_FOR);
                }
            }
            break;
        case 'i': return checkKeyword(s, 1, 1, "f", TOKEN_IF);
        case 'n': return checkKeyword(s, 1, 3, "ull", TOKEN_NULL);
        case 'o':
            if (s->current - s->start > 1) {
                switch (s->start[1]) {
                    case 'b': return checkKeyword(s, 2, 1, "j", TOKEN_OBJ);
                    case 'r': return checkKeyword(s, 2, 0, "", TOKEN_OR);
                }
            }
            break;
        case 'r': return checkKeyword(s, 1, 2, "et", TOKEN_RET);
        case 't':
            if (s->current - s->start > 1) {
                switch (s->start[1]) {
                    case 'h': return checkKeyword(s, 2, 2, "is", TOKEN_THIS);
                    case 'r': return checkKeyword(s, 2, 2, "ue", TOKEN_TRUE);
                }
            }
            break;
        case 'u': return checkKeyword(s, 1, 4, "ntil", TOKEN_UNTIL);    
        case 'v': return checkKeyword(s, 1, 2, "ar", TOKEN_VAR);
        case 'w': return checkKeyword(s, 1, 4, "hile", TOKEN_WHILE);
    }
    return TOKEN_IDENTIFIER;
}

static Token identifier(Scanner* s) {
    while (isAlpha(peek(s)) || isDigit(peek(s))) advance(s);
    return makeToken(s, identifierType(s));
}

Token scanToken(Scanner* s) {
    skipWhitespace(s);
    s->start = s->current;
    if (isAtEnd(s)) return makeToken(s, TOKEN_EOF);

    char c = advance(s);
    if (isAlpha(c)) return identifier(s);
    if (isDigit(c)) return number(s);

    switch (c) {
        case '(': return makeToken(s, TOKEN_LEFT_PAREN);
        case ')': return makeToken(s, TOKEN_RIGHT_PAREN);
        case '{': return makeToken(s, TOKEN_LEFT_BRACE);
        case '}': return makeToken(s, TOKEN_RIGHT_BRACE);
        case ';': return makeToken(s, TOKEN_SEMICOLON);
        case ',': return makeToken(s, TOKEN_COMMA);
        case '.': return makeToken(s, TOKEN_DOT);
        case '-': return makeToken(s, TOKEN_MINUS);
        case '+': return makeToken(s, TOKEN_PLUS);
        case '/': return makeToken(s, TOKEN_SLASH);
        case '*': return makeToken(s, TOKEN_STAR);
        case '!': return makeToken(s, 
            match(s, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=': return makeToken(s, 
            match(s, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<': return makeToken(s, 
            match(s, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>': return makeToken(s, 
            match(s, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '"': return string(s);    
    }

    return errorToken(s, "Unexpected character.");
}