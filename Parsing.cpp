#include <stdio.h>
#include <stdlib.h>

#include "Parsing.h"

void syntax_assert(bool flag, const char *nameFunc)
{
    if(!flag)
    {
        printf("\"%s\" error: %s", nameFunc, s + p);
        abort();
    }
}

void get_G(Tree* tree, char** buf)
{

    get_E();
    syn_assert(*buf == '\0', "get_G");
    return val;
}

int get_E()
{
    int val = get_T();
    while(s[p] == '+' || s[p] == '-')
    {
        char op = s[p];
        p++;
        int val1 = get_T();

        switch(op)
        {
            case '+':
                val += val1;
                break;
            case '-':
                val -= val1;
                break;
            default:
                syn_assert(false, "get_E");
                break;
        }
    }
    return val;
}

int get_T()
{
    int val = get_P();
    while(s[p] == '*' || s[p] == '/')
    {
        char op = s[p];
        p++;
        int val1 = get_P();

        switch(op)
        {
            case '*':
                val *= val1;
                break;
            case '/':
                val /= val1;
                break;
            default:
                syn_assert(false, "get_T");
                break;
        }
    }
    return val;
}

int get_P((char** buf)
{
    if(s[p] == '(')
    {
        p++;
        int val = get_E();
        syn_assert(s[p] == ')', "get_P");
        p++;
        return val;
    }
    return get_N();
}

 get_N(char** buf)
{
    int val = 0;
    int oldBuf = *buf;
    while('0' <= **buf && **buf <= '9')
    {
        val = val * 10 + **buf - '0';
        *buf++;
    }

    syn_assert(oldBuf < p, "get_N");
    return val;
}
