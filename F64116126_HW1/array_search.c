#include <stdio.h>

int arraySearch(int *p_a, int arr_size, int target)
{
    int result = -1;
    /*
    in C
    for (int i = 0; i < arr_size; i++) {
        if (p_a[i] == target) {
            result = i;
            break;
        }
    }

    */
    asm volatile(
        //2025.3.22 by TSAI and GPT

        //Step1:初始化i
        "li t3, 0\n\t"              //t3 = i

        //Step2:迴圈讀值
        "LOOP:\n\t"                 //迴圈的意思
        "slli t0, t3, 2\n\t"        //t0 = i * 4
        "add t1, %[base], t0\n\t"   //t1 = &p_a[i]
        "lw t2, 0(t1)\n\t"          //t2 = p_a[i]

        //Step3.比對target
        "beq t2,%[target],FOUND\n\t"//如果等於，就去FOUND

        //Step4.i++
        "addi t3,t3,1\n\t"

        //Step5.如果超過迴圈數量就結束
        "bge t3,%[size],END\n\t"    //比較，若大於等於迴圈的大小 即結束

        //Step6.如果都不是 繼續迴圈
        "j LOOP\n\t"                // 否則跳回 LOOP

        //Step7-1.FOUND
        "FOUND:\n\t"
        "sw t3,%[res]\n\t"

        //Step7-2.END
        "END:\n\t"

        :
        : [base] "r"(p_a), [target] "r"(target), [size] "r"(arr_size), [res] "m"(result)
        : "t0", "t1", "t2", "t3"
    );

    return result;
}

// Main function to test the implementation
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
    int arr_size;
    fscanf(input, "%d", &arr_size);
    int arr[arr_size];

    // Read integers from input file into the array
    for (int i = 0; i < arr_size; i++) {
        int data;
        fscanf(input, "%d", &data);
        arr[i] = data;
    }
    int target;
    fscanf(input, "%d", &target);
    fclose(input);

    int *p_a = &arr[0];

    int index = arraySearch(p_a, arr_size, target);

    // Print the result
    printf("%d ", index);
    printf("\n");

    return 0;
}
