#include <stdint.h>
#include "SysTick.h"
#include "msp432p401r.h"

struct State{
    uint32_t Out;
    uint32_t Time;
    uint32_t WalkOut;
    const struct State *Next[16];
};

typedef const struct State STyp;

#define SENSOR  (*((volatile uint8_t*)0x40004C40))
#define LIGHT   (*((volatile uint8_t*)0x40004C23))
#define wLIGHT  (*((volatile uint8_t*)0x40004C03))

//Street Lights
#define goN     &FSM[0]
#define waitN   &FSM[1]
#define goE     &FSM[2]
#define waitE   &FSM[3]
//Walking Button
#define walkN   &FSM[4]
#define walkE   &FSM[5]
#define walkN2  &FSM[6]
#define walkE2  &FSM[7]
//Walking Faster
#define walk    &FSM[8]
#define walkF1  &FSM[9]
#define walkF2  &FSM[10]
#define walkF3  &FSM[11]
#define walkF4  &FSM[12]
#define walkF5  &FSM[13]
#define walkF6  &FSM[14]
#define walkF7  &FSM[15]

STyp FSM[16]={
             {0x21,3000,0x1,{goN,waitN,goN,waitN,walkN,walkN,walkN,walkN}},             //goN
             {0x22,500,0x1,{goE,goE,goE,goE,walkN,walkN,walkN,walkN}},                  //waitN
             {0x0C,3000,0x1,{goE,goE,waitE,waitE,waitE,waitE,waitE,waitE}},             //goE
             {0x14,500,0x1,{goN,goN,goN,goN,walkE,walkE,walkE,walkE}},                  //waitE

             {0x21,500,0x1,{goN,waitN,goN,waitN,walkN2,walkN2,walkN2,walkN2}},          //walkN
             {0x14,500,0x1,{goE,goE,waitE,waitE,walkE2,walkE2,walkE2,walkE2}},          //walkE
             {0x22,500,0x1,{walk,walk,walk,walk,walk,walk,walk,walk}},                  //walkN2
             {0x14,500,0x1,{walk,walk,walk,walk,walk,walk,walk,walk}},                  //walkE2

             {0x24,3000,0x2,{walkF1,walkF1,walkF1,walkF1,walkF1}},                      //walk
             {0x24,500,0x1,{walkF2,walkF2,walkF2,walkF2,walkF2}},                       //walkF1
             {0x24,500,0x0,{walkF3,walkF3,walkF3,walkF3,walkF3}},
             {0x24,500,0x1,{walkF4,walkF4,walkF4,walkF4,walkF4}},
             {0x24,500,0x0,{walkF5,walkF5,walkF5,walkF5,walkF5}},
             {0x24,500,0x1,{walkF6,walkF6,walkF6,walkF6,walkF6}},
             {0x24,500,0x0,{walkF7,walkF7,walkF7,walkF7,walkF7}},
             {0x24,1000,0x1,{goN,goE,goN,goN,goN,goN,goE,goN}}                          //walkF7
};

void main(void){
    STyp *Pt;
    uint32_t Input;

    SysTick_Init();
//    Port2_Init(RED);
    P2->SEL0 &= ~0x01;
    P2->SEL1 &= ~0x01;
    P2->DIR |= 0x03;
//    Port4_Init();
    P4->SEL0 &= ~0x3F;
    P4->SEL1 &= ~0x3F;
    P4->DIR |= 0x3F;
//    Port5_Init();
    P5->SEL0 &= ~0x07;
    P5->SEL1 &= ~0x07;
    P5->DIR &= ~0x07;

    Pt = goN;

    while(1){
        LIGHT = (LIGHT&~0x3F)|(Pt->Out); // set lights
        wLIGHT = (wLIGHT&~0x03)|(Pt->WalkOut);
        SysTick_Wait10ms(Pt->Time);
        Input = (SENSOR&0x07);  // read sensors
        Pt = Pt->Next[Input];
    }
}
