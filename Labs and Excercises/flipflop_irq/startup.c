/*
 * 	startup.c
 *
 */
#include "gpio.h"
#include "syscfg.h"
#include "exti.h"

#define EXERCISE 6

#define EXTI_3_BPOS 0x8
#define NVIC *((volatile unsigned int *) 0xE000E100)

#define IRQ0 0x1
#define IRQ1 0x2
#define IRQ2 0x4
#define IRQ 0x8
#define RST0 0x10
#define RST1 0x20
#define RST2 0x40

void startup(void) __attribute__((naked)) __attribute__((section (".start_section")) );
void init_app(void);
void interrupt_handler(void);
void reset_irq_pin(unsigned char);
void irq0_handler(void);
void irq1_handler(void);
void irq2_handler(void);

static volatile int counter = 0;

void startup ( void ){
__asm volatile(
	" LDR R0,=0x2001C000\n"		/* set stack */
	" MOV SP,R0\n"
	" BL main\n"				/* call main */
	"_exit: B .\n"				/* never return */
	) ;
}

void main(void){
    app_init();
#if EXERCISE == 4
    while(1){
        GPIO_D.odrLow = counter;
		int y = 0;
    }
#endif // exercise 4

#if EXERCISE > 4
    GPIO_D.odrHigh = 0;
    while(1){
        GPIO_D.odrLow = counter;
    }
#endif // exercise 5 or 6
}

void interrupt_handler(void){
#if EXERCISE == 4
    if (EXTI.pr & EXTI_3_BPOS){
        counter++;
        EXTI.pr |= EXTI_3_BPOS; //denna bit nollställs då den ettställs. makes perfect sense.
    }
#endif // exercise 4

#if EXERCISE == 5
    unsigned char exti_pr = EXTI.pr;
    if(exti_pr & IRQ){
        if(GPIO_E.idrLow & IRQ0){
            reset_irq_pin(RST0);
            counter++;
        }
        if(GPIO_E.idrLow & IRQ1){
            reset_irq_pin(RST1);
            counter = 0;
        }
        if(GPIO_E.idrLow & IRQ2){
            reset_irq_pin(RST2);
            GPIO_D.odrHigh ^= 0xFF;
        }
        EXTI.pr |= IRQ;
    }
#endif // exercise 5
}

void irq0_handler(void){
    if(EXTI.pr & IRQ0){
        reset_irq_pin(RST0);
        counter++;
        EXTI.pr |= IRQ0;
    }
}

void irq1_handler(void){
    if(EXTI.pr & IRQ1){
        reset_irq_pin(RST1);
        counter = 0;
        EXTI.pr |= IRQ1;
    }
}

void irq2_handler(void){
    if(EXTI.pr & IRQ2){
        reset_irq_pin(RST2);
        GPIO_D.odrHigh ^= 0xFF;
        EXTI.pr |= IRQ2;
    }
}

void reset_irq_pin(unsigned char pin){
    // Resets an IRQ-pin by turning its reset value on and off again
    GPIO_E.odrLow |= pin;
    GPIO_E.odrLow &= ~pin;
}

void app_init(void){
#ifdef USBDM
    // Start clocks for port D & E
    *((unsigned long*)0x40023830) = 0x18;
    // Start clocks for SYSCFG
    *((unsigned long*)0x40023844) |= 0x4000;
    // Relocate the vector table
    *((unsigned long*)0xE000ED08) = 0x2001C000;
#endif // USBDM

    // Set up GPIO
    GPIO_E.moder &= 0xFFFF0000;
    GPIO_E.moder |= 0x00001500;
    
#if EXERCISE == 4
    GPIO_D.moder &= 0xFFFF0000;
    GPIO_D.moder |= 0x00005555;
#else
    GPIO_D.moder = 0x55555555;
#endif // Different values for GPIO_D.moder based on exercise
    
#if EXERCISE < 6
    // Reset EXTI3, then connect port E to EXTI3, that means PE3 becomes an interrupt pin
    
	//fungerar
	SYSCFG.exticr &= 0x0FFF;
	SYSCFG.exticr |= 0x4000;
	
	//fungerar också
	//SYSCFG.exticr1 &= 0x0FFF;
	//SYSCFG.exticr1 |= 0x4000;

    // Configure EXTI3 for interruptions on negative flank
    EXTI.imr |= EXTI_3_BPOS;
	EXTI.ftsr |= EXTI_3_BPOS;
	EXTI.rtsr &= ~EXTI_3_BPOS;
	
    // Set up interrupt vector
    *((void (**) (void))0x2001C064) = interrupt_handler;
    
    // Enable interrupt
    NVIC |= (1<<9);
#endif // exercise 4 or 5

#if EXERCISE == 6
    SYSCFG.exticr1 &= 0xF000;
    SYSCFG.exticr |= 0x0444;
    // Configure EXTI3 for interruptions on negative flank
    EXTI.imr |= IRQ0 + IRQ1 + IRQ2;
	EXTI.ftsr &= ~(IRQ0 + IRQ1 + IRQ2);
	EXTI.rtsr |= IRQ0 + IRQ1 + IRQ2;
    
    // Set up interrupt vector
    *((void (**) (void))0x2001C058) = irq0_handler;
    *((void (**) (void))0x2001C05C) = irq1_handler;
    *((void (**) (void))0x2001C060) = irq2_handler;
    
    // Enable interrupt
    NVIC |= (1<<6) + (1<<7) + (1<<8);
#endif // exercise 6
}
