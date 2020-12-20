// App.cpp : Определяет точку входа для приложения.
//
#define _CRT_SECURE_NO_WARNINGS
#include "framework.h"
#include "App.h"

#define MAX_LOADSTRING 100
#define IDC_LISTVIEW 1111
#define WM_FIND_BTN 10000
#define WM_DELETE_BTN 10001
#define WM_REFRESH_BTN 10002
#define WM_EDIT_BTN 10003
#define WM_ADD_BTN 10004

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];  
HINSTANCE hInsForChildWind;// имя класса главного окна

HWND hEdit;
HWND hEdits[8] = { 0 };

TCHAR  selectedComboBoxItem[256] = L"Phone number";
HWND hWndListView;
const std::vector<LPCWSTR> columnsNames{ L"Phone number", L"Surname", L"Name", L"Father's name", L"Street", L"House", L"Block", L"Flat" };
Record chosenRecord;
bool editFlag = false;


HMODULE hmd = LoadLibrary(L"DatabaseCore.dll");

typedef vector<Record*>(*loadDB)();
loadDB loadDatabase = (loadDB)GetProcAddress(hmd, FORM_FUNCTION);
loadDB getAllRecords = (loadDB)GetProcAddress(hmd, GET_ALL_FUNCTION);
typedef void(*change)(Record*,Record*, int);
change changeRecord = (change)GetProcAddress(hmd, CHANGE_FUNCTION);

typedef vector<Record*>(*srch)(std::string, int);
srch search = (srch)GetProcAddress(hmd, SEARCH_FUNCTION);

typedef void(*del)(Record*);
del deleteRecord = (del)GetProcAddress(hmd, DELETE_FUNCTION);

typedef bool(*add)(Record*);
add addRecord = (add)GetProcAddress(hmd, ADD_FUNCTION);

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LPWSTR ConvertToLPWSTR(std::string s);
VOID InsertListViewItems(HWND hWndListView, std::vector<Record*> records);
VOID InitListViewColumns(HWND hWndListView);
HWND CreateListView(HWND hWndParent);
LRESULT CALLBACK editWindowProc(HWND, UINT, WPARAM, LPARAM);
Record getRecordFromListView(int);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_APP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_APP));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_APP);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    vector<Record*> data = loadDatabase();
    hWndListView = CreateListView(hWnd);
    InitListViewColumns(hWndListView);
    InsertListViewItems(hWndListView, data);
    ShowWindow(hWndListView, SW_SHOWDEFAULT);

    HWND hWndComboBox = CreateWindow(
        WC_COMBOBOX, TEXT(""),
        CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        220,
        10,
        200,
        200,
        hWnd,
        NULL,
        GetModuleHandle(NULL),
        NULL);
    ShowWindow(hWndComboBox, SW_SHOWDEFAULT);

    TCHAR A[16];
    wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Surname"));
    SendMessage(hWndComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);

    wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Street"));
    SendMessage(hWndComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);

    wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Phone number"));
    SendMessage(hWndComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);

    SendMessage(hWndComboBox, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);

    hEdit = CreateWindow(
        L"Edit",
        NULL,
        WS_BORDER | WS_CHILD | WS_VISIBLE | NULL | NULL,
        10,
        10,
        200,
        20,
        hWnd,
        NULL,
        NULL,
        0);
    int i = 0;
    for (LPCWSTR str : columnsNames) {
        hEdits[i] = CreateWindow(
            L"Edit",
            NULL,
            WS_BORDER | WS_CHILD | WS_VISIBLE | NULL | NULL,
            870,
            40+30 * i,
            100,
            20,
            hWnd,
            NULL,
            NULL,
            0);
        HWND hwnd_st_u = CreateWindow(L"static", str,
            WS_CHILD | WS_VISIBLE | WS_TABSTOP,
            760, 40+ 30 * i, 100, 20,
            hWnd, (HMENU)(501),
            hInst, NULL);
        //SetWindowText(hwnd_st_u, L"User:");
        i++;
    }
    const LPCWSTR names[] = { L"Find",L"Delete",L"Refresh" ,L"Edit" };
    for (int i = 0; i < 4; i++) {
        HWND hwndRefreshButton = CreateWindow(
            L"BUTTON",
            names[i],
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            490+100*i,
            10,
            60,
            20,
            hWnd,
            (HMENU)(WM_FIND_BTN+i),
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);
    }
    HWND hwndEditButton = CreateWindow(
        L"BUTTON",
        L"OK",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        865,
        280,
        40,
        20,
        hWnd,
        (HMENU)WM_ADD_BTN,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int hWmId = HIWORD(wParam);
        switch (hWmId) {
        case CBN_SELCHANGE:
            int ItemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
            SendMessage((HWND)lParam, (UINT)CB_GETLBTEXT, (WPARAM)ItemIndex, (LPARAM)selectedComboBoxItem);
            break;
        }
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case WM_FIND_BTN: {

            LPWSTR ptr = ConvertToLPWSTR("");
            GetWindowText(hEdit, ptr, 255);
          //  SendMessage(hEdit, WM_GETTEXT, (WPARAM)255, (LPARAM)ptr);

            char buffer[500];
            wcstombs(buffer, ptr, 500);
            std::string str = std::string(buffer);
            if (str.size() == 0)
                break;
            ListView_DeleteAllItems(hWndListView);
            std::vector<Record*> result;

            if (lstrcmpW((LPCWSTR)selectedComboBoxItem, L"Phone number") == 0) {
                  result = search(str,PHONE);
            }

            if (lstrcmpW((LPCWSTR)selectedComboBoxItem, L"Street") == 0) {
                  result = search(str,ADDRESS);
            }

            if (lstrcmpW((LPCWSTR)selectedComboBoxItem, L"Surname") == 0) {
                 result = search(str, SURNAME);
            }

            InsertListViewItems(hWndListView, result);
        }
                        break;
        case WM_ADD_BTN: {
            bool allParamsTyped = true;
            vector<std::string> params;
            for (HWND hEd : hEdits) {
                LPWSTR ptr = ConvertToLPWSTR("");
                GetWindowText(hEd, ptr, 255);
               // SendMessage(hEd, WM_GETTEXT, (WPARAM)255, (LPARAM)ptr);

                char buffer[500];

                wcstombs(buffer, ptr, 500);
                std::string param = std::string(buffer);
                if (param.size() == 0) {
                    MessageBox(hWnd, L"Please enter all parameters for record", L"Error", MB_OK | MB_ICONINFORMATION);
                    allParamsTyped = false;
                    break;
                }
                params.push_back(param);
            }
            if (!allParamsTyped)
                break;
            Record* newRecord = new Record(params[0],
                params[1],
                params[2],
                params[3],
                params[4],
                stoi(params[5]),
                stoi(params[6]),
                stoi(params[7])
                );
            if (editFlag) {
                int k = 0;
                if (chosenRecord.surname.compare(newRecord->surname))
                    k |= SURNAME;
                if (chosenRecord.street.compare(newRecord->street))
                    k |= ADDRESS;
                if (chosenRecord.phoneNumber.compare(newRecord->phoneNumber))
                    k |= PHONE;
                changeRecord(&chosenRecord, newRecord, k);///////////
                for (HWND hEd : hEdits) {
                    SetWindowText(hEd, L"");
                }
                editFlag = false;
            }
            else {
                if (!addRecord(newRecord)) {
                    MessageBox(hWnd, L"Phone number already exists in database", L"Error", MB_OK | MB_ICONINFORMATION);
                    break;
                }
                LVITEM lvI;
                lvI.pszText = LPSTR_TEXTCALLBACK;
                lvI.mask = LVIF_TEXT;
                lvI.iSubItem = 0;
                lvI.iItem = 0;
                ListView_InsertItem(hWndListView, &lvI);
                for (int i = 0; i < 8; i++) {
                    ListView_SetItemText(hWndListView, 0, i, ConvertToLPWSTR(params[i]));
                }
            }
        }
            break;
        case WM_REFRESH_BTN: {
            ListView_DeleteAllItems(hWndListView);
            vector<Record*> records = getAllRecords();
            InsertListViewItems(hWndListView, records);
        }
            break;
        case WM_EDIT_BTN: {
            int itemIndex = ListView_GetNextItem(hWndListView, -1, LVNI_SELECTED);
            if (itemIndex == -1) {
                MessageBox(hWnd, L"Choose record in list view", L"Error", MB_OK | MB_ICONINFORMATION);
                break;
            }
            chosenRecord = getRecordFromListView(itemIndex);
            editFlag = true;
            SetWindowText(hEdits[0], wstring(chosenRecord.phoneNumber.begin(), chosenRecord.phoneNumber.end()).c_str());
            SetWindowText(hEdits[1], wstring(chosenRecord.surname.begin(), chosenRecord.surname.end()).c_str());
            SetWindowText(hEdits[2], wstring(chosenRecord.name.begin(), chosenRecord.name.end()).c_str());
            SetWindowText(hEdits[3], wstring(chosenRecord.fathersName.begin(), chosenRecord.fathersName.end()).c_str());
            SetWindowText(hEdits[4], wstring(chosenRecord.street.begin(), chosenRecord.street.end()).c_str());
            SetWindowText(hEdits[5], to_wstring(chosenRecord.houseNumber).c_str());
            SetWindowText(hEdits[6], to_wstring(chosenRecord.blockNumber).c_str());
            SetWindowText(hEdits[7], to_wstring(chosenRecord.flatNumber).c_str());
        }
            break;
        case WM_DELETE_BTN: {
            int itemIndex = ListView_GetNextItem(hWndListView, -1, LVNI_SELECTED);
            if (itemIndex == -1) {
                MessageBox(hWnd, L"Please choose item", L"Error", MB_OK | MB_ICONINFORMATION);
                break;
            }
            Record r = getRecordFromListView(itemIndex);
            deleteRecord(&r);
            ListView_DeleteItem(hWndListView, itemIndex);
        }
            break;
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

LPWSTR ConvertToLPWSTR(std::string s)
{
    const char* str = s.c_str();
    wchar_t wtext[255];
    std::mbstowcs(wtext, str, s.size() + 1);
    LPWSTR ptr = wtext;
    return ptr;
}

VOID InsertListViewItems(HWND hWndListView, std::vector<Record*> records) {
    ListView_DeleteAllItems(hWndListView);
    LVITEM lvI;

    lvI.pszText = LPSTR_TEXTCALLBACK;
    lvI.mask = LVIF_TEXT;
    lvI.iSubItem = 0;
    for (int i = 0; i < records.size(); i++) {
        lvI.iItem = i;

        ListView_InsertItem(hWndListView, &lvI);

        ListView_SetItemText(hWndListView, i, 0, ConvertToLPWSTR(records[i]->phoneNumber));
        ListView_SetItemText(hWndListView, i, 1, ConvertToLPWSTR(records[i]->surname));
        ListView_SetItemText(hWndListView, i, 2, ConvertToLPWSTR(records[i]->name));
        ListView_SetItemText(hWndListView, i, 3, ConvertToLPWSTR(records[i]->fathersName));
        ListView_SetItemText(hWndListView, i, 4, ConvertToLPWSTR(records[i]->street));
        ListView_SetItemText(hWndListView, i, 5, ConvertToLPWSTR(std::to_string(records[i]->houseNumber)));
        ListView_SetItemText(hWndListView, i, 6, ConvertToLPWSTR(std::to_string(records[i]->blockNumber)));
        ListView_SetItemText(hWndListView, i, 7, ConvertToLPWSTR(std::to_string(records[i]->flatNumber)));
    }
}

VOID InitListViewColumns(HWND hWndListView)
{
    RECT rcListView;
    LVCOLUMN lvc;

    GetClientRect(hWndListView, &rcListView);
    lvc.mask = LVCF_WIDTH | LVCF_TEXT;
    lvc.cx = 80;
    for (int i = 0; i < columnsNames.size(); i++) {
        lvc.pszText = const_cast<LPWSTR>(columnsNames[i]);
        ListView_InsertColumn(hWndListView, i, &lvc);
    }
}

HWND CreateListView(HWND hWndParent)
{
    HWND hWndListView;
    INITCOMMONCONTROLSEX icex;
    RECT rcClient;

    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);
    GetClientRect(hWndParent, &rcClient);

    hWndListView = CreateWindowEx(NULL, WC_LISTVIEW, L"", WS_CHILD | LVS_REPORT | LVS_EDITLABELS, 0, 40, 640, 190, hWndParent, (HMENU)IDC_LISTVIEW, GetModuleHandle(NULL), NULL);
    return hWndListView;//rcClient.right - rcClient.left
}

Record getRecordFromListView(int itemIndex) {
    LPWSTR ptr = ConvertToLPWSTR("");

    vector < std::string> values;
    for (int i = 0; i < 8; i++) {
        ListView_GetItemText(hWndListView, itemIndex, i, ptr, 256);
        char buffer[256];
        int ret;
        ret = wcstombs(buffer, ptr, sizeof(buffer));
        std::string s = string(buffer);
        values.push_back(s);
    }
    return Record(values[0],
        values[1],
        values[2],
        values[3],
        values[4],
        stoi(values[5]),
        stoi(values[6]),
        stoi(values[7])
    );
}