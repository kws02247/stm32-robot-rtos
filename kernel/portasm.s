.syntax unified
.cpu cortex-m4
.thumb


.global PendSV_Handler


.extern scheduler_save_current_sp
.extern scheduler_switch_next


.section .text.PendSV_Handler, "ax", %progbits

.thumb_func
PendSV_Handler:

    /*
     * Hardware already saved on PSP:
     *
     * R0
     * R1
     * R2
     * R3
     * R12
     * LR
     * PC
     * xPSR
     */


    /*
     * Get current task PSP.
     */
    mrs r0, psp


    /*
     * Save R4-R11 manually.
     */
    stmdb r0!, {r4-r11}


    /*
     * Save EXC_RETURN.
     *
     * Push 8 bytes so MSP remains
     * 8-byte aligned before calling C.
     */
    push {r3, lr}


    /*
     * r0 = current task saved SP
     */
    bl scheduler_save_current_sp


    /*
     * Select next task.
     *
     * Return:
     * r0 = next task saved SP
     */
    bl scheduler_switch_next


    /*
     * Restore EXC_RETURN.
     */
    pop {r3, lr}


    /*
     * Restore next task R4-R11.
     */
    ldmia r0!, {r4-r11}


    /*
     * r0 now points to the
     * hardware exception frame.
     */
    msr psp, r0


    /*
     * Exception return.
     *
     * Cortex-M restores:
     *
     * R0-R3
     * R12
     * LR
     * PC
     * xPSR
     */
    bx lr