#include <stdio.h>
#include <stdlib.h>
//2025.3.23 by TSAI and GPT
typedef struct Node {
    int data;
    struct Node *next;
} Node;
//從檔案讀入一串數字 → 建立 Linked List → 用 Merge Sort 排序（全靠組語）→ 印出排序後的結果


//Block A : splitList
//Split the linked list into two parts，快慢指標拆中點。
void splitList(Node *head, Node **firstHalf, Node **secondHalf)
{
    asm volatile(
        /*
        Block A (splitList), which splits the linked list into two halves
        */
        /*
        ex.將A->B->C->D變成A->B和C->D
        */

        //Step 0:初始化快慢指標slow和fast
        "mv t0, %[head]\n\t"        //t0 = slow
        "ld t1, 8(%[head])\n\t"     //t1 = fast = head->next
        //Step 1:開始走->找中間點
        "LOOP_SPLIT:\n\t"
        "beqz t1, END_SPLIT\n\t"    //if (fast == NULL) break
        "ld t2, 8(t1)\n\t"          //t2 = fast->next
        "beqz t2, END_SPLIT\n\t"    //if (fast->next == NULL) break
        "ld t0, 8(t0)\n\t"          //slow = slow->next
        "ld t1, 8(t2)\n\t"          //fast = fast->next->next
        "j LOOP_SPLIT\n\t"
        "END_SPLIT:\n\t"
        //Step 2:斷開->前半段->後半段
        /*
        in C
            *firstHalf = head;
            *secondHalf = slow->next;
            slow->next = NULL;
        */
        "sd %[head], 0(%[first])\n\t"//firstHalf = head
        "ld t3, 8(t0)\n\t"           //t3 = slow->next    
        "sd t3, 0(%[second])\n\t"    //secondHalf = slow->next
        "sd x0, 8(t0)\n\t"           //slow->next = NULL

        :
        : [head] "r"(head), [first] "r"(firstHalf), [second] "r"(secondHalf)
        : "t0", "t1", "t2", "t3", "memory"
    );
}

//Block B : mergeSortedLists
//Merge two sorted linked lists
Node *mergeSortedLists(Node *a, Node *b)
{
    Node *result = NULL;
    Node *tail = NULL;

    asm volatile(
        /*
        Block B (mergeSortedList), which merges two sorted lists into one
        */
        /*
        result = NULL
        while (a && b):
            if (a->data < b->data):
                選 a；a = a->next
            else:
                選 b；b = b->next
            把選到的接到 tail

        接剩下的 a 或 b

        ex.將A->B和C->D合併成A->B->C->D
        */
        //Step 1:主要的合併迴圈
        "LOOP_MERGE:\n\t"
        "beqz %[a], APPEND_B_MERGE\n\t"         // if a == NULL, jump to b
        "beqz %[b], APPEND_A_MERGE\n\t"         // if b == NULL, jump to a

        //Step 2:比較當前節點 a->data 和 b->data
        "ld t4, 0(%[a])\n\t"                    // t4 = a->data
        "ld t5, 0(%[b])\n\t"                    // t5 = b->data
        "blt t4, t5, SELECT_A_MERGE\n\t"        // if (a < b) goto SELECT_A
        "j SELECT_B_MERGE\n\t"
        
        //Step 3-1:選擇 a 作為新節點
        "SELECT_A_MERGE:\n\t"
        "mv t2, %[a]\n\t"                       // selected = a
        "ld %[a], 8(%[a])\n\t"                  // a = a->next
        "j CONTINUE_MERGE\n\t"

        //Step 3-2:選擇 b 作為新節點
        "SELECT_B_MERGE:\n\t"
        "mv t2, %[b]\n\t"                       // selected = b
        "ld %[b], 8(%[b])\n\t"                  // b = b->next

        //Step 4:將選擇的節點接到 tail
        "CONTINUE_MERGE:\n\t"
        "beqz %[result], INIT_MERGE\n\t"        // if result == NULL
        "sd t2, 8(%[tail])\n\t"                 // tail->next = selected
        "mv %[tail], t2\n\t"                    // tail = selected
        "j LOOP_MERGE\n\t"

        //Step 5:初始化 result 和 tail
        "INIT_MERGE:\n\t"
        "mv %[result], t2\n\t"                  // result = selected
        "mv %[tail], t2\n\t"                    // tail = selected
        "j LOOP_MERGE\n\t"

        //Step 6:將剩下的 a 或 b 接到 tail
        "APPEND_A_MERGE:\n\t"
        "beqz %[tail], SET_RESULT_A\n\t"
        "sd %[a], 8(%[tail])\n\t"
        "j END_MERGE\n\t"

        "SET_RESULT_A:\n\t"  ////為了避免只有一次結點存在////
        "mv %[result], %[a]\n\t"
        "j END_MERGE\n\t"

        "APPEND_B_MERGE:\n\t"
        "beqz %[tail], SET_RESULT_B\n\t"
        "sd %[b], 8(%[tail])\n\t"
        "j END_MERGE\n\t"

        "SET_RESULT_B:\n\t"
        "mv %[result], %[b]\n\t"

        //Step 7:結束
        "END_MERGE:\n\t"
        :
        [a] "+r"(a), [b] "+r"(b), [result] "+r"(result), [tail] "+r"(tail)
        :
        : "t2", "t4", "t5", "memory"
    );

        return result;
        
}

// Merge Sort function for linked list
Node *mergeSort(Node *head)
{
    if (!head || !head->next)
        return head; // Return directly if there is only one node

    Node *firstHalf, *secondHalf;
    splitList(head, &firstHalf,
              &secondHalf); // Split the list into two sublists

    firstHalf = mergeSort(firstHalf);   // Recursively sort the left half
    secondHalf = mergeSort(secondHalf); // Recursively sort the right half

    return mergeSortedLists(firstHalf, secondHalf); // Merge the sorted sublists
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        fprintf(stderr, "Error opening file: %s\n", argv[1]);
        return 1;
    }
    int list_size;
    fscanf(input, "%d", &list_size);
    Node *head = (list_size > 0) ? (Node *)malloc(sizeof(Node)) : NULL;
    Node *cur = head;
    for (int i = 0; i < list_size; i++) {
        fscanf(input, "%d", &(cur->data));
        if (i + 1 < list_size)
            cur->next = (Node *)malloc(sizeof(Node));
        cur = cur->next;
    }
    fclose(input);

    //Block C : traverse 印出結果
    //Linked list sort -> 熟悉linked list的結構
    //把cur指向head，然後用while迴圈去traverse整個linked list
    head = mergeSort(head);

    cur = head;
    while (cur) {
        printf("%d ", cur->data);
        asm volatile(
            /*
            Block C (Move to the next node), which updates the pointer to
            traverse the linked list
            */
            "ld %[cur],8(%[cur])\n\t"
            : [cur] "+r"(cur));
            //cur = cur->next;

    }
    
    printf("\n");
    return 0;
}