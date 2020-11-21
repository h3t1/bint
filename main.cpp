#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif
#include "resource.h"
#include "bint.h"
#include "bint.c"
/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SubProc (HWND, UINT, WPARAM, LPARAM) ;
WNDPROC SubbedProc;
WNDPROC	oldEditWndProc;
Bint n = init();
/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");
int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG msg;            /* Here messages to the application are saved */
    BOOL bRet;          // for return value of GetMessage
    HACCEL haccel;      // handle to accelerator table
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_BINT));
    wincl.hIconSm = (HICON)LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_BINT), IMAGE_ICON, 32, 32, 0);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND+7;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               0,                   /* Extended possibilites for variation */
               szClassName,         /* Classname */
               _T("BINT"),       /* Title Text */
               WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, /* default window */
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               544,                 /* The programs width */
               375,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);
    UpdateWindow(hwnd);
    // Load the accelerator table.

    haccel = LoadAccelerators(hThisInstance, "ACC");


    /* Run the message loop. It will run until GetMessage() returns 0 */
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (!TranslateAccelerator(
                    hwnd,  // handle to receiving window
                    haccel,    // handle to active accelerator table
                    &msg) && !IsDialogMessage(hwnd,&msg))         // message data
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return msg.wParam;
}
BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch(Message)
    {
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDOK:
            EndDialog(hwnd, IDOK);
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}
bool IsValidNumber(const char* pszText, bool allowMinus)
{
    if(allowMinus && ('-' == *pszText))
        ++pszText;
    while(*pszText != '\0')
    {
        if((*pszText)<'0' || (*pszText)>'9')
            return false;
        ++pszText;
    }
    return true;
}

void HandlePaste(HWND hwnd)
{
    if(OpenClipboard(hwnd))
    {
        HANDLE hData = GetClipboardData(CF_TEXT);
        if(NULL != hData)
        {
            LPSTR pszText = (LPSTR)GlobalLock(hData);

            if(NULL != pszText)
            {
                bool doPaste      = true;
                bool allowMinus = true;

                DWORD from = 0;
                DWORD to   = 0;
                SendMessageA(hwnd, EM_GETSEL, (WPARAM)&from, (WPARAM)&to);
                if(0 == from)
                {
                    if(0 == to)
                    {
                        char buffer[2] = ""; // big enough for one char plus term null
                        SendMessageA(hwnd, WM_GETTEXT, (WPARAM)2, (WPARAM)buffer);
                        if(buffer[0] == '-')
                            doPaste = false;
                    }
                }
                else
                {
                    allowMinus = false;
                }
                if(doPaste)
                {
                    doPaste = IsValidNumber(pszText, allowMinus);
                }

                if(doPaste)
                {
                    SendMessageA(hwnd, EM_REPLACESEL, (WPARAM)TRUE, (WPARAM)pszText);
                }
            }
            GlobalUnlock(hData);
        }

        CloseClipboard();
    }
}

LRESULT CALLBACK SubProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message == WM_CHAR)
    {
        if(0x8000 & GetKeyState(VK_CONTROL))
        {
            switch(wParam)
            {
            case 0x16: // ctrl-V
                HandlePaste(hwnd);
                return 0;
                break;
            case 0x03: // ctrl-C
            case 0x18: // ctrl-X
            case 0x19: // ctrl-Y
            case 0x1A: // ctrl-Z
                break;
            default:
                return 0;
            }
        }
        else
        {
            // Only check non-digits
            if(wParam < '0' || wParam > '9')
            {
                switch(wParam)
                {
                case _T('-'):
                {
                    DWORD from = 0;
                    DWORD to   = 0;
                    SendMessage(hwnd, EM_GETSEL, (WPARAM)&from, (WPARAM)&to);
                    if(0 == from)
                    {
                        // if to is greater than zero, we're replacing the 0th
                        // char so we don't need to worry. If we're inserting
                        // have to check the first char isn't already a -
                        if(0 == to)
                        {
                            TCHAR buffer[2] = _T(""); // big enough for one char plus term null
                            SendMessage(hwnd, WM_GETTEXT, (WPARAM)2, (WPARAM)buffer);
                            if(buffer[0] == _T('-'))
                                return 0;
                        }
                        // allow through
                    }
                    else return 0;
                }
                case VK_BACK:
                    // allow backspace through
                break;
                default:
                    // block everything else
                    return 0;
                }
            }
        }
    }
    return CallWindowProc (SubbedProc, hwnd, message, wParam, lParam) ;
}
LRESULT CALLBACK MultiLineEditWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = CallWindowProc(oldEditWndProc, hwnd, message, wParam, lParam);
    switch (message)
    {
    case WM_GETDLGCODE:
        res &= ~DLGC_WANTALLKEYS;
    }
    return res;
}
/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND a,op,b,eq,r;
    switch (message)                  /* handle the messages */
    {
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDC_ENTER:
        {
        HWND tmp=GetFocus();
            if(tmp!=b && tmp!=eq)
            break;
        }
        case IDC_EQ:
            {
            char a_str[255],b_str[255],op_str[2];
            char res[255]={'0'};
            res[255]='\0';
            GetWindowText(a,a_str,255);
            GetWindowText(b,b_str,255);
            GetWindowText(op,op_str,2);
            Bint n=init();
            n=calc(a_str,b_str,op_str[0],hwnd);

             SetWindowText(r,read(n,hwnd));
            }
            break;
        case IDM_SELECT_ALL:
            SendMessage (GetFocus(), EM_SETSEL, 0, -1);
            break;
        case IDM_PASTE:
            SendMessage (GetFocus(), WM_PASTE, 0, 0);
            break;
        case IDM_CLOSE:
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        case ID_HELP_ABOUT:
        {
            int ret = DialogBox(GetModuleHandle(NULL),MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutDlgProc);
            if(ret == IDOK);
                //MessageBox(hwnd, "Dialog exited with IDOK.", "Notice", MB_OK | MB_ICONINFORMATION);
            else if(ret == -1)
                MessageBox(hwnd, "Dialog failed!", "Error", MB_OK | MB_ICONINFORMATION);
        }
        }
        break;
    case WM_CREATE:
    {
        int i=0;
        a = CreateWindowEx(
                0, "EDIT",   // predefined class
                NULL,         // no window title
                WS_CHILD | WS_VISIBLE | WS_TABSTOP| WS_GROUP |
                ES_LEFT  | ES_AUTOVSCROLL | ES_MULTILINE,
                10, 10, 508, 64,   // set size in WM_SIZE message
                hwnd,         // parent window
                (HMENU) IDC_A,   // edit control ID
                (HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE),
                NULL);
        op = CreateWindow(
                 "COMBOBOX",
                 NULL,
                 WS_VISIBLE | WS_CHILD  | CBS_DROPDOWNLIST | WS_TABSTOP,
                 10, 84,80,320,
                 hwnd,
                 (HMENU)IDC_OP,
                 (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
                 NULL);
        SendMessage(op, CB_ADDSTRING, 0, (LPARAM)"+");
        SendMessage(op, CB_ADDSTRING, 0, (LPARAM)"-");
        SendMessage(op, CB_ADDSTRING, 0, (LPARAM)"*");
        SendMessage(op, CB_ADDSTRING, 0, (LPARAM)"/");
        SendMessage(op, CB_ADDSTRING, 0, (LPARAM)"%");
        b = CreateWindowEx(
                0, "EDIT",   // predefined class
                NULL,         // no window title
                WS_CHILD | WS_VISIBLE | WS_TABSTOP| ES_MULTILINE |
                ES_LEFT | ES_AUTOVSCROLL,
                10, 120, 508, 64,   // set size in WM_SIZE message
                hwnd,         // parent window
                (HMENU) IDC_B,   // edit control ID
                (HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE),
                NULL);
        eq = CreateWindowEx(
                 0, "BUTTON",   // predefined class
                 "=",         // no window title
                 WS_CHILD | WS_VISIBLE | WS_TABSTOP |
                 ES_CENTER,
                 10, 194, 80, 32,   // set size in WM_SIZE message
                 hwnd,         // parent window
                 (HMENU) IDC_EQ,   // edit control ID
                 (HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE),
                 NULL);
        r = CreateWindowEx(
                0, "EDIT",
                NULL,
                WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL |ES_READONLY,
                10, 236, 508, 64,   // set size in WM_SIZE message
                hwnd,         // parent window
                (HMENU) IDC_R,   // edit control ID
                (HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE),
                NULL);
        SendMessage(op, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
        /** SUBCLASSING **/
        SubbedProc = (WNDPROC)SetWindowLong(a, GWL_WNDPROC, (LONG) SubProc);
        SubbedProc = (WNDPROC)SetWindowLong(b, GWL_WNDPROC, (LONG) SubProc);
        oldEditWndProc = (WNDPROC)SetWindowLong( a, GWL_WNDPROC, (LONG) MultiLineEditWndProc );
        oldEditWndProc = (WNDPROC)SetWindowLong( b, GWL_WNDPROC, (LONG) MultiLineEditWndProc );
    }
    break;
    case WM_SETFOCUS:
        SetFocus(a);
        return 0;
    case WM_ERASEBKGND :
    {
        HDC dc; /* Standard Device Context; used to do the painting */
        /* rect = Client Rect of the window;
        Temp = Temparory rect tangle for the color bands */
        RECT rect, temp;
        HBRUSH color; /* A brush to do the painting with */
        /* Get the dc for the wnd */
        dc = GetDC(hwnd);
        /* Get the client rect */
        GetClientRect(hwnd, &rect);
        /* Change the R,G,B values
        to the color of your choice */
        int r1 = 123, g1 = 67, b1 = 151;
        int r2 = 242, g2 = 148, b2 = 146;

        /* loop to create the gradient */
        for(int i=0; i<rect.right; i++)
        {
            /* Color ref. for the gradient */
            int r,g,b;
            /* Determine the colors */
            r = r1 + (i * (r2-r1) / rect.right);
            g = g1 + (i * (g2-g1) / rect.right);
            b = b1 + (i * (b2-b1) / rect.right);
            /* Fill in the rectangle information */
            /* The uper left point of the rectangle
            being painted; uses i as the starting point*/
            temp.left = i;
            /* Upeer Y cord. Always start at the top */
            temp.top = 0;
            /* Okay heres the key part,
            create a rectangle thats 1 pixel wide */
            temp.right = i + 1;
            /* Height of the rectangle */
            temp.bottom = rect.bottom;
            /* Create a brush to draw with;
            these colors are randomized */
            color = CreateSolidBrush(RGB(r, g, b));
            /* Finally fill in the rectangle */
            FillRect(dc, &temp, color);
        }
    }
    break;
    case WM_DESTROY:
        PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}
