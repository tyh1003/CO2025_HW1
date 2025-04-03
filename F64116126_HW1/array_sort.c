#include <stdio.h>

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
    fclose(input);

    int *p_a = &arr[0];

    // array a bubble sort
    /* Original C code segment
    for (int i = 0; i < arr_size - 1; i++) {
        for (int j = 0; j < arr_size - i -1; j++) {
            if (*(p_a + j) > *(p_a + j + 1)) {
                int tmp = *(p_a + j);
                *(p_a + j) = *(p_a + j + 1);
                *(p_a + j + 1) = tmp;
            }
        }
    }
    */

    for (int i = 0; i < arr_size - 1; i++) {
        for (int j = 0; j < arr_size - i - 1; j++) {
            asm volatile(
                //2025.3.22 by TSAI and GPT
                // t0 = j * 4（計算 arr[j] 的位元組偏移量）
                // t1 = &arr[j]（t1 = p_a + j*4）
                // t2 = arr[j] 的值
                // t5 = &arr[j+1]（t5 = p_a + (j+1)*4）
                // t6 = arr[j+1] 的值

                // Step1.計算arr[j]的位址，並讀取值
                "slli t0, %[j], 2\n\t"       // t0 = j * 4（位移）
                "add t1, %[base], t0\n\t"    // t1 = p_a + j*4 = &arr[j]
                "lw t2, 0(t1)\n\t"           // t2 = arr[j]


                // Step2.計算arr[j+1]的位址，並讀取值
                "addi t0, t0, 4\n\t"         // t0 = (j+1) * 4
                "add t5, %[base], t0\n\t"    // t5 = p_a + (j+1)*4 = &arr[j+1]
                "lw t6, 0(t5)\n\t"           // t6 = arr[j+1]
                
                // Step3.比較，然後交換
                "ble t2, t6, 1f\n\t"         // 若 arr[j] <= arr[j+1]，跳到標籤1:
                "sw t6, 0(t1)\n\t"           // arr[j] = arr[j+1]
                "sw t2, 0(t5)\n\t"           // arr[j+1] = 原本的 arr[j]
                
                "1:\n\t"                     // no_swap 的標籤，沒有要幹嘛

                :                            //似乎沒作用呢
                : [j] "r"(j), [base] "r"(p_a)//j 對應 t3，p_a 對應 base
                : "t0", "t1", "t2", "t5", "t6"//用到的暫存器，t4沒用到
                //success 
            ); 
          
        }
    }
    p_a = &arr[0];
    for (int i = 0; i < arr_size; i++)
        printf("%d ", *p_a++);
    printf("\n");
    return 0;
}
