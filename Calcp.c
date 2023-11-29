#include "Calcp.h"

VMCHAR v[2000];
VMWCHAR vvv[1000];
VMWCHAR editor_title[100];
VMCHAR cmyresult[100] = "Invalid Expression !";
VMFLOAT myresult;
VMINT myresult1 = 0;

void vm_main(void) { 
    vm_ascii_to_ucs2(editor_title, 34, "Input expresion:");
    vm_input_set_editor_title((VMWSTR)editor_title);
    vm_input_text3(NULL, 2000, 4, f1);
}

void f(VMINT state, VMWSTR text) {
    vm_exit_app();
}

void f1(VMINT state, VMWSTR text) {
    vm_ucs2_to_ascii(v, wstrlen(text) + 1, text);
    myresult = parse_string(v);
    if (myresult1 != 1) {sprintf(cmyresult, "%01.2f", myresult);}
    vm_ascii_to_ucs2(vvv, 100, cmyresult);
    vm_ascii_to_ucs2(editor_title, 16, "Result:");
    vm_input_set_editor_title((VMWSTR)editor_title);
    vm_input_text3((VMWSTR)vvv, 100, 0, f);
}

//Code from: https://github.com/benhenshaw/numb

typedef struct
{
    char * start;
    int length;
    f64 value;
    u8 kind;
}
Token;

char * stream;
Token token;

bool next_token()
{
    if (*stream >= '0' && *stream <= '9')
    {
        // The next token is a number.
        token.start = stream;
        f64 whole_portion = 0.0;
        f64 fractional_portion = 0.0;
        f64 fractional_multiplier = 0.1;
        bool fractional = false;
        while ((*stream >= '0' && *stream <= '9') || *stream == '.')
        {
            if (*stream == '.')
            {
                fractional = true;
                ++stream;
            }
            else if (fractional)
            {
                fractional_portion += (*stream++ - '0') * fractional_multiplier;
                fractional_multiplier *= 0.1;
            }
            else
            {
                whole_portion *= 10.0;
                whole_portion += *stream++ - '0';
            }
        }
        token.length = stream - token.start;
        token.kind = TOKEN_NUMBER;
        token.value = whole_portion + fractional_portion;
    }
    else if (*stream == ' ' || *stream == '\t')
    {
        // The next token is whitespace, so skip it.
        while (*stream == ' ' || *stream == '\t') ++stream;
        next_token();
    }
    else if (*stream == '+' || *stream == '-' ||
             *stream == '*' || *stream == '/' ||
             *stream == '(' || *stream == ')')
    {
        // The next token is an operator.
        token.start = stream;
        token.length = 1;
        token.kind = *stream++;
    }
    else if (*stream == '\0')
    {
        // The next token is the end of the character stream.
        token.kind = TOKEN_END;
        token.start = stream;
        token.length = 1;
        return false;
    }
    else
    {
        // The next token is an unknown character.
        if (token.kind == TOKEN_BAD)
        {
            ++token.length;
        }
        else
        {
            token.kind = TOKEN_BAD;
            token.start = stream;
            token.length = 1;
        }
        return false;
    }
    return true;
}

//
// Direct parser.
//
// This parser will parse and evaluate expressions,
// producing a final numerical value.
//

bool token_is(int kind)
{
    return token.kind == kind;
}

bool match_token(int kind)
{
    if (token_is(kind))
    {
        next_token();
        return true;
    }
    else
    {
        return false;
    }
}

// Numerical values and brackets.
f64 parse_level_three()
{
    if (token_is(TOKEN_NUMBER))
    {
        f64 value = token.value;
        next_token();
        return value;
    }
    else if (match_token('('))
    {
        f64 parse_level_zero();
        f64 value = parse_level_zero();
        if (match_token(')'))
        {
            return value;
        }
        else
        {
            //printf("Invalid Expression: Expected ')'.\n");
            sprintf(cmyresult, "%s", "Invalid Expression: Expected ')'.\n");
            myresult1 = 1;
            return 0;
            exit(1);
        }
    }
    else
    {
        //printf("Invalid Expression: Expected number or '('.\n");
        sprintf(cmyresult, "%s", "Invalid Expression: Expected number or '('.\n");
        myresult1 = 1;
        return 0;
        exit(1);
    }
}

// Unary plus and minus.
f64 parse_level_two()
{
    if (match_token('-'))
    {
        return -parse_level_two();
    }
    else if (match_token('+'))
    {
        return parse_level_two();
    }
    else
    {
        return parse_level_three();
    }
}

// Multiplication and division.
f64 parse_level_one()
{
    f64 value = parse_level_two();
    while (token_is('*') || token_is('/'))
    {
        int operator = token.kind;
        next_token();
        f64 r_value = parse_level_one();
        if (operator == '*')
        {
            value *= r_value;
        }
        else if (operator == '/')
        {
            value /= r_value;
        }
        else
        {
            //printf("Invalid Expression: Expected '*' or '/'.\n");
            sprintf(cmyresult, "%s", "Invalid Expression: Expected '*' or '/'.\n");
            myresult1 = 1;
            return 0;
            exit(1);
        }
    }
    return value;
}

// Addition and subtraction.
f64 parse_level_zero()
{
    f64 value = parse_level_one();
    while (token_is('+') || token_is('-'))
    {
        int operator = token.kind;
        next_token();
        f64 r_value = parse_level_one();
        if (operator == '+')
        {
            value += r_value;
        }
        else if (operator == '-')
        {
            value -= r_value;
        }
        else
        {
            //printf("Invalid Expression: Expected '+' or '-'.\n");
            sprintf(cmyresult, "%s", "Invalid Expression: Expected '+' or '-'.\n");
            myresult1 = 1;
            return 0;
            exit(1);
        }
    }
    return value;
}

f64 parse_string(char * string)
{
    f64 result = 0;
    stream = string;
    next_token();
    result = parse_level_zero();
    return result;
}
