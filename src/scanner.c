#include "tree_sitter/parser.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

enum TokenType {
    STRING,
};

typedef enum {
    SingleQuote = 1 << 0,
    DoubleQuote = 1 << 1,
    BackQuote = 1 << 2,
    Raw = 1 << 3,
    Format = 1 << 4,
    Triple = 1 << 5,
    Bytes = 1 << 6,
} Flags;

typedef struct {
    char flags;
} Delimiter;

static inline Delimiter new_delimiter() { return (Delimiter){0}; }

static inline bool is_raw(Delimiter *delimiter) { return delimiter->flags & Raw; }
static inline bool is_triple(Delimiter *delimiter) { return delimiter->flags & Triple; }
static inline bool is_bytes(Delimiter *delimiter) { return delimiter->flags & Bytes; }

static inline int32_t end_character(Delimiter *delimiter) {
    if (delimiter->flags & SingleQuote) {
        return '\'';
    }
    if (delimiter->flags & DoubleQuote) {
        return '"';
    }
    if (delimiter->flags & BackQuote) {
        return '`';
    }
    return 0;
}

static inline void set_format(Delimiter *delimiter) { delimiter->flags |= Format; }
static inline void set_raw(Delimiter *delimiter) { delimiter->flags |= Raw; }
static inline void set_triple(Delimiter *delimiter) { delimiter->flags |= Triple; }
static inline void set_bytes(Delimiter *delimiter) { delimiter->flags |= Bytes; }

static inline void set_end_character(Delimiter *delimiter, int32_t character) {
    switch (character) {
        case '\'':
            delimiter->flags |= SingleQuote;
            break;
        case '"':
            delimiter->flags |= DoubleQuote;
            break;
        case '`':
            delimiter->flags |= BackQuote;
            break;
        default:
            assert(false);
    }
}

static inline void advance(TSLexer *lexer) { lexer->advance(lexer, false); }
static inline void skip(TSLexer *lexer) { lexer->advance(lexer, true); }

bool tree_sitter_python_external_scanner_scan(void *payload, TSLexer *lexer, const bool *valid_symbols) {
    if (valid_symbols[STRING]) {
        while (lexer->lookahead == ' ' || lexer->lookahead == '\n' || lexer->lookahead == '\r' || lexer->lookahead == '\t') {
            skip(lexer);
        }

        if (lexer->lookahead == '"') {
            advance(lexer);
            while (lexer->lookahead) {
                if (lexer->lookahead == '"') {
                    advance(lexer);
                    lexer->mark_end(lexer);
                    lexer->result_symbol = STRING;
                    return true;
                }
                advance(lexer);
            }
        }
        if (lexer->lookahead == '\'') {
             advance(lexer);
             while (lexer->lookahead) {
                 if (lexer->lookahead == '\'') {
                     advance(lexer);
                     lexer->mark_end(lexer);
                     lexer->result_symbol = STRING;
                     return true;
                 }
                 advance(lexer);
             }
        }
    }
    return false;
}

unsigned tree_sitter_python_external_scanner_serialize(void *payload, char *buffer) { return 0; }
void tree_sitter_python_external_scanner_deserialize(void *payload, const char *buffer, unsigned length) {}
void *tree_sitter_python_external_scanner_create() { return NULL; }
void tree_sitter_python_external_scanner_destroy(void *payload) {}