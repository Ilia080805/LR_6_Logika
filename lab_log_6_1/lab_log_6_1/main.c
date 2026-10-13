#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>


typedef struct Node {
    int vertex;
    struct Node* next;
} Node;


void freeAdjList(Node** adjList, int n) {
    if (!adjList) return;
    for (int i = 0; i < n; i++) {
        Node* curr = adjList[i];
        while (curr != NULL) {
            Node* tmp = curr;
            curr = curr->next;
            free(tmp);
        }
    }
    free(adjList);
}


void addEdgeToList(Node** adjList, int u, int v) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->vertex = v;
    newNode->next = adjList[u];
    adjList[u] = newNode;
}


Node** matrixToAdjList(int** matrix, int n) {
    Node** adjList = (Node**)malloc(n * sizeof(Node*));
    for (int i = 0; i < n; i++) {
        adjList[i] = NULL;
    }
    for (int i = 0; i < n; i++) {
        for (int j = n - 1; j >= 0; j--) {
            if (matrix[i][j] == 1) {
                addEdgeToList(adjList, i, j);
            }
        }
    }
    return adjList;
}


void printAdjList(Node** adjList, int n) {
    printf("\n=== Списки смежности ===\n");
    for (int i = 0; i < n; i++) {
        printf("Вершина %d: ", i + 1);
        Node* curr = adjList[i];
        if (!curr) {
            printf("[нет связей]");
        }
        while (curr != NULL) {
            printf("-> %d ", curr->vertex + 1);
            curr = curr->next;
        }
        printf("\n");
    }
}


int** createMatrix(int n) {
    int** matrix = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        matrix[i] = (int*)malloc(n * sizeof(int));
    }
    return matrix;
}


void freeMatrix(int** matrix, int n) {
    if (!matrix) return;
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}


void generateGraph(int** matrix, int n) {
    for (int i = 0; i < n; i++) {
        matrix[i][i] = 0; // петли отсутствуют
        for (int j = i + 1; j < n; j++) {
            int val = rand() % 2;
            matrix[i][j] = val;
            matrix[j][i] = val;
        }
    }
}


void printMatrix(int** matrix, int n, const char* title) {
    printf("\n--- %s (размер: %d x %d) ---\n", title, n, n);
    printf("     ");
    for (int j = 0; j < n; j++) {
        printf("%3d", j + 1);
    }
    printf("\n     ");
    for (int j = 0; j < n; j++) {
        printf("---");
    }
    printf("\n");

    for (int i = 0; i < n; i++) {
        printf("%3d |", i + 1);
        for (int j = 0; j < n; j++) {
            printf("%3d", matrix[i][j]);
        }
        printf("\n");
    }
}


int getEdge(int** m, int n, int i, int j) {
    if (i < n && j < n) {
        return m[i][j];
    }
    return 0;
}


int** identification(int** matrix, int n, int u, int v, int* out_n) {
    int new_n = n - 1;
    *out_n = new_n;
    int** res = createMatrix(new_n);


    if (u > v) {
        int tmp = u; u = v; v = tmp;
    }


    for (int i = 0, ni = 0; i < n; i++) {
        if (i == v) continue;
        for (int j = 0, nj = 0; j < n; j++) {
            if (j == v) continue;

            if (i == u || j == u) {
                int val_from_u = matrix[i][j];
                int val_from_v = (i == u) ? matrix[v][j] : matrix[i][v];
                res[ni][nj] = val_from_u || val_from_v;
            }
            else {
                res[ni][nj] = matrix[i][j];
            }
            nj++;
        }
        ni++;
    }

    res[u][u] = 0;
    return res;
}

int** contractEdge(int** matrix, int n, int u, int v, int* out_n) {
    if (matrix[u][v] == 0) {
        printf("\n[Ошибка]: Между вершинами %d и %d нет ребра! Стягивание невозможно.\n", u + 1, v + 1);
        *out_n = n;
        return NULL;
    }
    return identification(matrix, n, u, v, out_n);
}


int** splitVertex(int** matrix, int n, int u, int* out_n) {
    int new_n = n + 1;
    *out_n = new_n;
    int** res = createMatrix(new_n);


    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            res[i][j] = matrix[i][j];
        }
        res[i][n] = 0;
    }
    for (int j = 0; j < new_n; j++) {
        res[n][j] = 0;
    }


    res[u][n] = 1;
    res[n][u] = 1;


    int count = 0;
    for (int i = 0; i < n; i++) {
        if (i != u && matrix[u][i] == 1) {
            count++;
            if (count % 2 == 0) {
                res[u][i] = 0;
                res[i][u] = 0;
                res[n][i] = 1;
                res[i][n] = 1;
            }
        }
    }

    return res;
}


int** graphUnion(int** m1, int n1, int** m2, int n2, int* out_n) {
    int n = (n1 > n2) ? n1 : n2;
    *out_n = n;
    int** res = createMatrix(n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            res[i][j] = getEdge(m1, n1, i, j) || getEdge(m2, n2, i, j);
        }
    }
    return res;
}


int** graphIntersection(int** m1, int n1, int** m2, int n2, int* out_n) {
    int n = (n1 < n2) ? n1 : n2;
    *out_n = n;
    int** res = createMatrix(n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            res[i][j] = m1[i][j] && m2[i][j];
        }
    }
    return res;
}


int** graphRingSum(int** m1, int n1, int** m2, int n2, int* out_n) {
    int n = (n1 > n2) ? n1 : n2;
    *out_n = n;
    int** res = createMatrix(n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            res[i][j] = getEdge(m1, n1, i, j) ^ getEdge(m2, n2, i, j);
        }
    }
    return res;
}


int** cartesianProduct(int** m1, int n1, int** m2, int n2, int* out_n) {
    int total_n = n1 * n2;
    *out_n = total_n;
    int** res = createMatrix(total_n);

    for (int i = 0; i < total_n; i++) {
        for (int j = 0; j < total_n; j++) {
            res[i][j] = 0;
        }
    }

    for (int u1 = 0; u1 < n1; u1++) {
        for (int v1 = 0; v1 < n2; v1++) {
            int row = u1 * n2 + v1;

            for (int u2 = 0; u2 < n1; u2++) {
                for (int v2 = 0; v2 < n2; v2++) {
                    int col = u2 * n2 + v2;

                    if ((u1 == u2 && m2[v1][v2] == 1) ||
                        (v1 == v2 && m1[u1][u2] == 1)) {
                        res[row][col] = 1;
                    }
                }
            }
        }
    }
    return res;
}


int main() {
    setlocale(LC_ALL, "Russian");
    srand((unsigned int)time(NULL));

    int n1 = 0, n2 = 0;
    int** G1 = NULL;
    int** G2 = NULL;

    printf("========================================================\n");
    printf("   Лабораторная работа №6: Операции над графами\n");
    printf("========================================================\n");

    printf("Введите количество вершин для графа G1: ");
    if (scanf("%d", &n1) != 1 || n1 < 1) n1 = 4;

    printf("Введите количество вершин для графа G2: ");
    if (scanf("%d", &n2) != 1 || n2 < 1) n2 = 4;

    G1 = createMatrix(n1);
    G2 = createMatrix(n2);
    generateGraph(G1, n1);
    generateGraph(G2, n2);

    printMatrix(G1, n1, "Исходный граф G1");
    printMatrix(G2, n2, "Исходный граф G2");

    while (1) {
        printf("\n------------------------- МЕНЮ -------------------------\n");
        printf("1. Показать матрицы графов G1 и G2\n");
        printf("2. Показать списки смежности для G1 и G2 (Задание 1*)\n");
        printf("3. Отождествление вершин в G1 (Задание 2а)\n");
        printf("4. Стягивание ребра в G1 (Задание 2б)\n");
        printf("5. Расщепление вершины в G1 (Задание 2в)\n");
        printf("6. Объединение G1 и G2 (Задание 3а)\n");
        printf("7. Пересечение G1 и G2 (Задание 3б)\n");
        printf("8. Кольцевая сумма G1 и G2 (Задание 3в)\n");
        printf("9. Декартово произведение G1 x G2 (Задание 4*)\n");
        printf("10. Перегенерировать графы G1 и G2\n");
        printf("0. Выход\n");
        printf("Выберите пункт: ");

        int choice;
        if (scanf("%d", &choice) != 1) {
            break;
        }

        if (choice == 0) break;

        switch (choice) {
        case 1:
            printMatrix(G1, n1, "Граф G1");
            printMatrix(G2, n2, "Граф G2");
            break;

        case 2: {
            Node** list1 = matrixToAdjList(G1, n1);
            printf("\nСписки смежности для G1:");
            printAdjList(list1, n1);
            freeAdjList(list1, n1);

            Node** list2 = matrixToAdjList(G2, n2);
            printf("\nСписки смежности для G2:");
            printAdjList(list2, n2);
            freeAdjList(list2, n2);
            break;
        }

        case 3: {
            if (n1 < 2) {
                printf("Недостаточно вершин для отождествления!\n");
                break;
            }
            int u, v;
            printf("Введите номера двух вершин для отождествления (1..%d): ", n1);
            scanf("%d %d", &u, &v);
            if (u < 1 || u > n1 || v < 1 || v > n1 || u == v) {
                printf("Некорректные номера вершин!\n");
                break;
            }
            int res_n;
            int** res = identification(G1, n1, u - 1, v - 1, &res_n);
            printMatrix(res, res_n, "Результат отождествления");

            Node** list = matrixToAdjList(res, res_n);
            printAdjList(list, res_n);
            freeAdjList(list, res_n);

            freeMatrix(res, res_n);
            break;
        }

        case 4: {
            if (n1 < 2) {
                printf("Недостаточно вершин!\n");
                break;
            }
            int u, v;
            printf("Введите номера двух смежных вершин для стягивания (1..%d): ", n1);
            scanf("%d %d", &u, &v);
            if (u < 1 || u > n1 || v < 1 || v > n1 || u == v) {
                printf("Некорректные номера вершин!\n");
                break;
            }
            int res_n;
            int** res = contractEdge(G1, n1, u - 1, v - 1, &res_n);
            if (res) {
                printMatrix(res, res_n, "Результат стягивания ребра");
                Node** list = matrixToAdjList(res, res_n);
                printAdjList(list, res_n);
                freeAdjList(list, res_n);

                freeMatrix(res, res_n);
            }
            break;
        }

        case 5: {
            int u;
            printf("Введите номер вершины для расщепления (1..%d): ", n1);
            scanf("%d", &u);
            if (u < 1 || u > n1) {
                printf("Некорректный номер вершины!\n");
                break;
            }
            int res_n;
            int** res = splitVertex(G1, n1, u - 1, &res_n);
            printMatrix(res, res_n, "Результат расщепления вершины");

            Node** list = matrixToAdjList(res, res_n);
            printAdjList(list, res_n);
            freeAdjList(list, res_n);

            freeMatrix(res, res_n);
            break;
        }

        case 6: {
            int res_n;
            int** res = graphUnion(G1, n1, G2, n2, &res_n);
            printMatrix(res, res_n, "Результат объединения G1 U G2");
            freeMatrix(res, res_n);
            break;
        }

        case 7: {
            int res_n;
            int** res = graphIntersection(G1, n1, G2, n2, &res_n);
            printMatrix(res, res_n, "Результат пересечения G1 ∩ G2");
            freeMatrix(res, res_n);
            break;
        }

        case 8: {
            int res_n;
            int** res = graphRingSum(G1, n1, G2, n2, &res_n);
            printMatrix(res, res_n, "Результат кольцевой суммы G1 ⊕ G2");
            freeMatrix(res, res_n);
            break;
        }

        case 9: {
            int res_n;
            int** res = cartesianProduct(G1, n1, G2, n2, &res_n);
            printMatrix(res, res_n, "Результат декартова произведения G1 x G2");
            freeMatrix(res, res_n);
            break;
        }

        case 10: {
            freeMatrix(G1, n1);
            freeMatrix(G2, n2);
            printf("Введите количество вершин для графа G1: ");
            scanf("%d", &n1);
            printf("Введите количество вершин для графа G2: ");
            scanf("%d", &n2);
            G1 = createMatrix(n1);
            G2 = createMatrix(n2);
            generateGraph(G1, n1);
            generateGraph(G2, n2);
            printMatrix(G1, n1, "Новый граф G1");
            printMatrix(G2, n2, "Новый граф G2");
            break;
        }

        default:
            printf("Неверный пункт меню!\n");
        }
    }


    freeMatrix(G1, n1);
    freeMatrix(G2, n2);

    printf("\nПрограмма успешно завершена. Память очищена.\n");
    return 0;
}