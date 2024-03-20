#include <tree_sitter/parser.h>
#include <string.h>
#include <stdio.h>

#define fprintf(...) // 

typedef struct
{
    uint32_t len;
    uint32_t cap;
    uint16_t *data;
} State;

enum TokenType
{
    VIRT_START,
    VIRT_SEMI,
    VIRT_END,
    COMMENT,
};


void ensure(State *state, uint32_t count)
{
    if (state->cap < count)
    {
        state->cap = count * 2;
        uint16_t *new_data = malloc(sizeof(uint16_t) * state->cap);
        memcpy(new_data, state->data, state->len * sizeof(uint16_t));
        free(state->data);
        state->data = new_data;
    }
}

void *push(State *state, uint16_t col) {
//    fprintf(stderr, "push %d\n", col);
    ensure(state, state->len + 1);
    state->data[state->len++] = col;
}

uint32_t *pop(State *state) {
    if (state->len) {
//        fprintf(stderr, "pop %d\n", state->data[state->len-1]);
        state->len--;
        return state->data[state->len];
    }
}

int32_t * peek(State *state) {
    return state->len ? state->data[state->len-1] : -1;  // or -1?
}

#define PEEK lexer->lookahead

// internal scanner not skipping comments?
bool tree_sitter_piforall_external_scanner_scan(State *state, TSLexer *lexer, const bool *syms)
{
    fprintf(stderr, "%d %d %d %d\n", syms[0], syms[1], syms[2], syms[3] );
    // skip whitespace
    while (PEEK == ' ' || PEEK == '\n' || PEEK == '\t') {
        lexer->advance(lexer,true);
    }
    
    // Might have to deal with comments in here.
    if (PEEK == '-' || PEEK == '{') return false;
    int32_t cur = peek(state);
    uint32_t col = lexer->get_column(lexer);
    if (syms[VIRT_START]) {
        fprintf(stderr, "start [%d %d %d %d] %d %d\n", syms[0], syms[1], syms[2], syms[3], col, cur);
        push(state, col);
        lexer->result_symbol = VIRT_START;
        return true;
    } 
    if (syms[VIRT_END]) {

        if (col < cur) {
            fprintf(stderr, "end [%d %d %d %d] %d %d\n", syms[0], syms[1], syms[2], syms[3], col, cur);
            pop(state);
            lexer->result_symbol = VIRT_END;
            return true;
        }
    }
    if (syms[VIRT_SEMI]) {
        lexer->result_symbol = VIRT_SEMI;
        // FIXME - not eof, but we are requiring one at end of file at the moment.
        if (!lexer->eof(lexer) && col == cur) {
            fprintf(stderr, "semi [%d %d %d %d] %d %d\n", syms[0], syms[1], syms[2], syms[3], col, cur);
            return true;
        }  else {
            fprintf(stderr, "not semi [%d %d %d %d] %d %d\n", syms[0], syms[1], syms[2], syms[3], col, cur);
        }
    }
    return false;
}

void *tree_sitter_piforall_external_scanner_create()
{
    State *state = calloc(sizeof(State), 1);
    state->cap = 20;
    state->data = malloc(sizeof(uint16_t) * state->cap);
    return state;
}

void tree_sitter_piforall_external_scanner_destroy(State *state)
{
    free(state->data);
    free(state);
}



unsigned tree_sitter_piforall_external_scanner_serialize(State *state, char *buffer)
{
    unsigned size = sizeof(state->data[0]) * state->len;
    if (size > TREE_SITTER_SERIALIZATION_BUFFER_SIZE)
    {

        return 0;
    }
    memcpy(buffer, state->data, size);
    return size;
}

void tree_sitter_piforall_external_scanner_deserialize(State *state, char *buffer, unsigned length)
{
    unsigned len = length / sizeof(state->data[0]);
    if (len > 0)
    {
        ensure(state, len);
        state->len = len;
        memcpy(state->data, buffer, length);
    }
}
