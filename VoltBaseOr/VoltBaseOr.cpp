#include <windows.h>
#include <string>
#include <fstream>

#define ID_LOGIN_BUTTON 1
#define ID_REGISTER_BUTTON 2
#define ID_LOGIN_USERNAME 3
#define ID_LOGIN_PASSWORD 4
#define ID_REGISTER_USERNAME 5
#define ID_REGISTER_PASSWORD 6

const wchar_t g_szLoginClassName[] = L"loginWindowClass";
const wchar_t g_szHomeClassName[] = L"homeWindowClass";
const wchar_t g_szDbFileName[] = L"userdb.txt"; // DataBase File

LRESULT CALLBACK LoginWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HomeWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { 0 };
    HWND hwnd;
    MSG msg;

    // Register login window class
    wc.lpfnWndProc = LoginWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = g_szLoginClassName;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClassW(&wc)) {
        MessageBoxW(NULL, L"Login Window Class Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Register home window class
    wc.lpfnWndProc = HomeWndProc;
    wc.lpszClassName = g_szHomeClassName;

    if (!RegisterClassW(&wc)) {
        MessageBoxW(NULL, L"Home Window Class Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create the login window
    hwnd = CreateWindowExW(
        0,
        g_szLoginClassName,
        L"Login Screen",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        MessageBoxW(NULL, L"Login Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Main message loop
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK LoginWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hLoginUsername, hLoginPassword, hRegisterUsername, hRegisterPassword;
    static HWND hLoginButton, hRegisterButton;
    static bool showingLogin = true;

    switch (msg) {
    case WM_CREATE:
        // Create login controls
        CreateWindowW(L"STATIC", L"Username:", WS_VISIBLE | WS_CHILD, 10, 10, 80, 25, hwnd, NULL, NULL, NULL);
        hLoginUsername = CreateWindowW(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 10, 150, 25, hwnd, (HMENU)ID_LOGIN_USERNAME, NULL, NULL);
        CreateWindowW(L"STATIC", L"Password:", WS_VISIBLE | WS_CHILD, 10, 45, 80, 25, hwnd, NULL, NULL, NULL);
        hLoginPassword = CreateWindowW(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD, 100, 45, 150, 25, hwnd, (HMENU)ID_LOGIN_PASSWORD, NULL, NULL);
        hLoginButton = CreateWindowW(L"BUTTON", L"Login", WS_VISIBLE | WS_CHILD, 10, 80, 80, 25, hwnd, (HMENU)ID_LOGIN_BUTTON, NULL, NULL);
        hRegisterButton = CreateWindowW(L"BUTTON", L"Register", WS_VISIBLE | WS_CHILD, 100, 80, 80, 25, hwnd, (HMENU)ID_REGISTER_BUTTON, NULL, NULL);

        // Create register controls
        CreateWindowW(L"STATIC", L"Username:", WS_VISIBLE | WS_CHILD, 10, 10, 80, 25, hwnd, NULL, NULL, NULL);
        hRegisterUsername = CreateWindowW(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 10, 150, 25, hwnd, (HMENU)ID_REGISTER_USERNAME, NULL, NULL);
        CreateWindowW(L"STATIC", L"Password:", WS_VISIBLE | WS_CHILD, 10, 45, 80, 25, hwnd, NULL, NULL, NULL);
        hRegisterPassword = CreateWindowW(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD, 100, 45, 150, 25, hwnd, (HMENU)ID_REGISTER_PASSWORD, NULL, NULL);
        hRegisterButton = CreateWindowW(L"BUTTON", L"Register", WS_VISIBLE | WS_CHILD, 10, 80, 80, 25, hwnd, (HMENU)ID_REGISTER_BUTTON, NULL, NULL);
        ShowWindow(hRegisterUsername, SW_HIDE);
        ShowWindow(hRegisterPassword, SW_HIDE);
        ShowWindow(hRegisterButton, SW_HIDE);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == ID_LOGIN_BUTTON) {
            // Handle login
            wchar_t username[256], password[256];
            GetWindowTextW(hLoginUsername, username, 256);
            GetWindowTextW(hLoginPassword, password, 256);

            std::wifstream file(g_szDbFileName);
            std::wstring storedUser, storedPass;
            bool loginSuccessful = false;

            while (std::getline(file, storedUser)) {
                std::getline(file, storedPass);
                if (storedUser == username && storedPass == password) {
                    loginSuccessful = true;
                    break;
                }
            }
            file.close();

            if (loginSuccessful) {
                // Create and show the home screen window
                HWND hHomeWnd = CreateWindowExW(
                    0,
                    g_szHomeClassName,
                    L"Home Screen",
                    WS_OVERLAPPEDWINDOW,
                    CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
                    NULL,
                    NULL,
                    (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                    NULL
                );

                if (hHomeWnd == NULL) {
                    MessageBoxW(hwnd, L"Home Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
                }
                else {
                    ShowWindow(hHomeWnd, SW_SHOW);
                    UpdateWindow(hHomeWnd);
                    ShowWindow(hwnd, SW_HIDE); // Hide login window
                }
            }
            else {
                MessageBoxW(hwnd, L"Invalid username or password.", L"Error", MB_ICONEXCLAMATION | MB_OK);
            }
        }
        else if (LOWORD(wParam) == ID_REGISTER_BUTTON) {
            // Handle registration
            if (showingLogin) {
                ShowWindow(hLoginUsername, SW_HIDE);
                ShowWindow(hLoginPassword, SW_HIDE);
                ShowWindow(hLoginButton, SW_HIDE);
                ShowWindow(hRegisterUsername, SW_SHOW);
                ShowWindow(hRegisterPassword, SW_SHOW);
                ShowWindow(hRegisterButton, SW_SHOW);
                showingLogin = false;
            }
            else {
                wchar_t username[256], password[256];
                GetWindowTextW(hRegisterUsername, username, 256);
                GetWindowTextW(hRegisterPassword, password, 256);

                std::wofstream file(g_szDbFileName, std::ios::app);
                file << username << std::endl << password << std::endl;
                file.close();

                MessageBoxW(hwnd, L"Registration Successful. You can now log in.", L"Info", MB_OK);

                ShowWindow(hRegisterUsername, SW_HIDE);
                ShowWindow(hRegisterPassword, SW_HIDE);
                ShowWindow(hRegisterButton, SW_HIDE);
                ShowWindow(hLoginUsername, SW_SHOW);
                ShowWindow(hLoginPassword, SW_SHOW);
                ShowWindow(hLoginButton, SW_SHOW);
                showingLogin = true;
            }
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK HomeWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        CreateWindowW(L"STATIC", L"Welcome to the Home Screen", WS_VISIBLE | WS_CHILD, 10, 10, 200, 25, hwnd, NULL, NULL, NULL);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}
