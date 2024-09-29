#include <msp430.h>
#include <time.h>
// �w�q�C�q��ܾ����q��
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

unsigned long seed = 1; // ��l�ؤl

void srand(unsigned long new_seed) {
    seed = new_seed;
}

unsigned int rand(void) {
    seed = (LCG_A * seed + LCG_C) & LCG_M;
    return (unsigned int)(seed >> 16) & 0x7FFF; // ��^�� 15 ��@���H����
}

// �w�q�Ʀr0��9�b�C�q��ܾ��W����ܭ�
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

#define DELAY_MS 1000 // ����ɶ��A�H�@�����

// ���s1�˴����
int IBP1(void) {
    __delay_cycles(20000);
    return !(P4IN & BIT5); // �p�G���s���U�A��^1�F�_�h��^0
}

// ���s2�˴����
int IBP2(void) {
    __delay_cycles(20000);
    return !(P4IN & BIT6); // �p�G���s���U�A��^1�F�_�h��^0
}

// ���s3�˴����
int IBP3(void) {
    __delay_cycles(20000);
    return !(P4IN & BIT7); // �p�G���s���U�A��^1�F�_�h��^0
}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // ����ݪ����p�ɾ�
    PM5CTL0 &= ~LOCKLPM5;       // �T�� GPIO �������ܼҦ��A�H�ҥκݤf�]�m

    // �]�m P1 �M P3 ����X
    P1DIR |= (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G);
    P3DIR |= (SEG_A | SEG_B | SEG_0 | SEG_D | SEG_E | SEG_F | SEG_G);

    // ��l�� P1 �M P3 ���C
    P1OUT &= ~(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G);
    P3OUT &= ~(SEG_A | SEG_B | SEG_0 | SEG_D | SEG_E | SEG_F | SEG_G);

    P2DIR |= (BIT0 | BIT1 | BIT2); // �]�m P2 ����X
    P2OUT = 0x00; // ��l�� P2 ���C

    P4DIR &= ~(BIT5 | BIT6 | BIT7); // �N���s�}��]�m����J
    P4REN |= (BIT5 | BIT6 | BIT7);  // �ҥΤ����W�Թq��
    P4OUT |= (BIT5 | BIT6 | BIT7);  // �]�m���W�ԡA�Ϩ�O�����q��

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
        // ��ܭp�ƾ��`��
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

        // ��ܪ�l�{�{
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
            // �˴����s1
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
            // �˴����s2
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
            // �˴����s3
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

        // �ˬd���ר���ܵ��G
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

        // ��ܿ��~�{�{
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
