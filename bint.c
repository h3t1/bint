#include "bint.h"
void readn(unsigned long long n,char *msg,HWND hwnd)
{
    char buf[250];
    sprintf(buf,"%llu",n);
    MessageBoxA(hwnd,buf,msg,MB_OK);
}
Bint init()
{
    Bint n;
    n.fl=NULL;
    n.fr=NULL;
    n.len=0;
    n._dir=FALSE;
    n._signed=FALSE;
    return n;
}
BOOL is_empty(Bint n)
{
    return (n.len>0)?FALSE:TRUE;
}
Bint concat(Bint n,unsigned long long p, HWND hwnd)
{
    Node *tmp;
    tmp = (Node*)malloc(sizeof(Node));
    if(tmp!=NULL)
    {
        tmp->packet = p;
        tmp->prev = NULL;
        tmp->next = n.fl;
        if(is_empty(n))
            n.fr=tmp;
        else
            tmp->next->prev=tmp;
        n.fl=tmp;
        n.len++;
    }
    else
        MessageBox(hwnd, "Mémoire insuffisante!", "Erreur", MB_OK | MB_ICONINFORMATION);
    return n;
}
Bint calc(char* a, char* b, char op,HWND hwnd)
{
    Bint n1=init(),n2=init(), n3=init();
    n1 = parse_str(a,op,hwnd);
    n2 = parse_str(b,op,hwnd);
    switch(op)
    {
    case '+':
    {
        if(n1._signed && !n2._signed)
        {
            if(cmp(n1,n2,hwnd)>0)
            {
                n3= sub(n1,n2,hwnd);
                n3._signed=TRUE;
            }
            else
            {
                n3 = sub(n2,n1,hwnd);
                n3._signed=FALSE;
            }
        }
        else if(!n1._signed && n2._signed)
        {
            if(cmp(n1,n2,hwnd)>0)
            {
                n3= sub(n1,n2,hwnd);
                n3._signed=FALSE;
            }
            else
            {
                n3 = sub(n2,n1,hwnd);
                n3._signed=TRUE;
            }
        }
        else
            n3 = add(n1,n2,hwnd);
    }
    break;
    case '-':
        if(n1._signed && !n2._signed)
        {
            n3 = add(n1,n2,hwnd);
            n3._signed=TRUE;
        }
        else if(!n1._signed && n2._signed)
        {
            n3 = add(n1,n2,hwnd);
            n3._signed=FALSE;
        }
        else if(cmp(n1,n2,hwnd)>=0)
        {
            n3 = sub(n1,n2,hwnd);
            if(!n1._signed)
                n3._signed=FALSE;
            else
                n3._signed=TRUE;
        }
        else
        {
            n3 = sub(n2,n1,hwnd);
            if(!n1._signed)
                n3._signed=TRUE;
            else
                n3._signed=FALSE;
        }
        break;
    case '*':
        n3 = mul(n1,n2,hwnd);
        if((n1._signed && n2._signed) || (!n1._signed && !n2._signed))
            n3._signed=FALSE;
        else
            n3._signed=TRUE;
        break;
    case '/':
        if(n2.len==1 && n2.fl->packet==0)
        {
            MessageBox(hwnd,"Impossible de diviser par 0!","Erreur",MB_ICONERROR);
        }
        else if(cmp(n1,n2,hwnd)<0)
            n3 = concat(n3,0,hwnd);
        else
            n3 = dm(n1,n2,op,hwnd);
        break;
    case '%':
        n3 = dm(n1,n2,op,hwnd);
        break;
    default:
        MessageBox(hwnd,"opération non définie","Erreur",MB_ICONERROR);
    }
    return n3;
}
int cmp(Bint a,Bint b, HWND hwnd)
{
    if(a.len>b.len)
        return 1;
    else if(a.len<b.len)
        return -1;

    unsigned long long a_packet,b_packet;

    while((a.fl && a.fr))
    {
        a_packet =(a._dir)?a.fl->packet:a.fr->packet,
        b_packet =(b._dir)?b.fl->packet:b.fr->packet;
        if(a_packet > b_packet)
            return 1;
        else if(a_packet < b_packet)
            return -1;
        else
        {
            if(a._dir)
                a.fl=a.fl->next;
            else
                a.fr=a.fr->prev;
            if(b._dir)
                b.fl=b.fl->next;
            else
                b.fr=b.fr->prev;
        }
    }
    return 0;
}
Bint dm(Bint a,Bint b,char op,HWND hwnd)
{
    Bint r=init();
    size_t diff;
    unsigned long long coef = 0;
    r=concat(r,0,hwnd);
    if(a.len==1 && b.len==1)
    {
        Bint tmp =init();
        tmp = concat(tmp,(op=='/')?a.fr->packet/b.fr->packet:a.fr->packet%b.fr->packet,hwnd);
        r = add(r,tmp,hwnd);
    }
    else
    {
        while(cmp(a,b,hwnd)>=0)
        {
            coef = 0;
            diff = get_length(a) - get_length(b);
            Bint tmp = parse_str(read(b,hwnd),'*',hwnd);/* tmp=b */
            tmp = mul(rfwz(1,diff-1,'*',hwnd),tmp,hwnd);/* tmp=b*10^diff */

            while(cmp(a,tmp,hwnd)>=0)/* while a>tmp: a=a-tmp; coef++; */
            {
                a = sub(a,tmp,hwnd);
                /*MessageBoxA(hwnd,read(a,hwnd),"a",0);*/
                /*MessageBoxA(hwnd,read(tmp,hwnd),"tmp",0);*/
                coef++;
            }
            /*MessageBoxA(hwnd,read(tmp,hwnd),"tmp",0);*/
            if(op=='/')
            {
                r = add(r,rfwz(coef,diff,'+',hwnd),hwnd);/*r = r + coef*10^diff*/
            }
            /*MessageBoxA(hwnd,read(r,hwnd),"r",0);*/
        }
    }
    if(op=='%')
        r = add(r,a,hwnd);
    /*MessageBoxA(hwnd,"s",NULL,0);
      r._dir = TRUE;
        */
    return r;
}

Bint mul(Bint a,Bint b,HWND hwnd)
{
    unsigned long long res=0;
    Bint r = init(),tmp=init();
    r = concat(r,0,hwnd);
    int i=0,j=0;
    Node *n;
    n = (Node*)malloc(sizeof(Node));
    while(b.fl && b.fr)
    {
        i=j++;
        n = (a._dir)?a.fr:a.fl;

        while(n)
        {
            res = (b._dir)?b.fr->packet:b.fl->packet;
            res *= n->packet;
            tmp = rfwz(res,9*i++,'+',hwnd);
            r = add(r,tmp,hwnd);
            n = (a._dir)?n->prev:n->next;
        }
        if(b._dir)
        {
            b.fr=b.fr->prev;
        }
        else
        {
            b.fl=b.fl->next;
        }
    }
    return r;
}
Bint sub(Bint a, Bint b, HWND hwnd)
{
    char carry=0;
    unsigned long long res=0;
    Bint c = init();
    while((a.fl && a.fr) || (b.fl && b.fr))
    {
        if(!b.fl || !b.fr)
        {
            res = a._dir?a.fr->packet:a.fl->packet;
            if(res>=carry)
            {
                res -= carry;
                carry=0;
            }
            else
            {
                res = 999999999999999999ULL;
                carry=1;
            }
            c=concat(c,res,hwnd);
            if(a._dir)
            {
                a.fr=a.fr->prev;
            }
            else
            {
                a.fl=a.fl->next;
            }
        }
        else
        {
            unsigned long long n1=a._dir?a.fr->packet:a.fl->packet,
                               n2=b._dir?b.fr->packet:b.fl->packet;
            if(n1>=(n2+carry))
            {
                res = n1 - (n2 + carry);
                carry = 0;
            }
            else
            {
                res = (n1+1000000000000000000ULL) - (n2 + carry);
                carry = 1;
            }
            c=concat(c,res,hwnd);
            if(a._dir)
            {
                a.fr=a.fr->prev;
            }
            else
            {
                a.fl=a.fl->next;
            }
            if(b._dir)
            {
                b.fr=b.fr->prev;
            }
            else
            {
                b.fl=b.fl->next;
            }
        }
    }
    c._dir=TRUE;
    /*return c;
    readn(carry,"carry",hwnd);*/
    return rlz(c,hwnd);
}
Bint add(Bint a,Bint b,HWND hwnd)
{
    char carry=0;
    unsigned long long res=0;
    Bint c = init();
    while((a.fl && a.fr) || (b.fl && b.fr))
    {
        if(!a.fl || !a.fr)
        {
            res = carry;
            res += b._dir?(b.fr->packet):(b.fl->packet);
            if(res>999999999999999999ULL)
            {
                carry=1;
                res%=1000000000000000000ULL;
            }
            else
                carry=0;
            c = concat(c,res,hwnd);
            if(b._dir)
                b.fr=b.fr->prev;
            else
                b.fl=b.fl->next;
        }
        else if(!b.fl || !b.fr)
        {
            res = a._dir?(a.fr->packet)+carry:(a.fl->packet)+carry;
            if(res>999999999999999999ULL)
            {
                carry=1;
                res%=1000000000000000000ULL;
            }
            else
                carry=0;
            c = concat(c,res,hwnd);
            if(a._dir)
                a.fr=a.fr->prev;
            else
                a.fl=a.fl->next;
        }
        else
        {
            res = carry;
            res+= a._dir?(a.fr->packet):(a.fl->packet);
            res+= b._dir?(b.fr->packet):(b.fl->packet);
            if(res>999999999999999999ULL)
            {
                carry=1;
                res%=1000000000000000000ULL;
            }
            else
                carry=0;
            c=concat(c,res,hwnd);
            if(a._dir)
            {
                a.fr=a.fr->prev;
            }
            else
            {
                a.fl=a.fl->next;
            }
            if(b._dir)
            {
                b.fr=b.fr->prev;
            }
            else
            {
                b.fl=b.fl->next;
            }
        }
    }
    if(carry)
        c=concat(c,carry,hwnd);
    if(a._signed && b._signed)
        c._signed=TRUE;
    else
        c._signed=FALSE;
    c._dir=TRUE;
    return c;
}
Bint rlz(Bint n,HWND hwnd)
{
    Node * t;
    while(n.fl && n.fr)
    {
        if(n._dir && n.fl->packet==0)
        {
            t = n.fl;
            n.fl = n.fl->next;
            free(t);
        }
        else if(!n._dir && n.fr->packet==0)
        {
            t= n.fr;
            n.fr = n.fr->prev;
            free(t);
        }
        else
            break;
        n.len--;
    }
    if(n.len==0)
        n=concat(n,0,hwnd);
    return n;
}
Bint rfwz(unsigned long long ull,size_t i,char op,HWND hwnd)
{
    size_t len = snprintf(NULL,0,"%llu",ull);
    char *fwz,*n;
    fwz = (char *)malloc(sizeof(char)*(i+1));
    n = (char*)malloc(sizeof(char)*(i+len+1));
    sprintf(n,"%llu",ull);
    *(fwz+i)='\0';
    if(i!=0)
    {
        memset(fwz, '0', sizeof(char)*i);
        strcat(n,fwz);
    }
    return parse_str(n,op,hwnd);
}
Bint parse_str(char* str,char op,HWND hwnd)
{
    Bint n = init();
    if(*str=='-')
    {
        n._signed=TRUE;
        str++;
    }
    else
        n._signed=FALSE;
    while(*str=='0')
        str++;

    size_t size_str = strlen(str),
           max_ull = snprintf(NULL, 0, "%llu", ULLONG_MAX),
           size_p = (op!='*')? max_ull-2 : max_ull/2-1,
           nb_p = size_str/size_p,
           last_p = size_str%size_p;
    if(size_str==0)
        return concat(n,0,hwnd);
    char tmp[size_p];
    if(last_p!=0)
    {
        char buf[244];
        sprintf(buf,"%d",last_p);
        /*MessageBoxA(hwnd,buf,"parse_bint",NULL);*/
        strncpy(tmp,str,last_p);
        tmp[last_p]='\0';
        n=concat(n,atoll(tmp),hwnd);
    }
    size_t i=0;
    for(; i<nb_p; i++)
    {
        strncpy(tmp,str+(last_p+size_p*i),size_p);// 234 345678 2349
        tmp[size_p]='\0';
        n = concat(n,atoll(tmp),hwnd);
    }
    return n;
}
size_t get_length(Bint a)
{
    size_t len;
    len =(a._dir)? snprintf(NULL,0,"%llu",a.fl->packet):snprintf(NULL,0,"%llu",a.fr->packet);
    return len+=(a.len-1)*18;
}
//unsigned long long rmld(unsigned long long n,size_t len,HWND hwnd)
//{
// char *buf;
//buf = (char*)malloc(len*sizeof(char)+1);
//sprintf(buf,"%llu",n);
//return atoll(++buf);
//}
char * read(Bint n,HWND hwnd)
{
    Node * tmp;
    char *n_str;
    char n_tmp[20];
    size_t size_p = snprintf(NULL, 0, "%llu", ULLONG_MAX)-2;

    tmp = (Node*)malloc(sizeof(Node));
    n_str = (char*)malloc(sizeof(char)*n.len*size_p + 2);

    if(n._signed)
    {
        *n_str = '-';
        *(n_str+1)='\0';
    }
    else *n_str='\0';
    tmp = n._dir?n.fl:n.fr;
    BOOL s = FALSE;
    while(tmp)
    {
        sprintf(n_tmp,"%llu",tmp->packet);
        char diff = size_p-strlen(n_tmp);
        if(diff && s)
        {
            char *zero;
            zero = (char*)malloc(sizeof(char)*(size_p + 1));
            *zero='\0';
            while(diff--)
                strcat(zero,"0");
            strcat(zero,n_tmp);
            strcat(n_str,zero);
        }
        else
            strcat(n_str,n_tmp);
        s=TRUE;
        tmp = n._dir?tmp->next:tmp->prev;
    }
    return n_str;
}
