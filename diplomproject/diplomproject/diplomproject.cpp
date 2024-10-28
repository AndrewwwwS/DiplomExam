#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <gdipluscolor.h>
#include <Gdipluspixelformats.h>
#pragma comment(lib, "gdiplus.lib")


using namespace Gdiplus;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProc1(HWND hwnd1, UINT uMsg, WPARAM wParam, LPARAM lParam);

HBRUSH hBrushBackground;
HBRUSH hBrushTopPanel;

const std::wstring ICON_PATH_NORMAL = L"C:\\Users\\Admin\\Desktop\\585831.png"; // Закрытие
const std::wstring ICON_PATH_HOVER = L"C:\\Users\\Admin\\Desktop\\58583.png"; // Закрытие
const std::wstring ICON_PATH_TRASH = L"C:\\Users\\Admin\\Desktop\\icons8-trash-96.png"; // Мусорка
const std::wstring ICON_PATH_COPY = L"C:\\Users\\Admin\\Desktop\\icons8-copy-96.png"; // Копирка
const std::wstring ICON_PATH_START = L"C:\\Users\\Admin\\Desktop\\icons8-start-96.png"; // Старт
const std::wstring WINDOW_TEXT = L"Error fixer";

int firstcycle = 1;
const int CLOSE_BUTTON_SIZE = 30;
const int TOP_PANEL_HEIGHT = 0;
RECT closeButtonRect;
bool isMouseOverCloseButton = false;
bool isMouseInWindow = false;
bool isInputCleared = false;
bool isCtrlPressed = false;
int currentFontSize = 20;
Image* iconNormal = nullptr;
Image* iconHover = nullptr;
Image* iconTrash = nullptr;
Image* iconCopy = nullptr;
Image* iconStart = nullptr;

HWND hwnd = NULL;
HWND hEditInput;
HWND hStaticOutput1;
HWND hStaticOutput2;

HFONT hFontSegoeUI;

int mouseX = 0, mouseY = 0;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc1 = {};
    wc1.lpfnWndProc = WindowProc1;
    wc1.hInstance = hInstance;
    hBrushTopPanel = CreateSolidBrush(RGB(59, 59, 59));
    wc1.hbrBackground = hBrushTopPanel;
    wc1.lpszClassName = L"TopPanelClass";

    RegisterClass(&wc1);

    HWND hwnd1 = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW,
        L"TopPanelClass",
        L"TopWin",
        WS_POPUP | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX,
        300, 470,
        700,
        30,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    hBrushBackground = CreateSolidBrush(RGB(117, 117, 117));
    wc.hbrBackground = hBrushBackground;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW,
        CLASS_NAME,
        L"MainWin",
        WS_POPUP | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX,
        300, 500,
        700, 1000,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL || hwnd1 == NULL) {
        return 0;
    }

    SetLayeredWindowAttributes(hwnd1, RGB(0, 0, 0), 140, LWA_ALPHA);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    ShowWindow(hwnd1, nCmdShow);
    ShowWindow(hwnd, nCmdShow);

    iconNormal = new Image(ICON_PATH_NORMAL.c_str());
    iconHover = new Image(ICON_PATH_HOVER.c_str());
    iconTrash = new Image(ICON_PATH_TRASH.c_str());
    iconCopy = new Image(ICON_PATH_COPY.c_str());
    iconStart = new Image(ICON_PATH_START.c_str());
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DeleteObject(hBrushBackground);
    DeleteObject(hBrushTopPanel);
    delete iconNormal;
    delete iconHover;
    delete iconTrash;
    delete iconCopy;
    delete iconStart;
    GdiplusShutdown(gdiplusToken);

    return 0;
}

LRESULT CALLBACK WindowProc1(HWND hwnd1, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static POINT prevPos = { 0, 0 };

    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd1, &ps);

        FillRect(hdc, &ps.rcPaint, hBrushTopPanel);

        closeButtonRect.left = ps.rcPaint.right - CLOSE_BUTTON_SIZE;
        closeButtonRect.top = TOP_PANEL_HEIGHT;
        closeButtonRect.right = ps.rcPaint.right;
        closeButtonRect.bottom = TOP_PANEL_HEIGHT + CLOSE_BUTTON_SIZE;

        Image* currentIcon = isMouseOverCloseButton ? iconHover : iconNormal;
        if (currentIcon->GetLastStatus() == Ok) {
            Graphics graphics(hdc);
            graphics.DrawImage(currentIcon, closeButtonRect.left, closeButtonRect.top, CLOSE_BUTTON_SIZE, CLOSE_BUTTON_SIZE);
        }

        Gdiplus::Font font(L"Arial", 12, Gdiplus::FontStyleRegular);
        Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 255, 255, 255));

        RECT textRect = { 10, 0, 200, 30 };
        DrawText(hdc, WINDOW_TEXT.c_str(), -1, &textRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

        EndPaint(hwnd1, &ps);
    }
                 return 0;

    case WM_MOUSEMOVE: {
        POINT cursorPos;
        GetCursorPos(&cursorPos);
        ScreenToClient(hwnd1, &cursorPos);
        bool newMouseOverCloseButton = PtInRect(&closeButtonRect, cursorPos);

        if (newMouseOverCloseButton != isMouseOverCloseButton) {
            isMouseOverCloseButton = newMouseOverCloseButton;
            InvalidateRect(hwnd1, NULL, TRUE);
        }

        if (!isMouseInWindow) {
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd1;
            TrackMouseEvent(&tme);
            isMouseInWindow = true;
        }

        if (wParam & MK_LBUTTON) {
            int dx = cursorPos.x - closeButtonRect.left;
            int dy = cursorPos.y - closeButtonRect.top;

            RECT rect;
            GetWindowRect(hwnd1, &rect);
            MoveWindow(hwnd1, rect.left + dx, rect.top + dy, rect.right - rect.left, rect.bottom - rect.top, TRUE);

            GetWindowRect(hwnd, &rect);
            MoveWindow(hwnd, rect.left + dx, rect.top + dy, rect.right - rect.left, rect.bottom - rect.top, TRUE);
        }
    }
                     return 0;

    case WM_LBUTTONDOWN: {
        POINT cursorPos;
        GetCursorPos(&cursorPos);
        ScreenToClient(hwnd1, &cursorPos);
        if (PtInRect(&closeButtonRect, cursorPos)) {
            DestroyWindow(hwnd1);
        }
        else {
            ReleaseCapture();
            SendMessage(hwnd1, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
        }
    }
                       return 0;

    case WM_MOVE: {
        RECT rect;
        GetWindowRect(hwnd1, &rect);

        int offsetX = rect.left - prevPos.x;
        int offsetY = rect.top - prevPos.y;

        prevPos.x = rect.left;
        prevPos.y = rect.top;

        RECT rectMain;
        GetWindowRect(hwnd, &rectMain);
        MoveWindow(hwnd, rectMain.left + offsetX, rectMain.top + offsetY, rectMain.right - rectMain.left, rectMain.bottom - rectMain.top, TRUE);
    }
                return 0;

    case WM_MOUSELEAVE: {
        isMouseInWindow = false;
        isMouseOverCloseButton = false;
        InvalidateRect(hwnd1, NULL, TRUE);
    }

    case WM_SETCURSOR: {
        if (LOWORD(lParam) == HTCLIENT) {
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return TRUE;
        }
        break;
    }
    }
    return DefWindowProc(hwnd1, uMsg, wParam, lParam);
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {

        HFONT hFont = CreateFont(
            20,
            0,
            0, 0,
            FW_NORMAL,
            FALSE,
            FALSE,
            FALSE,
            DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_SWISS,
            L"Segoe UI");

        hEditInput = CreateWindowEx(
            WS_EX_CLIENTEDGE, L"EDIT", L"Ну выкладывай",
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | ES_LEFT,
            20, 100, 320, 650, hwnd, NULL, NULL, NULL
        );

        hStaticOutput1 = CreateWindowEx(
            0, L"EDIT", L"Исправленный код:",
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_LEFT | ES_READONLY,
            360, 100, 320, 650, hwnd, NULL, NULL, NULL
        );

        hStaticOutput2 = CreateWindowEx(
            0, L"EDIT", L"Исправленные ошибки:",
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_LEFT | ES_READONLY,
            20, 760, 660, 220, hwnd, NULL, NULL, NULL
        );


        SendMessage(hEditInput, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hStaticOutput1, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hStaticOutput2, WM_SETFONT, (WPARAM)hFont, TRUE);

        return 0;
    }



    case WM_COMMAND: {
        HWND hWndEdit = (HWND)lParam;

        if (HIWORD(wParam) == EN_SETFOCUS && hWndEdit == hEditInput) {
            if (!isInputCleared) {
                SetWindowText(hEditInput, L"");
                isInputCleared = true;
            }
        }

        if (HIWORD(wParam) == EN_KILLFOCUS && hWndEdit == hEditInput) {
            int textLength = GetWindowTextLength(hEditInput);
            if (textLength == 0) {
                SetWindowText(hEditInput, L"Ну выкладывай");
                isInputCleared = false;
            }
        }
        break;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, hBrushBackground);
        Graphics graphics(hdc);
        graphics.DrawImage(iconTrash, 320, 75, 25, 25);
        graphics.DrawImage(iconCopy, 295, 75, 25, 25);
        graphics.DrawImage(iconCopy, 656, 75, 25, 25);
        graphics.DrawImage(iconStart, 20, 73, 28, 28);
        EndPaint(hwnd, &ps);
    }
                 return 0;
    case WM_LBUTTONDOWN: {
        POINT cursorPos;
        GetCursorPos(&cursorPos);
        ScreenToClient(hwnd, &cursorPos);

        RECT iconRectTrash = { 320, 75, 320 + 25, 75 + 25 };
        if (PtInRect(&iconRectTrash, cursorPos)) {
            SetWindowText(hEditInput, L"");
        }
        RECT iconRectCopyLeft = { 295, 75, 295 + 25, 75 + 25 };
        if (PtInRect(&iconRectCopyLeft, cursorPos)) {
            if (PtInRect(&iconRectCopyLeft, cursorPos)) {
                int textLength = GetWindowTextLength(hEditInput);
                if (textLength > 0) {
                    wchar_t* buffer = new wchar_t[textLength + 1];
                    GetWindowText(hEditInput, buffer, textLength + 1);

                    if (OpenClipboard(hwnd)) {
                        EmptyClipboard();

                        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (textLength + 1) * sizeof(wchar_t));
                        if (hMem) {
                            memcpy(GlobalLock(hMem), buffer, (textLength + 1) * sizeof(wchar_t));
                            GlobalUnlock(hMem);
                            SetClipboardData(CF_UNICODETEXT, hMem);
                        }

                        CloseClipboard();
                    }

                    delete[] buffer;
                }
            }
        }
        RECT iconRectCopyRight = { 656, 75, 656 + 25, 75 + 25 };
        if (PtInRect(&iconRectCopyRight, cursorPos)) {
            if (PtInRect(&iconRectCopyRight, cursorPos)) {
                int textLength = GetWindowTextLength(hStaticOutput1);
                if (textLength > 0) {
                    wchar_t* buffer = new wchar_t[textLength + 1];
                    GetWindowText(hStaticOutput1, buffer, textLength + 1);

                    if (OpenClipboard(hwnd)) {
                        EmptyClipboard();

                        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (textLength + 1) * sizeof(wchar_t));
                        if (hMem) {
                            memcpy(GlobalLock(hMem), buffer, (textLength + 1) * sizeof(wchar_t));
                            GlobalUnlock(hMem);
                            SetClipboardData(CF_UNICODETEXT, hMem);
                        }

                        CloseClipboard();
                    }

                    delete[] buffer;
                }
            }
        }
        RECT iconRectStart = { 20, 73, 20 + 28, 73 + 28 };
        if (PtInRect(&iconRectStart, cursorPos)) {
            int textLength = GetWindowTextLength(hEditInput);
            if (textLength > 0) {
                if (firstcycle == 1) {
                    wchar_t* inputBuffer = new wchar_t[textLength + 1];
                    GetWindowText(hEditInput, inputBuffer, textLength + 1);


                    int outputTextLength = GetWindowTextLength(hStaticOutput1);
                    wchar_t* outputBuffer = new wchar_t[outputTextLength + 1];
                    GetWindowText(hStaticOutput1, outputBuffer, outputTextLength + 1);

                    std::wstring newText = outputBuffer;
                    newText += L"\r\n";
                    newText += inputBuffer;

                    SetWindowText(hStaticOutput1, newText.c_str());

                    delete[] inputBuffer;
                    delete[] outputBuffer;
                    firstcycle = 0;
                }
                else {
                    SetWindowText(hStaticOutput1, L"");
                    wchar_t* inputBuffer = new wchar_t[textLength + 1];
                    GetWindowText(hEditInput, inputBuffer, textLength + 1);

                    std::wstring newText = L"Исправленный код:";
                    newText += L"\r\n";
                    newText += inputBuffer;

                    SetWindowText(hStaticOutput1, newText.c_str());

                    delete[] inputBuffer;
                }
            }
        }

        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}