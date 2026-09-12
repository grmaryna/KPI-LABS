#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct linked_list {
    double info;
    struct linked_list *next_p;
} l_list;

l_list *l_list_init(double item) {
    l_list *l_p = malloc(sizeof(l_list));
    if (l_p == NULL) {
        printf("Помилка виділення пам'яті!\n");
        exit(1);
    }
    l_p->info   = item;
    l_p->next_p = NULL;
    return l_p;
}

l_list *add_list(l_list *head, double item) {
    l_list *node_p = l_list_init(item);

    if (head == NULL)
        return node_p;

    l_list *current = head;
    while (current->next_p != NULL)
        current = current->next_p;

    current->next_p = node_p;
    return head;
}

void print_list(l_list *head) {
    l_list *current = head;
    while (current != NULL) {
        printf("%.2f", current->info);
        if (current->next_p != NULL)
            printf(" -> ");
        current = current->next_p;
    }
    printf(" -> NULL\n");
}

void bubble_sort(l_list *head) {
    if (head == NULL)
        return;

    int swapped;

    do {
        swapped = 0;
        l_list *current = head;

        while (current->next_p != NULL) {
            if (current->info < current->next_p->info) {
                double temp          = current->info;
                current->info        = current->next_p->info;
                current->next_p->info = temp;
                swapped = 1;
            }
            current = current->next_p;
        }
    } while (swapped);
}

l_list *find_list(l_list *head, double item) {
    l_list *current = head;
    while (current != NULL) {
        if (fabs(current->info - item) < 1e-9)
            return current;
        current = current->next_p;
    }
    return NULL;
}

l_list *del_first(l_list *head) {
    if (head == NULL)
        return NULL;
    l_list *next = head->next_p;
    free(head);
    return next;
}

void free_list(l_list *head) {
    while (head != NULL)
        head = del_first(head);
}

int main() {
    l_list *head = NULL;
    int    n;
    double value;

    do {
        printf("Введіть кількість елементів (n > 0): ");
        scanf("%d", &n);
    } while (n <= 0);

    for (int i = 0; i < n; i++) {
        printf("Елемент %d: ", i + 1);
        scanf("%lf", &value);
        head = add_list(head, value);
    }

    printf("\nПочатковий список:\n");
    print_list(head);

    bubble_sort(head);

    printf("\nВідсортований список (за незбільшенням):\n");
    print_list(head);

    printf("\nВведіть значення для пошуку: ");
    scanf("%lf", &value);

    l_list *found = find_list(head, value);
    if (found != NULL)
        printf("Знайдено: %.2f\n", found->info);
    else
        printf("Елемент %.2f не знайдено\n", value);

    free_list(head);
    head = NULL;

    return 0;
}