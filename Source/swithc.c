__asm void PendSV_Handler(void)
{
    IMPORT blockPtr
    LDR R0, =blockPtr
    LDR R0, [R0]
    LDR R0, [R0]            // R0 = blockPtr.stackPtr. R0中保存的就是任务栈的栈顶,也就是数组stackBuffer的最后一个元素+1的位置
                            // 例如：R0 = 0x20000040
    STMDB R0!, {R4-R11}     // 将R11,R10,R9,R8,R7,R6,R5,R4依次保存到数组stackBuffer中, R0的值为最新值
                            // 例如：R0 = 0x20000020
    LDR R1, =blockPtr
    LDR R1, [R1]
    STR R0, [R1]            // 将最新的栈顶指针写入到blockPtr中
	                        // 例如：blockPtr = 0x20000020

    ADD R4, R4, #1
    ADD R5, R5, #1

    LDMIA R0!, {R4-R11}     // 恢复寄存器的值
	
	NOP

    BX LR
}
