#include <msp430.h>
#include <time.h>
// 定義七段顯示器的段位
#define SEG_0 BIT0
#define SEG_A BIT1
#define SEG_B BIT2
#define SEG_C BIT3
#define SEG_D BIT4
#define SEG_E BIT5
#define SEG_F BIT6
#define SEG_G BIT7
#define LCG_A 1103515245
#define LCG_C 12345
#define LCG_M 0x7FFFFFFF

unsigned long seed = 1; // 初始種子

void srand(unsigned long new_seed) {
    seed = new_seed;
}

unsigned int rand(void) {
    seed = (LCG_A * seed + LCG_C) & LCG_M;
    return (unsigned int)(seed >> 16) & 0x7FFF; // 返回高 15 位作為隨機數
}

// 定義數字0到9在七段顯示器上的顯示值
const unsigned char SEGT[10] = {
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,           // 0
    SEG_B | SEG_C,                                           // 1
    SEG_A | SEG_B | SEG_G | SEG_E | SEG_D,                   // 2
    SEG_A | SEG_B | SEG_G | SEG_C | SEG_D,                   // 3
    SEG_F | SEG_G | SEG_B | SEG_C,                           // 4
    SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,                   // 5
    SEG_A | SEG_F | SEG_G | SEG_C | SEG_D | SEG_E,           // 6
    SEG_A | SEG_B | SEG_C,                                   // 7
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,   // 8
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G            // 9
};

const unsigned char SEG3[10] = {
    SEG_A | SEG_B | SEG_0 | SEG_D | SEG_E | SEG_F,           // 0
    SEG_B | SEG_0,                                           // 1
    SEG_A | SEG_B | SEG_G | SEG_E | SEG_D,                   // 2
    SEG_A | SEG_B | SEG_G | SEG_0 | SEG_D,                   // 3
    SEG_F | SEG_G | SEG_B | SEG_0,                           // 4
    SEG_A | SEG_F | SEG_G | SEG_0 | SEG_D,                   // 5
    SEG_A | SEG_F | SEG_G | SEG_0 | SEG_D | SEG_E,           // 6
    SEG_A | SEG_B | SEG_0,                                   // 7
    SEG_A | SEG_B | SEG_0 | SEG_D | SEG_E | SEG_F | SEG_G,   // 8
    SEG_A | SEG_B | SEG_0 | SEG_D | SEG_F | SEG_G            // 9
};

const unsigned char TP2[3] = {
    BIT0,   // 0
    BIT1,   // 1
    BIT2    // 2
};

#define DELAY_MS 1000 // 延遲時間，以毫秒為單位

// 按鈕1檢測函數
int IBP1(void) {
    __delay_cycles(20000);
    return !(P4IN & BIT5); // 如果按鈕按下，返回1；否則返回0
}

// 按鈕2檢測函數
int IBP2(void) {
    __delay_cycles(20000);
    return !(P4IN & BIT6); // 如果按鈕按下，返回1；否則返回0
}

// 按鈕3檢測函數
int IBP3(void) {
    __delay_cycles(20000);
    return !(P4IN & BIT7); // 如果按鈕按下，返回1；否則返回0
}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // 停止看門狗計時器
    PM5CTL0 &= ~LOCKLPM5;       // 禁用 GPIO 的高阻抗模式，以啟用端口設置

    // 設置 P1 和 P3 為輸出
    P1DIR |= (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G);
    P3DIR |= (SEG_A | SEG_B | SEG_0 | SEG_D | SEG_E | SEG_F | SEG_G);

    // 初始化 P1 和 P3 為低
    P1OUT &= ~(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G);
    P3OUT &= ~(SEG_A | SEG_B | SEG_0 | SEG_D | SEG_E | SEG_F | SEG_G);

    P2DIR |= (BIT0 | BIT1 | BIT2); // 設置 P2 為輸出
    P2OUT = 0x00; // 初始化 P2 為低

    P4DIR &= ~(BIT5 | BIT6 | BIT7); // 將按鈕腳位設置為輸入
    P4REN |= (BIT5 | BIT6 | BIT7);  // 啟用內部上拉電阻
    P4OUT |= (BIT5 | BIT6 | BIT7);  // 設置為上拉，使其保持高電平

    srand(time(0));
    unsigned int i, j, k, t, tar, ans,times;
    for (;;) {
        times = 0;
        j = 0;
        k = 0;
        i = 0;
        t = 0;
        tar = rand();
        tar = tar % 100;
        P1OUT = 0x00;
        P3OUT = 0x00;
        // 顯示計數器循環
        for (i = 0; i < 2; i++) {
            P2OUT = TP2[i];
            for (t = 0; t < 10; t++) {
                P1OUT = SEGT[t];
                P3OUT = SEG3[t];
                P2OUT ^= TP2[i];
                __delay_cycles(130000);
            }
            __delay_cycles(70000);
        }

        // 顯示初始閃爍
        for (i = 0; i < 3; i++) {
            P1OUT = 0x00;
            P3OUT = 0x00;
            P2OUT = (BIT0 | BIT1 | BIT2);
            __delay_cycles(170000);
            P1OUT = SEGT[0];
            P3OUT = SEG3[0];
            P2OUT = 0x00;
            __delay_cycles(170000);
        }
        for(;;){
        P1OUT = SEGT[j];
        P3OUT = SEG3[k];
        P2OUT = (BIT0 | BIT1 | BIT2);
        __delay_cycles(110000);
        P2OUT = 0x00;
        for (;;) {
            __delay_cycles(90000);
            // 檢測按鈕1
            if (IBP1()) {
                __delay_cycles(100000);
                j++;
                if (j == 10) {
                    j = 0;
                    k++;
                    if (k == 10) {
                        k = 0;
                    }
                }
                P1OUT = SEGT[j];
                P3OUT = SEG3[k];
                __delay_cycles(50000);
            }
            // 檢測按鈕2
            if (IBP2()) {
                __delay_cycles(100000);
                k++;
                if (k == 10) {
                    k = 0;
                }
                P1OUT = SEGT[j];
                P3OUT = SEG3[k];
                __delay_cycles(50000);
            }
            // 檢測按鈕3
            if (IBP3()) {
                __delay_cycles(100000);
                times++;
                P1OUT = SEGT[j];
                P3OUT = SEG3[k];
                ans = k * 10 + j;
                break;
            }
            if (j == 10) {
                j = 0;
            }
            if (k == 10) {
                k = 0;
            }
        }

        // 檢查答案並顯示結果
        srand(ans+12658);
        if (ans == tar) {
            for (i = 0; i < 3; i++) {
                P1OUT = 0x00;
                P3OUT = 0x00;
                P2OUT = BIT0;
                __delay_cycles(170000);
                P1OUT = BIT7;
                P3OUT = BIT7;
                P2OUT = 0x00;
                __delay_cycles(170000);
            }
            __delay_cycles(970000);
            break;
        } else if (ans > tar) {
            P2OUT = BIT1;
            __delay_cycles(970000);
        } else {
            P2OUT = BIT2;
            __delay_cycles(970000);
        }

        // 顯示錯誤閃爍
        for (i = 0; i < 3; i++) {
            P1OUT = SEG_A | SEG_D | SEG_G;
            P3OUT = SEG_A | SEG_D | SEG_G;
            __delay_cycles(170000);
            P1OUT = 0x00;
            P3OUT = 0x00;
            __delay_cycles(170000);
        }
        P1OUT = SEGT[j];
        P3OUT = SEG3[k];
        P2OUT = 0x00;
        if(times == 7){
            for (i = 0; i < 5; i++) {
                P1OUT = SEG_A | SEG_D | SEG_G;
                P3OUT = SEG_A | SEG_D | SEG_G;
                P2OUT = (BIT0 | BIT1 | BIT2);
                __delay_cycles(170000);
                P1OUT = 0x00;
                P3OUT = 0x00;
                P2OUT = 0x00;
                __delay_cycles(170000);
            }
            break;
        }
    }
}
}
