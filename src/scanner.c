#include "tree_sitter/parser.h"


enum TokenType {
    STRING,
};



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