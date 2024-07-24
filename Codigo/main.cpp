#include <windows.h>
#include <iostream>
#include <sstream>
#include <cstring>

using namespace std;


HBITMAP hBitmapEmergente; // Variable para almacenar el bitmap de fondo de la ventana emergente
HDC hdcMemEmergente;
static HWND hwndEmergente = NULL;




struct Circulo {
    int x, y, diameter;
    char text[100];
};

struct Conexion {
    int startIndex, endIndex;
};

struct AVLNode {
    int key;
    Circulo circle;
    AVLNode* left;
    AVLNode* right;
    AVLNode* parent;
    int height;
};



// Funciones de recorrido del árbol
void PreordenTraversal(AVLNode* nodo, ostringstream& oss) {
    if (nodo) {
        oss << nodo->key << " ";
        PreordenTraversal(nodo->left, oss);
        PreordenTraversal(nodo->right, oss);
    }
}

void InordenTraversal(AVLNode* nodo, ostringstream& oss) {
    if (nodo) {
        InordenTraversal(nodo->left, oss);
        oss << nodo->key << " ";
        InordenTraversal(nodo->right, oss);
    }
}

void PostordenTraversal(AVLNode* nodo, ostringstream& oss) {
    if (nodo) {
        PostordenTraversal(nodo->left, oss);
        PostordenTraversal(nodo->right, oss);
        oss << nodo->key << " ";
    }
}





AVLNode* CrearNodo(int key, Circulo circle);
int ObtenerAltura(AVLNode* nodo);
int ObtenerBalance(AVLNode* nodo);
AVLNode* RotarDerecha(AVLNode* y);
AVLNode* RotarIzquierda(AVLNode* x);
AVLNode* InsertarNodo(AVLNode* nodo, int key, Circulo circle);
//Eliminar Nodo por el menor de los mayores
AVLNode* EliminarNodo_MenorMayores(AVLNode* nodo, int key);
//Eliminar por el mayor de los menores
AVLNode* EliminarNodo_MayorMenores(AVLNode* nodo, int key);
AVLNode* BuscarMinimo(AVLNode* nodo);
AVLNode* BuscarMaximo(AVLNode* nodo);
int ObtenerNivel(AVLNode* raiz, int key, int nivel);
string buscar_por_Anchura(AVLNode* raiz, int val);
bool buscar_porAnchura(AVLNode* raiz,int nivel, int val, string &camino);
bool backtracking_(AVLNode* node, int val,string& camino);
string buscarConBacktracking(AVLNode* raiz,int val);


/*
    Métodos para el modo gráfico
*/
void DibujarArbolAVL(HDC hdc, AVLNode* nodo, int x, int y, int dx, int offsetY);
void RegistrarClaseVentana(HINSTANCE hInstancia);
void CrearBotones(HWND hwnd, HINSTANCE hInstancia);
void MostrarVentanaEmergente(HWND hwnd, HINSTANCE hInstancia);
void AgregarCirculo(Circulo* circulos, int* numCirculos, int x, int y, int diameter, const char* text);
void AgregarConexion(Conexion* conexiones, int* numConexiones, int startIndex, int endIndex);
void DibujarCirculosYConexiones(HDC hdc, Circulo* circulos, int numCirculos, Conexion* conexiones, int numConexiones);

AVLNode* arbolAVL = nullptr;
Circulo circulos[100];
int numCirculos = 0;
Conexion conexiones[100];
int numConexiones = 0;
int x = 300, y = 100;
HBITMAP hBitmap; // Variable para almacenar el bitmap de fondo

LRESULT CALLBACK ProcedimientoVentana(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ProcedimientoVentanaEmergente(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstanciaActual, HINSTANCE hInstanciaPrevia, LPSTR lpCmdLinea, int nCmdShow) {
    RegistrarClaseVentana(hInstanciaActual);
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int windowWidth = 515;
    int windowHeight = 361;

    int posX = (screenWidth - windowWidth) / 2;
    int posY = (screenHeight - windowHeight) / 2;

    HWND ventana = CreateWindowEx(
        0,
        "IdentificadorVentana",
        "Ventana Principal",
        WS_POPUP | WS_CAPTION,
        posX, posY, windowWidth, windowHeight,
        HWND_DESKTOP,
        NULL,
        hInstanciaActual,
        NULL
    );

    if (!ventana) {
        MessageBox(NULL, "No se pudo crear la ventana", "Error", MB_ICONERROR);
        return EXIT_FAILURE;
    }

    ShowWindow(ventana, nCmdShow);
    UpdateWindow(ventana);

    MSG mensaje;
    while (GetMessage(&mensaje, NULL, 0, 0) > 0) {
        TranslateMessage(&mensaje);
        DispatchMessage(&mensaje);
    }

    return mensaje.wParam;
}


void RegistrarClaseVentana(HINSTANCE hInstancia) {
    WNDCLASSEX clase;
    clase.cbSize = sizeof(WNDCLASSEX);
    clase.cbClsExtra = 0;
    clase.cbWndExtra = 0;
    clase.style = CS_HREDRAW | CS_VREDRAW;
    clase.lpfnWndProc = ProcedimientoVentana;
    clase.hInstance = hInstancia;
    clase.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    clase.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    clase.hCursor = LoadCursor(NULL, IDC_ARROW);
    clase.hbrBackground = CreateSolidBrush(RGB(26, 47, 173));
    clase.lpszMenuName = NULL;
    clase.lpszClassName = "IdentificadorVentana";

    if (!RegisterClassEx(&clase)) {
        MessageBox(NULL, "No se pudo registrar la clase de la ventana", "Error", MB_ICONERROR);
        exit(EXIT_FAILURE);
    }
}

void CrearBotones(HWND hwnd, HINSTANCE hInstancia) {
    RECT rect;
    GetClientRect(hwnd, &rect);
    int windowWidth = rect.right - rect.left;
    int buttonWidth = 130;
    int buttonHeight = 30;
    int buttonSpacing = 10;
    int totalHeight = buttonHeight * 3 + buttonSpacing * 2;

    int startX = (windowWidth - buttonWidth) / 2;
    int startY = (rect.bottom - rect.top - totalHeight) / 2;

       CreateWindow(
        "BUTTON",
        "Iniciar Simulación",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        startX,
        startY,
        buttonWidth,
        buttonHeight,
        hwnd,
        (HMENU)1,
        hInstancia,
        NULL
    );

    CreateWindow(
        "BUTTON",
        "Mostrar nodo",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        startX,
        startY + buttonHeight + buttonSpacing,
        buttonWidth,
        buttonHeight,
        hwnd,
        (HMENU)2,
        hInstancia,
        NULL
    );

    CreateWindow(
        "BUTTON",
        "Salir",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        startX,
        startY + 2 * (buttonHeight + buttonSpacing),
        buttonWidth,
        buttonHeight,
        hwnd,
        (HMENU)3,
        hInstancia,
        NULL
    );
}

void MostrarVentanaEmergente(HWND hwnd, HINSTANCE hInstancia) {
    if (hwndEmergente && IsWindow(hwndEmergente)) {
        // Si la ventana ya existe y está oculta, simplemente muéstrala
        ShowWindow(hwndEmergente, SW_SHOW);
        return;
    }

    WNDCLASSEX claseEmergente;
    claseEmergente.cbSize = sizeof(WNDCLASSEX);
    claseEmergente.style = 0;
    claseEmergente.lpfnWndProc = ProcedimientoVentanaEmergente;
    claseEmergente.cbClsExtra = 0;
    claseEmergente.cbWndExtra = 0;
    claseEmergente.hInstance = hInstancia;
    claseEmergente.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    claseEmergente.hCursor = LoadCursor(NULL, IDC_ARROW);
    claseEmergente.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    claseEmergente.lpszMenuName = NULL;
    claseEmergente.lpszClassName = "VentanaEmergente";
    claseEmergente.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&claseEmergente)) {
        MessageBox(NULL, "No se pudo registrar la ventana emergente", "Error", MB_ICONERROR);
        return;
    }

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int windowWidth = 1200;
    int windowHeight = 600;

    int posX = (screenWidth - windowWidth) / 2;
    int posY = (screenHeight - windowHeight) / 2;

    hwndEmergente = CreateWindowEx(
        0,
        "VentanaEmergente",
        "Anadir/Eliminar Nodo",
        WS_POPUP | WS_CAPTION,
        posX, posY, windowWidth, windowHeight,
        hwnd,
        NULL,
        hInstancia,
        NULL
    );

    if (!hwndEmergente) {
        MessageBox(NULL, "No se pudo crear la ventana emergente", "Error", MB_ICONERROR);
        return;
    }

    ShowWindow(hwndEmergente, SW_SHOW);
    UpdateWindow(hwndEmergente);
}

LRESULT CALLBACK ProcedimientoVentana(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HINSTANCE Instancia;

    switch (msg) {
        case WM_CREATE: {
            Instancia = ((LPCREATESTRUCT)lParam)->hInstance;
            CrearBotones(hwnd, Instancia);

            // Cargar la imagen de fondo
            hBitmap = (HBITMAP)LoadImage(NULL, "d.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            if (!hBitmap) {
                MessageBox(hwnd, "No se pudo cargar la imagen de fondo", "Error", MB_ICONERROR);
            }
            break;
        }
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case 1:
                    MostrarVentanaEmergente(hwnd, Instancia);
                    break;
                case 2:
                    MostrarVentanaEmergente(hwnd, Instancia);
                    break;
                case 3:
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
            }
            break;
        }
        case WM_GETMINMAXINFO: {
            MINMAXINFO* pMinMaxInfo = (MINMAXINFO*)lParam;
            pMinMaxInfo->ptMinTrackSize.x = 515; // Ancho mínimo
            pMinMaxInfo->ptMinTrackSize.y = 361; // Alto mínimo
            pMinMaxInfo->ptMaxTrackSize.x = 515; // Ancho máximo
            pMinMaxInfo->ptMaxTrackSize.y = 361; // Alto máximo
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Dibujar la imagen de fondo
            if (hBitmap) {
                HDC hdcMem = CreateCompatibleDC(hdc);
                SelectObject(hdcMem, hBitmap);
                BitBlt(hdc, 0, 0, ps.rcPaint.right, ps.rcPaint.bottom, hdcMem, 0, 0, SRCCOPY);
                DeleteDC(hdcMem);
            }

            EndPaint(hwnd, &ps);
            break;
        }
        case WM_DESTROY: {
            // Terminar el bucle de mensajes y cerrar el programa
            PostQuitMessage(0);
            break;
        }
        default: {
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
    }
    return 0;
}
LRESULT CALLBACK ProcedimientoVentanaEmergente(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hEdit;
    static HWND hButtonAgregar;
    static HWND hButtonEliminar;
    static HWND hButtonEliminarM;
    static HWND hbuttonBuscar;
    static HWND hButtonCerrar;
    static HWND hButtonOrdenar;

    static char texto[100];

    switch (msg) {
        case WM_CREATE: {

            CreateWindow(
                "STATIC",
                "Ingrese número:",
                WS_VISIBLE | WS_CHILD,
                20, 20, 110, 20,
                hwnd,
                NULL,
                ((LPCREATESTRUCT)lParam)->hInstance,
                NULL
            );

            hEdit = CreateWindow(
                "EDIT",
                "",
                WS_VISIBLE | WS_CHILD | WS_BORDER,
                140, 17, 200, 25,
                hwnd,
                NULL,
                ((LPCREATESTRUCT)lParam)->hInstance,
                NULL
            );

            hButtonAgregar = CreateWindow(
                "BUTTON",
                "Insertar",
                WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                20, 50, 100, 30,
                hwnd,
                (HMENU)1,
                ((LPCREATESTRUCT)lParam)->hInstance,
                NULL
            );

            hButtonEliminar = CreateWindow(
                "BUTTON",
                "Eliminar por el menor de los mayores",
                WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                130, 50, 250, 30,
                hwnd,
                (HMENU)2,
                ((LPCREATESTRUCT)lParam)->hInstance,
                NULL
            );
            hButtonEliminar = CreateWindow(
                "BUTTON",
                "Eliminar por el mayor de los menores",
                WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                130, 90, 250, 30,
                hwnd,
                (HMENU)3,
                ((LPCREATESTRUCT)lParam)->hInstance,
                NULL
            );

            hButtonOrdenar = CreateWindow(
                "BUTTON",
                "Ordenar",
                WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
               250, 125, 100, 30,
                hwnd,
                (HMENU)4,
                ((LPCREATESTRUCT)lParam)->hInstance,
                NULL
            );

             hbuttonBuscar = CreateWindow(
                "BUTTON",
                "Buscar",
                WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
               20, 90, 100, 30,
                hwnd,
                (HMENU)5,
                ((LPCREATESTRUCT)lParam)->hInstance,
                NULL
            );

            hButtonCerrar = CreateWindow(
                "BUTTON",
                "Cerrar",
                WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                1070, 520, 100, 30,
                hwnd,
                (HMENU)6,
                ((LPCREATESTRUCT)lParam)->hInstance,
                NULL
            );
            hBitmapEmergente = (HBITMAP)LoadImage(NULL, "m.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            if (!hBitmapEmergente) {
                MessageBox(hwnd, "No se pudo cargar la imagen de fondo de la ventana emergente", "Error", MB_ICONERROR);
            }
            break;
        }

        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case 1: {
                    GetWindowText(hEdit, texto, sizeof(texto));
                    int key;
                    if (sscanf(texto, "%d", &key) == 1) {
                        Circulo nuevoCirculo;
                        nuevoCirculo.x = x;
                        nuevoCirculo.y = y;
                        nuevoCirculo.diameter = 50;
                        snprintf(nuevoCirculo.text, sizeof(nuevoCirculo.text), "%d", key);

                        arbolAVL = InsertarNodo(arbolAVL, key, nuevoCirculo);

                        InvalidateRect(hwnd, NULL, TRUE);
                    }
                    SetWindowText(hEdit, "");
                    break;
                }
                case 2: {
                    GetWindowText(hEdit, texto, sizeof(texto));
                    int key;
                    if (sscanf(texto, "%d", &key) == 1) {
                        arbolAVL = EliminarNodo_MenorMayores(arbolAVL, key);
                        InvalidateRect(GetParent(hwnd), NULL, TRUE);
                    }
                    SetWindowText(hEdit, "");
                    break;
                }
                case 3: {
                    GetWindowText(hEdit, texto, sizeof(texto));
                    int key;
                    if (sscanf(texto, "%d", &key) == 1) {
                        arbolAVL = EliminarNodo_MayorMenores(arbolAVL, key);
                        InvalidateRect(GetParent(hwnd), NULL, TRUE);
                    }
                    break;
                    SetWindowText(hEdit, "");
                }
                case 4: {
                    ostringstream ssPre, ssIn, ssPost;

                    // Llamada a las funciones de recorrido
                    PreordenTraversal(arbolAVL, ssPre);
                    InordenTraversal(arbolAVL, ssIn);
                    PostordenTraversal(arbolAVL, ssPost);

                    // Mostrar los resultados
                    stringstream ss;
                    ss << "Preorden: " << ssPre.str() << "\n"
                       << "Inorden: " << ssIn.str() << "\n"
                       << "Postorden: " << ssPost.str();

                    MessageBox(hwnd, ss.str().c_str(), "Recorridos del árbol", MB_OK);
                break;
                }
                case 5: {
                    GetWindowText(hEdit, texto, sizeof(texto));
                    int key;
                    // Utiliza buscar_por_Anchura para obtener el resultado de la búsqueda
                    if (sscanf(texto, "%d", &key) == 1) {
                    string resultado = "El recorrido de búsqueda por anchura hasta encontrar " + std::to_string(key) + ":\n";
                    resultado+= buscar_por_Anchura(arbolAVL, key)+"\n";
                    resultado+= buscarConBacktracking(arbolAVL,key);
                    MessageBox(hwnd, resultado.c_str(), "Resultado de la búsqueda", MB_OK);
                }
                    SetWindowText(hEdit, "");
                    break;
                }
                case 6: {
                    ShowWindow(hwnd, SW_HIDE);  // Ocultar la ventana
                    break;
                }
            }
            break;
        }
        case WM_GETMINMAXINFO: {
            MINMAXINFO* pMinMaxInfo = (MINMAXINFO*)lParam;
            pMinMaxInfo->ptMinTrackSize.x = 1200; // Ancho mínimo
            pMinMaxInfo->ptMinTrackSize.y = 600; // Alto mínimo
            pMinMaxInfo->ptMaxTrackSize.x = 1200; // Ancho máximo
            pMinMaxInfo->ptMaxTrackSize.y = 600; // Alto máximo
            return 0;
        }
       case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);


            if (hBitmapEmergente) {
                HDC hdcMem = CreateCompatibleDC(hdc);
                HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hBitmapEmergente);

                BITMAP bitmap;
                GetObject(hBitmapEmergente, sizeof(BITMAP), &bitmap);

                BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

                SelectObject(hdcMem, hbmOld);
                DeleteDC(hdcMem);
            }







    RECT rect;
    GetClientRect(hwnd, &rect);
    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;

    int dx = windowWidth / 8;
    int offsetY = windowHeight / 6;
    if (arbolAVL) {
        DibujarArbolAVL(hdc, arbolAVL, (windowWidth / 2)+200, 40, dx, offsetY);
    }
            EndPaint(hwnd, &ps);
            break;
        }
        case WM_CLOSE:
            ShowWindow(hwnd, SW_HIDE);  // Ocultar la ventana
            break;
        case WM_DESTROY:
            PostQuitMessage(0); // Termina el bucle de mensajes
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

AVLNode* CrearNodo(int key, Circulo circle) {
    AVLNode* nodo = new AVLNode();
    nodo->key = key;
    nodo->circle = circle;
    nodo->left = nullptr;
    nodo->right = nullptr;
    nodo->parent = nullptr;
    nodo->height = 1;
    return nodo;
}

int ObtenerAltura(AVLNode* nodo) {
    return nodo ? nodo->height : 0;
}

int ObtenerBalance(AVLNode* nodo) {
    return nodo ? ObtenerAltura(nodo->left) - ObtenerAltura(nodo->right) : 0;
}

AVLNode* RotarDerecha(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(ObtenerAltura(y->left), ObtenerAltura(y->right)) + 1;
    x->height = max(ObtenerAltura(x->left), ObtenerAltura(x->right)) + 1;

    return x;
}

AVLNode* RotarIzquierda(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(ObtenerAltura(x->left), ObtenerAltura(x->right)) + 1;
    y->height = max(ObtenerAltura(y->left), ObtenerAltura(y->right)) + 1;

    return y;
}

AVLNode* InsertarNodo(AVLNode* nodo, int key, Circulo circle) {
    if (nodo == nullptr) return CrearNodo(key, circle);

    if (key < nodo->key) {
        nodo->left = InsertarNodo(nodo->left, key, circle);
    } else if (key > nodo->key) {
        nodo->right = InsertarNodo(nodo->right, key, circle);
    } else {

        return nodo;
    }

    nodo->height = 1 + max(ObtenerAltura(nodo->left), ObtenerAltura(nodo->right));

    int balance = ObtenerBalance(nodo);

    if (balance > 1 && key < nodo->left->key) {
        return RotarDerecha(nodo);
    }

    if (balance < -1 && key > nodo->right->key) {
        return RotarIzquierda(nodo);
    }

    if (balance > 1 && key > nodo->left->key) {
        nodo->left = RotarIzquierda(nodo->left);
        return RotarDerecha(nodo);
    }

    if (balance < -1 && key < nodo->right->key) {
        nodo->right = RotarDerecha(nodo->right);
        return RotarIzquierda(nodo);
    }

    return nodo;
}

AVLNode* EliminarNodo_MenorMayores(AVLNode* root, int key) {
    if (root == nullptr) return root;

    if (key < root->key) {
        root->left = EliminarNodo_MenorMayores(root->left, key);
    } else if (key > root->key) {
        root->right = EliminarNodo_MenorMayores(root->right, key);
    } else {
        if ((root->left == nullptr) || (root->right == nullptr)) {
            AVLNode* temp = root->left ? root->left : root->right;

            if (temp == nullptr) {
                temp = root;
                root = nullptr;
            } else {
                *root = *temp;
            }
            delete temp;
        } else {
            AVLNode* temp = BuscarMinimo(root->right);

            root->key = temp->key;
            root->circle = temp->circle;

            root->right = EliminarNodo_MenorMayores(root->right, temp->key);
        }
    }

    if (root == nullptr) return root;

    root->height = 1 + max(ObtenerAltura(root->left), ObtenerAltura(root->right));

    int balance = ObtenerBalance(root);

    if (balance > 1 && ObtenerBalance(root->left) >= 0) {
        return RotarDerecha(root);
    }

    if (balance > 1 && ObtenerBalance(root->left) < 0) {
        root->left = RotarIzquierda(root->left);
        return RotarDerecha(root);
    }

    if (balance < -1 && ObtenerBalance(root->right) <= 0) {
        return RotarIzquierda(root);
    }

    if (balance < -1 && ObtenerBalance(root->right) > 0) {
        root->right = RotarDerecha(root->right);
        return RotarIzquierda(root);
    }

    return root;
}

AVLNode* EliminarNodo_MayorMenores(AVLNode* root, int key) {
    if (root == nullptr) return root;

    if (key < root->key) {
        root->left = EliminarNodo_MayorMenores(root->left, key);
    } else if (key > root->key) {
        root->right = EliminarNodo_MayorMenores(root->right, key);
    } else {
        if ((root->left == nullptr) || (root->right == nullptr)) {
            AVLNode* temp = root->left ? root->left : root->right;

            if (temp == nullptr) {
                temp = root;
                root = nullptr;
            } else {
                *root = *temp;
            }
            delete temp;
        } else {
            AVLNode* temp = BuscarMaximo(root->left);

            root->key = temp->key;
            root->circle = temp->circle;

            root->left = EliminarNodo_MayorMenores(root->left, temp->key);
        }
    }

    if (root == nullptr) return root;

    root->height = 1 + max(ObtenerAltura(root->left), ObtenerAltura(root->right));

    int balance = ObtenerBalance(root);

    if (balance > 1 && ObtenerBalance(root->left) >= 0) {
        return RotarDerecha(root);
    }

    if (balance > 1 && ObtenerBalance(root->left) < 0) {
        root->left = RotarIzquierda(root->left);
        return RotarDerecha(root);
    }

    if (balance < -1 && ObtenerBalance(root->right) <= 0) {
        return RotarIzquierda(root);
    }

    if (balance < -1 && ObtenerBalance(root->right) > 0) {
        root->right = RotarDerecha(root->right);
        return RotarIzquierda(root);
    }

    return root;
}

/*
    Para usar el eliminar por el Menor de los mayores en caso de tener 2 hijos
*/
AVLNode* BuscarMinimo(AVLNode* nodo) {
    AVLNode* actual = nodo;
    while (actual->left != nullptr) {
        actual = actual->left;
    }
    return actual;
}

/*
    Para usar el eliminar por el Mayor de los menores en caso de tener 2 hijos
*/

AVLNode* BuscarMaximo(AVLNode* node) {
    AVLNode* actual = node;

    while (actual->right != nullptr) {
        actual = actual->right;
    }

    return actual;
}

void DibujarArbolAVL(HDC hdc, AVLNode* nodo, int x, int y, int dx, int offsetY) {
    if (nodo == nullptr) return;

    int radio = 25;

    // Dibuja las conexiones entre los nodos
    if (nodo->left != nullptr) {
        MoveToEx(hdc, x, y, NULL);
        LineTo(hdc, x - dx, y + offsetY);
    }

    if (nodo->right != nullptr) {
        MoveToEx(hdc, x, y, NULL);
        LineTo(hdc, x + dx, y + offsetY);
    }
    // Dibuja el nodo
    Ellipse(hdc, x - radio, y - radio, x + radio, y + radio);

    RECT rect;
    SetBkMode(hdc, TRANSPARENT);
    rect.left = x - radio;
    rect.top = y - radio;
    rect.right = x + radio;
    rect.bottom = y + radio;
    DrawText(hdc, to_string(nodo->key).c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Llamadas recursivas con ajuste en dx y offsetY
    DibujarArbolAVL(hdc, nodo->left, x - dx, y + offsetY, dx / 2, offsetY);
    DibujarArbolAVL(hdc, nodo->right, x + dx, y + offsetY, dx / 2, offsetY);
}

 int ObtenerNivel(AVLNode* root, int key, int nivel) {
    // Checar si está lleno
    if (root == nullptr) {
        return -1;

    // Checar si se encuentra el valor
    if (root->key == key) {
        return nivel;
    }

    // Busca en el subárbol izquierdo
    int nivelEnIzquierda = ObtenerNivel(root->left, key, nivel + 1);

    // Búsqueda en izquierda y si lo encuentra deja de buscar
    if (nivelEnIzquierda != -1) {
        return nivelEnIzquierda;
    }
}
return ObtenerNivel(root->right, key, nivel + 1);
 }

 string buscar_por_Anchura(AVLNode* raiz, int val) {
        int altura = ObtenerAltura(raiz);
        string camino;
        for (int i = 1; i <= altura; i++) {
            if (buscar_porAnchura(raiz, i, val, camino)) {
                return camino;
            }
        }
                return "Valor no encontrado: " + std::to_string(val);
    }

        bool buscar_porAnchura(AVLNode* nodo, int nivel, int valor, string& camino) {
        if (nodo == nullptr) return false;
        if (nivel == 1) {
            camino += std::to_string(nodo->key) + " >> ";
            if (nodo->key == valor) {
                camino = camino.substr(0, camino.length() - 1); // Eliminar el último espacio
                return true;
            }
        } else if (nivel > 1) {
            if (buscar_porAnchura(nodo->left, nivel - 1, valor, camino)) return true;
            if (buscar_porAnchura(nodo->right, nivel - 1, valor, camino)) return true;
        }
        return false;
    }


bool backtracking_(AVLNode* nodo, int val,string& camino) {
        if (nodo == nullptr) return false;

        camino += std::to_string(nodo->key) + " >> ";

        if (nodo->key == val) {
            camino = camino.substr(0, camino.size() - 4);
            return true;
        }

        if (backtracking_(nodo->left, val, camino) || backtracking_(nodo->right, val, camino)) {
            return true;
        }

        // Si no se encuentra en este camino, eliminar el último nodo del camino
        size_t pos = camino.rfind(" >> ", camino.length() - 6);
        if (pos != std::string::npos) {
            camino = camino.substr(0, pos + 4);
        } else {
            camino.clear();
        }

        return false;
    }
string buscarConBacktracking(AVLNode* raiz,int val){
        std::string camino;
        if (backtracking_(raiz, val, camino)) {
            camino = "El recorrido de búsqueda por backtracking hasta encontrar " + std::to_string(val) + ":\n" + camino;
        } else {
            camino = "Valor no encontrado: " + std::to_string(val);
        }
        return camino;
    }
