/* Copyright 2020-2021 doodboard
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include QMK_KEYBOARD_H

#define EXPRESSIONS_BUFF_SIZE 64
int input_count = 0;                            // stores the count of the filled in expressions_buffer.
char expressions_buffer[EXPRESSIONS_BUFF_SIZE]; // stores the typed out string
char last_answer[EXPRESSIONS_BUFF_SIZE];        // stores the previous answer

// TinyExpr 
typedef struct te_expr {
    int type;
    union {double value; const double *bound; const void *function;};
    void *parameters[1];
} te_expr;

enum {
    TE_VARIABLE = 0,

    TE_FUNCTION0 = 8, TE_FUNCTION1, TE_FUNCTION2, TE_FUNCTION3,
    TE_FUNCTION4, TE_FUNCTION5, TE_FUNCTION6, TE_FUNCTION7,

    TE_CLOSURE0 = 16, TE_CLOSURE1, TE_CLOSURE2, TE_CLOSURE3,
    TE_CLOSURE4, TE_CLOSURE5, TE_CLOSURE6, TE_CLOSURE7,

    TE_FLAG_PURE = 32
};

typedef struct te_variable {
    const char *name;
    const void *address;
    int type;
    void *context;
} te_variable;

/* Parses the input expression, evaluates it, and frees it. */
double te_interp(const char *expression, int *error);

/* Parses the input expression and binds variables. */
te_expr *te_compile(const char *expression, const te_variable *variables, int var_count, int *error);

/* Evaluates the expression. */
double te_eval(const te_expr *n);

/* Prints debugging information on the syntax tree. */
void te_print(const te_expr *n);

/* Frees the expression. */
void te_free(te_expr *n);

void write_char_to_buff(char c);

enum layer_codes {
    L3_1 = SAFE_RANGE,
    L3_2,
    L3_3,
    L3_4,
    L3_5,
    L3_6,
    L3_7,
    L3_8,
    L3_9,
    L3_0,
    L3_SLASH,
    L3_MULTIPLY,
    L3_MINUS,
    L3_PLUS,
    L3_EQUALS,
    L3_DOT,
    L3_PRINT_ANS,
    L3_POWER,
    L3_MOD,
    L3_EXIT,
};

//Layout
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT( //default
                TG(1),   KC_PSLS, KC_PAST, KC_PMNS,
                KC_7,    KC_8,    KC_9,
                KC_4,    KC_5,    KC_6,    KC_PPLS,
                KC_1,    KC_2,    KC_3,
        TG(3),  KC_0,    KC_0,    KC_DOT,  KC_ENT),

    [1] = LAYOUT(
                TG(1),   KC_TRNS, KC_TRNS, KC_TRNS,
                KC_HOME, KC_UP,   KC_PGUP,
                KC_LEFT, KC_TRNS, KC_RGHT, KC_TRNS,
                KC_END,  KC_DOWN, KC_PGDN,
        TG(2),  KC_TRNS, KC_INS,  KC_DEL,  KC_TRNS),

    [2] = LAYOUT(
                KC_TRNS, RGB_TOG, RGB_MOD, KC_TRNS,
                RGB_HUI, RGB_SAI, RGB_VAI,
                RGB_HUD, RGB_SAD, RGB_VAD, KC_TRNS,
                KC_TRNS, KC_TRNS, KC_TRNS,
        TG(2),  QK_BOOT, KC_TRNS, KC_TRNS, KC_TRNS),

    [3] = LAYOUT(
                L3_EXIT, L3_SLASH, L3_MULTIPLY, L3_MINUS,
                L3_7,    L3_8,     L3_9,           
                L3_4,    L3_5,     L3_6,        L3_PLUS,
                L3_1,    L3_2,     L3_3,          
    L3_PRINT_ANS,L3_0,   L3_0,     L3_DOT,      L3_EQUALS),

};

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) { /* First encoder */
        if (clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    }
    return true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case L3_1:
            if (record->event.pressed) {
                write_char_to_buff('1');
            }
            break;
        case L3_2:
            if (record->event.pressed) {
                write_char_to_buff('2');
            }
            break;
        case L3_3:
            if (record->event.pressed) {
                write_char_to_buff('3');
            }
            break;
        case L3_4:
            if (record->event.pressed) {
                write_char_to_buff('4');
            }
            break;
        case L3_5:
            if (record->event.pressed) {
                write_char_to_buff('5');
            }
            break;
        case L3_6:
            if (record->event.pressed) {
                write_char_to_buff('6');
            }
            break;
        case L3_7:
            if (record->event.pressed) {
                write_char_to_buff('7');
            }
            break;
        case L3_8:
            if (record->event.pressed) {
                write_char_to_buff('8');
            }
            break;
        case L3_9:
            if (record->event.pressed) {
                write_char_to_buff('9');
            }
            break;
        case L3_0:
            if (record->event.pressed) {
                write_char_to_buff('0');
            }
            break;
        case L3_PLUS:
            if (record->event.pressed) {
                write_char_to_buff('+');
            }
            break;
        case L3_MINUS:
            if (record->event.pressed) {
                write_char_to_buff('-');
            }
            break;
        case L3_MULTIPLY:
            if (record->event.pressed) {
                write_char_to_buff('*');
            }
            break;
        case L3_SLASH:
            if (record->event.pressed) {
                write_char_to_buff('/');
            }
            break;
        case L3_DOT:
            if (record->event.pressed) {
                write_char_to_buff('.');
            }
            break;
        case L3_POWER:
            if (record->event.pressed) {
                write_char_to_buff('^');
            }
            break;
        case L3_MOD:
            if (record->event.pressed) {
                write_char_to_buff('%');
            }
            break;
        case L3_EQUALS:
            if (record->event.pressed) {
                double result = te_interp(expressions_buffer, 0);
                char output_string[EXPRESSIONS_BUFF_SIZE];
                dtostrf(result, 1, 2, output_string);
                strcpy(last_answer,output_string);
                input_count = 0;
            }
            break;
        case L3_PRINT_ANS:
            if (record->event.pressed) {
                if(input_count<=0){
                    send_string(last_answer);
                }
            }
            break;
        case L3_EXIT:
            if(record->event.pressed){
                input_count = 0;
                expressions_buffer[0] = '\0';
                last_answer[0] = '\0';
                layer_move(0);
            }
            break;
    }
	return true;
}


void write_char_to_buff(char c){
    if(input_count+1 < EXPRESSIONS_BUFF_SIZE){
        expressions_buffer[input_count] = c;
        expressions_buffer[input_count+1] = '\0';
        input_count++;
    }
}


//TinyExpr Functions 
#ifndef NAN
#define NAN (0.0/0.0)
#endif

#ifndef INFINITY
#define INFINITY (1.0/0.0)
#endif

typedef double (*te_fun2)(double, double);

enum {
    TOK_NULL = TE_CLOSURE7+1, TOK_ERROR, TOK_END,
    TOK_NUMBER, TOK_VARIABLE, TOK_INFIX
};


enum {TE_CONSTANT = 1};


typedef struct state {
    const char *start;
    const char *next;
    int type;
    union {double value; const double *bound; const void *function;};
    void *context;

    const te_variable *lookup;
    int lookup_len;
} state;


#define TYPE_MASK(TYPE) ((TYPE)&0x0000001F)

#define IS_PURE(TYPE) (((TYPE) & TE_FLAG_PURE) != 0)
#define IS_FUNCTION(TYPE) (((TYPE) & TE_FUNCTION0) != 0)
#define IS_CLOSURE(TYPE) (((TYPE) & TE_CLOSURE0) != 0)
#define ARITY(TYPE) ( ((TYPE) & (TE_FUNCTION0 | TE_CLOSURE0)) ? ((TYPE) & 0x00000007) : 0 )
#define NEW_EXPR(type, ...) new_expr((type), (const te_expr*[]){__VA_ARGS__})

static te_expr *new_expr(const int type, const te_expr *parameters[]) {
    const int arity = ARITY(type);
    const int psize = sizeof(void*) * arity;
    const int size = (sizeof(te_expr) - sizeof(void*)) + psize + (IS_CLOSURE(type) ? sizeof(void*) : 0);
    te_expr *ret = malloc(size);
    memset(ret, 0, size);
    if (arity && parameters) {
        memcpy(ret->parameters, parameters, psize);
    }
    ret->type = type;
    ret->bound = 0;
    return ret;
}


void te_free_parameters(te_expr *n) {
    if (!n) return;
    switch (TYPE_MASK(n->type)) {
        case TE_FUNCTION7: case TE_CLOSURE7: te_free(n->parameters[6]);     /* Falls through. */
        case TE_FUNCTION6: case TE_CLOSURE6: te_free(n->parameters[5]);     /* Falls through. */
        case TE_FUNCTION5: case TE_CLOSURE5: te_free(n->parameters[4]);     /* Falls through. */
        case TE_FUNCTION4: case TE_CLOSURE4: te_free(n->parameters[3]);     /* Falls through. */
        case TE_FUNCTION3: case TE_CLOSURE3: te_free(n->parameters[2]);     /* Falls through. */
        case TE_FUNCTION2: case TE_CLOSURE2: te_free(n->parameters[1]);     /* Falls through. */
        case TE_FUNCTION1: case TE_CLOSURE1: te_free(n->parameters[0]);
    }
}


void te_free(te_expr *n) {
    if (!n) return;
    te_free_parameters(n);
    free(n);
}



static const te_variable functions[] = {
    /* must be in alphabetical order */
    {"pow", pow,      TE_FUNCTION2 | TE_FLAG_PURE, 0},
    {0, 0, 0, 0}
};

static const te_variable *find_builtin(const char *name, int len) {
    int imin = 0;
    int imax = sizeof(functions) / sizeof(te_variable) - 2;

    /*Binary search.*/
    while (imax >= imin) {
        const int i = (imin + ((imax-imin)/2));
        int c = strncmp(name, functions[i].name, len);
        if (!c) c = '\0' - functions[i].name[len];
        if (c == 0) {
            return functions + i;
        } else if (c > 0) {
            imin = i + 1;
        } else {
            imax = i - 1;
        }
    }

    return 0;
}

static const te_variable *find_lookup(const state *s, const char *name, int len) {
    int iters;
    const te_variable *var;
    if (!s->lookup) return 0;

    for (var = s->lookup, iters = s->lookup_len; iters; ++var, --iters) {
        if (strncmp(name, var->name, len) == 0 && var->name[len] == '\0') {
            return var;
        }
    }
    return 0;
}



static double add(double a, double b) {return a + b;}
static double sub(double a, double b) {return a - b;}
static double mul(double a, double b) {return a * b;}
static double divide(double a, double b) {return a / b;}
static double negate(double a) {return -a;}

void next_token(state *s) {
    s->type = TOK_NULL;

    do {

        if (!*s->next){
            s->type = TOK_END;
            return;
        }

        /* Try reading a number. */
        if ((s->next[0] >= '0' && s->next[0] <= '9') || s->next[0] == '.') {
            s->value = strtod(s->next, (char**)&s->next);
            s->type = TOK_NUMBER;
        } else {
            /* Look for a variable or builtin function call. */
            if (s->next[0] >= 'a' && s->next[0] <= 'z') {
                const char *start;
                start = s->next;
                while ((s->next[0] >= 'a' && s->next[0] <= 'z') || (s->next[0] >= '0' && s->next[0] <= '9') || (s->next[0] == '_')) s->next++;

                const te_variable *var = find_lookup(s, start, s->next - start);
                if (!var) var = find_builtin(start, s->next - start);

                if (!var) {
                    s->type = TOK_ERROR;
                } else {
                    switch(TYPE_MASK(var->type))
                    {
                        case TE_VARIABLE:
                            s->type = TOK_VARIABLE;
                            s->bound = var->address;
                            break;

                        case TE_CLOSURE0: case TE_CLOSURE1: case TE_CLOSURE2: case TE_CLOSURE3:         /* Falls through. */
                        case TE_CLOSURE4: case TE_CLOSURE5: case TE_CLOSURE6: case TE_CLOSURE7:         /* Falls through. */
                            s->context = var->context;                                                  /* Falls through. */

                        case TE_FUNCTION0: case TE_FUNCTION1: case TE_FUNCTION2: case TE_FUNCTION3:     /* Falls through. */
                        case TE_FUNCTION4: case TE_FUNCTION5: case TE_FUNCTION6: case TE_FUNCTION7:     /* Falls through. */
                            s->type = var->type;
                            s->function = var->address;
                            break;
                    }
                }

            } else {
                /* Look for an operator or special character. */
                switch (s->next++[0]) {
                    case '+': s->type = TOK_INFIX; s->function = add; break;
                    case '-': s->type = TOK_INFIX; s->function = sub; break;
                    case '*': s->type = TOK_INFIX; s->function = mul; break;
                    case '/': s->type = TOK_INFIX; s->function = divide; break;
                    case '^': s->type = TOK_INFIX; s->function = pow; break;
                    case '%': s->type = TOK_INFIX; s->function = fmod; break;
                    case ' ': case '\t': case '\n': case '\r': break;
                    default: s->type = TOK_ERROR; break;
                }
            }
        }
    } while (s->type == TOK_NULL);
}


static te_expr *list(state *s);
static te_expr *expr(state *s);
static te_expr *power(state *s);

static te_expr *base(state *s) {
    te_expr *ret;
    int arity;

    switch (TYPE_MASK(s->type)) {
        case TOK_NUMBER:
            ret = new_expr(TE_CONSTANT, 0);
            ret->value = s->value;
            next_token(s);
            break;

        case TOK_VARIABLE:
            ret = new_expr(TE_VARIABLE, 0);
            ret->bound = s->bound;
            next_token(s);
            break;

        case TE_FUNCTION0:
        case TE_CLOSURE0:
            ret = new_expr(s->type, 0);
            ret->function = s->function;
            if (IS_CLOSURE(s->type)) ret->parameters[0] = s->context;
            next_token(s);
            break;

        case TE_FUNCTION1:
        case TE_CLOSURE1:
            ret = new_expr(s->type, 0);
            ret->function = s->function;
            if (IS_CLOSURE(s->type)) ret->parameters[1] = s->context;
            next_token(s);
            ret->parameters[0] = power(s);
            break;

        case TE_FUNCTION2: case TE_FUNCTION3: case TE_FUNCTION4:
        case TE_FUNCTION5: case TE_FUNCTION6: case TE_FUNCTION7:
        case TE_CLOSURE2: case TE_CLOSURE3: case TE_CLOSURE4:
        case TE_CLOSURE5: case TE_CLOSURE6: case TE_CLOSURE7:
            arity = ARITY(s->type);

            ret = new_expr(s->type, 0);
            ret->function = s->function;
            if (IS_CLOSURE(s->type)) ret->parameters[arity] = s->context;
            next_token(s);

            s->type = TOK_ERROR;
            break;

        default:
            ret = new_expr(0, 0);
            s->type = TOK_ERROR;
            ret->value = NAN;
            break;
    }

    return ret;
}


static te_expr *power(state *s) {
    /* <power>     =    {("-" | "+")} <base> */
    int sign = 1;
    while (s->type == TOK_INFIX && (s->function == add || s->function == sub)) {
        if (s->function == sub) sign = -sign;
        next_token(s);
    }

    te_expr *ret;

    if (sign == 1) {
        ret = base(s);
    } else {
        ret = NEW_EXPR(TE_FUNCTION1 | TE_FLAG_PURE, base(s));
        ret->function = negate;
    }

    return ret;
}


static te_expr *factor(state *s) {
    /* <factor>    =    <power> {"^" <power>} */
    te_expr *ret = power(s);

    while (s->type == TOK_INFIX && (s->function == pow)) {
        te_fun2 t = s->function;
        next_token(s);
        ret = NEW_EXPR(TE_FUNCTION2 | TE_FLAG_PURE, ret, power(s));
        ret->function = t;
    }

    return ret;
}



static te_expr *term(state *s) {
    /* <term>      =    <factor> {("*" | "/" | "%") <factor>} */
    te_expr *ret = factor(s);

    while (s->type == TOK_INFIX && (s->function == mul || s->function == divide || s->function == fmod)) {
        te_fun2 t = s->function;
        next_token(s);
        ret = NEW_EXPR(TE_FUNCTION2 | TE_FLAG_PURE, ret, factor(s));
        ret->function = t;
    }

    return ret;
}


static te_expr *expr(state *s) {
    /* <expr>      =    <term> {("+" | "-") <term>} */
    te_expr *ret = term(s);

    while (s->type == TOK_INFIX && (s->function == add || s->function == sub)) {
        te_fun2 t = s->function;
        next_token(s);
        ret = NEW_EXPR(TE_FUNCTION2 | TE_FLAG_PURE, ret, term(s));
        ret->function = t;
    }

    return ret;
}


static te_expr *list(state *s) {
    /* <list>      =    <expr> {"," <expr>} */
    te_expr *ret = expr(s);
    return ret;
}


#define TE_FUN(...) ((double(*)(__VA_ARGS__))n->function)
#define M_tinyexpr(e) te_eval(n->parameters[e])


double te_eval(const te_expr *n) {
    if (!n) return NAN;

    switch(TYPE_MASK(n->type)) {
        case TE_CONSTANT: return n->value;
        case TE_VARIABLE: return *n->bound;

        case TE_FUNCTION0: case TE_FUNCTION1: case TE_FUNCTION2: case TE_FUNCTION3:
        case TE_FUNCTION4: case TE_FUNCTION5: case TE_FUNCTION6: case TE_FUNCTION7:
            switch(ARITY(n->type)) {
                case 0: return TE_FUN(void)();
                case 1: return TE_FUN(double)(M_tinyexpr(0));
                case 2: return TE_FUN(double, double)(M_tinyexpr(0), M_tinyexpr(1));
                case 3: return TE_FUN(double, double, double)(M_tinyexpr(0), M_tinyexpr(1), M_tinyexpr(2));
                case 4: return TE_FUN(double, double, double, double)(M_tinyexpr(0), M_tinyexpr(1), M_tinyexpr(2), M_tinyexpr(3));
                case 5: return TE_FUN(double, double, double, double, double)(M_tinyexpr(0), M_tinyexpr(1), M_tinyexpr(2), M_tinyexpr(3), M_tinyexpr(4));
                case 6: return TE_FUN(double, double, double, double, double, double)(M_tinyexpr(0), M_tinyexpr(1), M_tinyexpr(2), M_tinyexpr(3), M_tinyexpr(4), M_tinyexpr(5));
                case 7: return TE_FUN(double, double, double, double, double, double, double)(M_tinyexpr(0), M_tinyexpr(1), M_tinyexpr(2), M_tinyexpr(3), M_tinyexpr(4), M_tinyexpr(5), M_tinyexpr(6));
                default: return NAN;
            }

        case TE_CLOSURE0: case TE_CLOSURE1: case TE_CLOSURE2: case TE_CLOSURE3:
        case TE_CLOSURE4: case TE_CLOSURE5: case TE_CLOSURE6: case TE_CLOSURE7:
            switch(ARITY(n->type)) {
                case 0: return TE_FUN(void*)(n->parameters[0]);
                case 1: return TE_FUN(void*, double)(n->parameters[1], M_tinyexpr(0));
                case 2: return TE_FUN(void*, double, double)(n->parameters[2], M_tinyexpr(0), M_tinyexpr(1));
                case 3: return TE_FUN(void*, double, double, double)(n->parameters[3], M_tinyexpr(0), M_tinyexpr(1), M_tinyexpr(2));
                case 4: return TE_FUN(void*, double, double, double, double)(n->parameters[4], M_tinyexpr(0), M_tinyexpr(1), M_tinyexpr(2), M_tinyexpr(3));
                case 5: return TE_FUN(void*, double, double, double, double, double)(n->parameters[5], M_tinyexpr(0), M_tinyexpr(1), M_tinyexpr(2), M_tinyexpr(3), M_tinyexpr(4));
                case 6: return TE_FUN(void*, double, double, double, double, double, double)(n->parameters[6], M_tinyexpr(0), M_tinyexpr(1), M_tinyexpr(2), M_tinyexpr(3), M_tinyexpr(4), M_tinyexpr(5));
                case 7: return TE_FUN(void*, double, double, double, double, double, double, double)(n->parameters[7], M_tinyexpr(0), M_tinyexpr(1), M_tinyexpr(2), M_tinyexpr(3), M_tinyexpr(4), M_tinyexpr(5), M_tinyexpr(6));
                default: return NAN;
            }

        default: return NAN;
    }

}

#undef TE_FUN
#undef M_tinyexpr

static void optimize(te_expr *n) {
    /* Evaluates as much as possible. */
    if (n->type == TE_CONSTANT) return;
    if (n->type == TE_VARIABLE) return;

    /* Only optimize out functions flagged as pure. */
    if (IS_PURE(n->type)) {
        const int arity = ARITY(n->type);
        int known = 1;
        int i;
        for (i = 0; i < arity; ++i) {
            optimize(n->parameters[i]);
            if (((te_expr*)(n->parameters[i]))->type != TE_CONSTANT) {
                known = 0;
            }
        }
        if (known) {
            const double value = te_eval(n);
            te_free_parameters(n);
            n->type = TE_CONSTANT;
            n->value = value;
        }
    }
}


te_expr *te_compile(const char *expression, const te_variable *variables, int var_count, int *error) {
    state s;
    s.start = s.next = expression;
    s.lookup = variables;
    s.lookup_len = var_count;

    next_token(&s);
    te_expr *root = list(&s);

    if (s.type != TOK_END) {
        te_free(root);
        if (error) {
            *error = (s.next - s.start);
            if (*error == 0) *error = 1;
        }
        return 0;
    } else {
        optimize(root);
        if (error) *error = 0;
        return root;
    }
}


double te_interp(const char *expression, int *error) {
    te_expr *n = te_compile(expression, 0, 0, error);
    double ret;
    if (n) {
        ret = te_eval(n);
        te_free(n);
    } else {
        ret = NAN;
    }
    return ret;
}


//OLED
#ifdef OLED_ENABLE

bool oled_task_user(void) {
        oled_set_cursor(0,6);
    // Layer Status
    oled_write_P(PSTR("MODE\n"), false);
    oled_write_P(PSTR("\n"), false);
    
    switch (get_highest_layer(layer_state)) {
        case 0:
            oled_write_P(PSTR("BASE\n"), false);
            break;
        case 1:
            oled_write_P(PSTR("FUNC\n"), false);
            break;
        case 2:
            oled_write_P(PSTR("RGB\n"), false);
            break;
        case 3:
            oled_write_P(PSTR("CALC\n"), false);
            break;
    }
    
    if(input_count>0){
        oled_write_ln(expressions_buffer,false);
    }else{
        oled_write_ln(last_answer,false);
    }
    return false;
}
#endif

void keyboard_post_init_user(void) {
  //Customise these values to debug
  debug_enable=true;
  debug_matrix=true;
  //debug_keyboard=true;
  //debug_mouse=true;
}
