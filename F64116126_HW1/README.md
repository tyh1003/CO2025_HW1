# Computer Organization  計算機組織 2025 Programming Assignment I

## 課程資訊
- **課程**: 成功大學資訊系 大二必修
- **學年**: 113學年度下學期
- **教授**: 涂嘉恒 (Tu, Chia-Heng)
- **作者**: MQ (GM 115)

## 作業簡介
[點我查看HW1作業說明](Computer%20Organization%202025%20Programming%20Assignment%20I.pdf)  
This assignment aims to get familiar with the hardware/software interface: instruction set
architecture (ISA), as well as the development environment and tools (e.g., compiler and
RISC-V simulator) for RISC-V programming. In particular, this programming assignment is to
write inline assembly code and run the developed code on a RISC-V ISA Simulator, Spike,
to obtain the simulation results.
The assignments of this course are based on the RISC-V Spike simulator running on Ubuntu
Linux 22.04. As our assignments are built upon open source projects, there will be
unexpected compatibility issues if you choose a different development environment.
This assignment consists of three exercises, with a total score of 100 points. You will be
given some test cases, and you must ensure that your code passes all these test cases,
referred to as public test cases. Please note that there will be hidden test cases as well.
Try to make your program handle special inputs properly. For details on how points are
assigned, see Scoring Criteria.

這份作業的目的是讓同學熟悉硬體/軟體介面，尤其是指令集架構（ISA），並且了解開發環境和工具（例如：編譯器和 RISC-V 模擬器）在 RISC-V 程式設計中的應用。具體來說，這次作業要求同學撰寫內聯組合語言程式碼，並在 RISC-V ISA 模擬器（Spike）上執行開發的程式碼，以獲得模擬結果。
本課程的作業是基於在 Ubuntu Linux 22.04 上運行的 RISC-V Spike 模擬器。如果你選擇了其他開發環境，可能會遇到不預期的相容性問題，因為我們的作業是建立在開源專案上的。
這份作業包含三個練習，總分 100 分。你將會收到一些測試案例，必須確保你的程式能通過所有這些測試案例，這些測試案例稱為「公開測試案例」。請注意，還會有隱藏測試案例。記得讓你的程式能正確處理特殊的輸入。詳細的計分標準請參考「計分標準」部分。

## Prerequisite 先備條件
- **按照 HW0 文件中列出的程序來設置你的開發環境。**
[點我查看安裝文檔](Homework%200%20-%20Environmental%20Setup.pdf)

如果你選擇在虛擬化環境中工作，而不從頭構建軟體，請參閱「A. 安裝已建置環境」以獲得進一步的指導。已建置的環境運行於 Ubuntu Linux 24.04.1，並且已安裝所需工具。
另外，如果你選擇從源代碼構建，則需要從相應的網站直接克隆源代碼專案，並從頭開始進行構建。請參閱「B. 從源代碼構建」以獲得詳細的步驟說明。
安裝完工具後，請參照「C. 測試 RISC-V 工具」指南來驗證工具是否成功安裝。
最後，「D. 程式設計作業練習」提供了 GNU GCC 內聯組合語言的教程以及相應的練習。建議學生在課程進行中練習這些練習，這些練習不會計分。
- **Method A:Install the pre-builded environment**
  - Step 1:Install VirtualBox (the virtual machine software)
  - Step 2:Import the prebuilt virtual machine image

- **Method B:Build from scratch**
  - Step 1:Ubuntu installation
  - Step 2:Install GNU Toolchain for RISC-V (GCC tools)
  - Step 3:Install the RISC-V ISA simulator (Spike)
  - Step 4: Install Proxy Kernel

- **Method C.Validate the RISC-V Toolchain Installation**

- **Method D.Practice for Programming Assignments**
  - Step 1:GCC Inline Assembly
  - Step 2:Programming Exercise

## Exercise 1 (Array Bubble Sort, 20%)
- **簡介**: Implement the bubble sort algorithm using inline
assembly to sort an array of integers.
使用組合語言，實作氣泡排序演算法，對一個整數陣列進行排序。
- **for 初學者**:
  - 流程 : 計算arr[j]的位址，並讀取值-->計算arr[j+1]的位址，並讀取值-->比較，然後交換
  - 暫存器 : "t0", "t1", "t2", "t5", "t6"
- **C code**:
```c
for (int i = 0; i < arr_size - 1; i++) {
        for (int j = 0; j < arr_size - i -1; j++) {
            if (*(p_a + j) > *(p_a + j + 1)) {
                int tmp = *(p_a + j);
                *(p_a + j) = *(p_a + j + 1);
                *(p_a + j + 1) = tmp;
            }
        }
    }
```
- **組合語言撰寫成果**:
```c
for (int i = 0; i < arr_size - 1; i++) {
        for (int j = 0; j < arr_size - i - 1; j++) {
            asm volatile(

                "slli t0, %[j], 2\n\t"       // t0 = j * 4（位移）
                "add t1, %[base], t0\n\t"    // t1 = p_a + j*4 = &arr[j]
                "lw t2, 0(t1)\n\t"           // t2 = arr[j]

                "addi t0, t0, 4\n\t"         // t0 = (j+1) * 4
                "add t5, %[base], t0\n\t"    // t5 = p_a + (j+1)*4 = &arr[j+1]
                "lw t6, 0(t5)\n\t"           // t6 = arr[j+1]

                "ble t2, t6, 1f\n\t"         // 若 arr[j] <= arr[j+1]，跳到標籤1:
                "sw t6, 0(t1)\n\t"           // arr[j] = arr[j+1]
                "sw t2, 0(t5)\n\t"           // arr[j+1] = 原本的 arr[j]
                
                "1:\n\t"                     // no_swap 的標籤，沒有要幹嘛

                :                            //似乎沒作用呢
                : [j] "r"(j), [base] "r"(p_a)//j 對應 t3，p_a 對應 base
                : "t0", "t1", "t2", "t5", "t6"//用到的暫存器，t4沒用到 
            ); 
        }
    }
```
- **測試成果**:
  - Correct/Total problems: 4/4、Obtained/Total scores: 20/20

## Exercise 2 (Array Search, 40%): 
- **簡介**: Implement the search algorithm using inline assembly
to find the index of a target value in an array.
使用組合語言，實作搜尋演算法，找到目標值在陣列中的索引。
- **for 初學者**:
  - 流程 : 初始化i-->迴圈讀值-->比對target-->i++-->如果超過迴圈數量就結束 or 如果都不是，繼續迴圈
  - 暫存器 : "t0", "t1", "t2", "t3"
- **C code**:
```c
for (int i = 0; i < arr_size; i++) {
        if (p_a[i] == target) {
            result = i;
            break;
        }
    }
```
- **組合語言撰寫成果**:
```c
    asm volatile(
        "li t3, 0\n\t"              //t3 = i

        "LOOP:\n\t"                 //迴圈的意思
        "slli t0, t3, 2\n\t"        //t0 = i * 4
        "add t1, %[base], t0\n\t"   //t1 = &p_a[i]
        "lw t2, 0(t1)\n\t"          //t2 = p_a[i]

        "beq t2,%[target],FOUND\n\t"//如果等於，就去FOUND

        "addi t3,t3,1\n\t"

        "bge t3,%[size],END\n\t"    //比較，若大於等於迴圈的大小 即結束

        "j LOOP\n\t"                // 否則跳回 LOOP

        "FOUND:\n\t"
        "sw t3,%[res]\n\t"

        "END:\n\t"

        :
        : [base] "r"(p_a), [target] "r"(target), [size] "r"(arr_size), [res] "m"(result)
        : "t0", "t1", "t2", "t3"
    );
```
- **測試成果**:
  - 原有的2份測試資料:Correct/Total problems: 2/2、Obtained/Total scores: 20/20
  - 另新增11份測試資料:Correct/Total problems: 13/13、Obtained/Total scores: 20/20

## Exercise 3 (Linked-List Merge Sort, 40%):
- **簡介**: Implement merge sort for a linked list using
inline assembly. You will need to split the list, merge sorted lists, and move between
nodes
使用組合語言，實作鏈結串列的合併排序。你需要將串列拆分、合併已排序的串列並在節點之間進行移動。
- **for初學者**
  - Block A : *將A->B->C->D變成A->B和C->D*  
  初始化快慢指標slow和fast-->開始走、找中間點-->斷開、分為前半段及後半段  
  使用暫存器 : "t0", "t1", "t2", "t3", "memory"
  - Block B : *將A->B和C->D合併成A->B->C->D*
  主要的合併迴圈-->比較當前節點(a->data、b->data)-->選擇a or b作為新節點-->將選擇的節點接到 tail-->初始化 result 和 tail-->將剩下的 a 或 b 接到 tail
  使用暫存器 : "t2", "t4", "t5", "memory"
  - Block C : *把cur指向head，然後用while迴圈去traverse整個linked list*
- **C code**
  - Block A : 
  ```c
  void splitList(Node *head, Node **firstHalf, Node **secondHalf) {
    Node *slow = head;
    Node *fast = head ? head->next : NULL;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    *firstHalf = head;        
    *secondHalf = slow->next; 
    slow->next = NULL;        
  }
  ```
  - Block B : 
  ```c
  Node *mergeSortedLists(Node *a, Node *b) {
    Node *result = NULL; 
    Node *tail = NULL; 

    while (a && b) {
        Node *selected = NULL;
        if (a->data < b->data) {
            selected = a;
            a = a->next;
        } else {
            selected = b;
            b = b->next;
        }

        if (!result) {  
            result = selected;
            tail = selected;
        } else {
            tail->next = selected;
            tail = selected;
        }
    }

    if (a) {
        tail->next = a;
    } else if (b) {
        tail->next = b;
    }

    return result;
  }
  ```
  - Block C : 
  ```c
  cur = cur->next;
  ```
- **組合語言撰寫成果**:
  - Block A :
  ```c
  asm volatile(
        "mv t0, %[head]\n\t"        //t0 = slow
        "ld t1, 8(%[head])\n\t"     //t1 = fast = head->next
  
        "LOOP_SPLIT:\n\t"
        "beqz t1, END_SPLIT\n\t"    //if (fast == NULL) break
        "ld t2, 8(t1)\n\t"          //t2 = fast->next
        "beqz t2, END_SPLIT\n\t"    //if (fast->next == NULL) break
        "ld t0, 8(t0)\n\t"          //slow = slow->next
        "ld t1, 8(t2)\n\t"          //fast = fast->next->next
        "j LOOP_SPLIT\n\t"
        "END_SPLIT:\n\t"
        
        "sd %[head], 0(%[first])\n\t"//firstHalf = head
        "ld t3, 8(t0)\n\t"           //t3 = slow->next    
        "sd t3, 0(%[second])\n\t"    //secondHalf = slow->next
        "sd x0, 8(t0)\n\t"           //slow->next = NULL

        :
        : [head] "r"(head), [first] "r"(firstHalf), [second] "r"(secondHalf)
        : "t0", "t1", "t2", "t3", "memory"
  );
  ```
  - Block B :
  **特別注意 : 有可能出現節點一次的部分，所以要判斷另一段是否為null**
  ```c
  asm volatile(
        "LOOP_MERGE:\n\t"
        "beqz %[a], APPEND_B_MERGE\n\t"         // if a == NULL, jump to b
        "beqz %[b], APPEND_A_MERGE\n\t"         // if b == NULL, jump to a

        "ld t4, 0(%[a])\n\t"                    // t4 = a->data
        "ld t5, 0(%[b])\n\t"                    // t5 = b->data
        "blt t4, t5, SELECT_A_MERGE\n\t"        // if (a < b) goto SELECT_A
        "j SELECT_B_MERGE\n\t"
        
        "SELECT_A_MERGE:\n\t"
        "mv t2, %[a]\n\t"                       // selected = a
        "ld %[a], 8(%[a])\n\t"                  // a = a->next
        "j CONTINUE_MERGE\n\t"

        "SELECT_B_MERGE:\n\t"
        "mv t2, %[b]\n\t"                       // selected = b
        "ld %[b], 8(%[b])\n\t"                  // b = b->next

        "CONTINUE_MERGE:\n\t"
        "beqz %[result], INIT_MERGE\n\t"        // if result == NULL
        "sd t2, 8(%[tail])\n\t"                 // tail->next = selected
        "mv %[tail], t2\n\t"                    // tail = selected
        "j LOOP_MERGE\n\t"

        "INIT_MERGE:\n\t"
        "mv %[result], t2\n\t"                  // result = selected
        "mv %[tail], t2\n\t"                    // tail = selected
        "j LOOP_MERGE\n\t"

        "APPEND_A_MERGE:\n\t"
        "beqz %[tail], SET_RESULT_A\n\t"
        "sd %[a], 8(%[tail])\n\t"
        "j END_MERGE\n\t"

        "SET_RESULT_A:\n\t"          ////為了避免只有一次節點存在////
        "mv %[result], %[a]\n\t"
        "j END_MERGE\n\t"

        "APPEND_B_MERGE:\n\t"        ////判斷null////
        "beqz %[tail], SET_RESULT_B\n\t"
        "sd %[b], 8(%[tail])\n\t"
        "j END_MERGE\n\t"

        "SET_RESULT_B:\n\t"
        "mv %[result], %[b]\n\t"
        "END_MERGE:\n\t"
        :
        [a] "+r"(a), [b] "+r"(b), [result] "+r"(result), [tail] "+r"(tail)
        :
        : "t2", "t4", "t5", "memory"
  );
  ```
  - Block C :
  ```c
  asm volatile(
        "ld %[cur],8(%[cur])\n\t"
        : [cur] "+r"(cur)
  );
  ```

- **測試成果**:
  - 原有的2份測試資料:Correct/Total problems: 2/2、Obtained/Total scores: 20/20
  - 另新增13份測試資料:Correct/Total problems: 15/15、Obtained/Total scores: 20/20

## 成長軌跡 -- 關於第一堂資工的課
哈囉，我是測量系大三的學生MQ，我是一直到最近才有想修資工系雙輔的想法，然後我英文很爛，所以README打中文。  
我這學期才開始修資工的課，所以計組HW1就是我在資工的第一份作業，可說是資工系小白。 (但應該不算程式小白?在本系修很多程式課www) 。接下來來講講寫第一份作業及我在資工系修課這幾周的心路歷程。  
首先，目前為止六周，資工系真的很酷，有著大系和熱門科系的光環，課程設計很完整、老師也都很風趣、和善，系館很大很漂亮。可能也因為這樣，課程對於我來說難度不是特別高，稍微花點心力就能應付和聽懂 (考試就另當別論了www) ，作業和考試的難度也非常合理、說明清楚，這些總總都是我們系做不到的地方 (測量系) ，甚至望塵莫及，所以最近有點變心了，抱歉了測量w。
第一份作業和ChatGPT付費版一起完成，GPT提示、我理解和撰寫的方式完成，程式完成的時間大約4~8小時，滿少的，；README是第一次打，打很久，但打得很開心。整體來說自己做測試資料花最久時間 (還沒弄出來w) ，其他部分都好好玩，下次還玩。
最後講一下自我期許好了 (打README突然就沒有中文能力了。) ，這學期希望搞好資工和本系的成績!!然後申請到雙主修。其實大四一整年也是要很拚成績，然後目標是大五往上推資工相關 (能到偏所就好了 真的XD) 。
OK，以上(by MQ 2025/03/25)

## 參考資料
- **Chapter 1: Computer Abstractions and Technology**  
  *出自國立成功大學資訊工程學系 Tu Chia-Heng 教授之課堂講義*    
- **Chapter 2: Instructions: Language of the Computer**  
  *出自國立成功大學資訊工程學系 Tu Chia-Heng 教授之課堂講義*    
- [HW0 安裝說明](Homework%200%20-%20Environmental%20Setup.pdf)  
- [HW1 作業講義](Computer%20Organization%202025%20Programming%20Assignment%20I.pdf) 
- **Computer Organization and Design: RISC-V Edition (Second Edition)**  
*David A. Patterson & John L. Hennessy*  出版社：Morgan Kaufmann
- **ChatGPT Plus (GPT-4, OpenAI)** 協助提示與內容整理
