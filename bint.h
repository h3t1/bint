#ifndef BINT_H_INCLUDED
#define BINT_H_INCLUDED
#include <windows.h>
#include <stdio.h>
#include <limits.h>
typedef struct node
{
    unsigned long long packet;
    struct node *next,*prev;
} Node;
typedef struct bint
{
    Node *fl, *fr;
    unsigned short len;
    BOOL _signed;
    BOOL _dir;
} Bint;

Bint init();
BOOL is_empty(Bint);
Bint parse_str(char*, char, HWND);
Bint concat(Bint, unsigned long long, HWND);
Bint calc(char*, char*, char,HWND);
Bint add(Bint, Bint, HWND);
Bint sub(Bint, Bint, HWND);
Bint mul(Bint, Bint, HWND);
Bint dm(Bint, Bint ,char op, HWND);/* DivisionModulo */
Bint rlz(Bint,HWND);/* Remove left zeros */
char * read(Bint, HWND);
int cmp(Bint, Bint, HWND);
size_t get_length(Bint);
Bint rfwz(unsigned long long, size_t, char op, HWND);/* Right Fill With Zero */

#endif // BINT_H_INCLUDED
