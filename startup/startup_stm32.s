.syntax unified
.cpu cortex-m4
.thumb


.global Reset_Handler
.global Default_Handler
.global SVC_Handler


.extern main

.extern _estack
.extern _sidata
.extern _sdata
.extern _edata
.extern _sbss
.extern _ebss

.extern SysTick_Handler
.extern PendSV_Handler

.extern rtos_get_first_task_sp


/*
 * =========================================================
 * Cortex-M Vector Table
 * =========================================================
 */
.section .isr_vector, "a", %progbits


.word _estack

.word Reset_Handler

.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler

.word 0
.word 0
.word 0
.word 0

.word SVC_Handler

.word Default_Handler

.word 0

.word PendSV_Handler

.word SysTick_Handler


/*
 * =========================================================
 * Reset Handler
 * =========================================================
 */
.section .text.Reset_Handler, "ax", %progbits

.thumb_func
Reset_Handler:


    /*
     * Copy .data from FLASH to RAM.
     */
    ldr r0, =_sidata
    ldr r1, =_sdata
    ldr r2, =_edata


copy_data:

    cmp r1, r2

    bge zero_bss


    ldr r3, [r0], #4

    str r3, [r1], #4


    b copy_data


/*
 * Clear .bss.
 */
zero_bss:

    ldr r0, =_sbss

    ldr r1, =_ebss

    movs r2, #0


clear_bss_loop:

    cmp r0, r1

    bge call_main


    str r2, [r0], #4


    b clear_bss_loop


/*
 * Enter C main().
 */
call_main:

    bl main


/*
 * main() must never return.
 */
reset_hang:

    b reset_hang


/*
 * =========================================================
 * SVC Handler
 *
 * Starts Task 0 using PSP.
 * =========================================================
 */
.section .text.SVC_Handler, "ax", %progbits

.thumb_func
SVC_Handler:


    /*
     * Get Task 0 saved SP.
     */
    bl rtos_get_first_task_sp


    /*
     * Restore R4-R11.
     */
    ldmia r0!, {r4-r11}


    /*
     * PSP now points at
     * hardware exception frame.
     */
    msr psp, r0


    /*
     * Thread mode uses PSP.
     *
     * CONTROL.SPSEL = 1
     */
    movs r0, #2

    msr control, r0

    isb


    /*
     * Exception return:
     *
     * Thread mode
     * PSP
     */
    ldr lr, =0xFFFFFFFD


    bx lr


/*
 * =========================================================
 * Default Handler
 * =========================================================
 */
.section .text.Default_Handler, "ax", %progbits

.thumb_func
Default_Handler:

    b Default_Handler