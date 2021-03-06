/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

#define better_TRAPHANDLER(name, num)						\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

#define better_TRAPHANDLER_NOEC(name, num)					\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */

better_TRAPHANDLER_NOEC(vector0,0)
better_TRAPHANDLER_NOEC(vector1,1)
better_TRAPHANDLER_NOEC(vector2,2)
better_TRAPHANDLER_NOEC(vector3,3)
better_TRAPHANDLER_NOEC(vector4,4)
better_TRAPHANDLER_NOEC(vector5,5)
better_TRAPHANDLER_NOEC(vector6,6)
better_TRAPHANDLER_NOEC(vector7,7)
better_TRAPHANDLER(vector8,8)
better_TRAPHANDLER_NOEC(vector9,9)
better_TRAPHANDLER(vector10,10)
better_TRAPHANDLER(vector11,11)
better_TRAPHANDLER(vector12,12)
better_TRAPHANDLER(vector13,13)
better_TRAPHANDLER(vector14,14)
better_TRAPHANDLER(vector15,15)
better_TRAPHANDLER_NOEC(vector16,16)
better_TRAPHANDLER_NOEC(vector17,17)
better_TRAPHANDLER_NOEC(vector18,18)
better_TRAPHANDLER_NOEC(vector19,19)
better_TRAPHANDLER_NOEC(vector20,20)
better_TRAPHANDLER_NOEC(vector21,21)
better_TRAPHANDLER_NOEC(vector22,22)
better_TRAPHANDLER_NOEC(vector23,23)
better_TRAPHANDLER_NOEC(vector24,24)
better_TRAPHANDLER_NOEC(vector25,25)
better_TRAPHANDLER_NOEC(vector26,26)
better_TRAPHANDLER_NOEC(vector27,27)
better_TRAPHANDLER_NOEC(vector28,28)
better_TRAPHANDLER_NOEC(vector29,29)
better_TRAPHANDLER_NOEC(vector30,30)
better_TRAPHANDLER_NOEC(vector31,31)
better_TRAPHANDLER_NOEC(vector32,32)
better_TRAPHANDLER_NOEC(vector33,33)
better_TRAPHANDLER_NOEC(vector34,34)
better_TRAPHANDLER_NOEC(vector35,35)
better_TRAPHANDLER_NOEC(vector36,36)
better_TRAPHANDLER_NOEC(vector37,37)
better_TRAPHANDLER_NOEC(vector38,38)
better_TRAPHANDLER_NOEC(vector39,39)
better_TRAPHANDLER_NOEC(vector40,40)
better_TRAPHANDLER_NOEC(vector41,41)
better_TRAPHANDLER_NOEC(vector42,42)
better_TRAPHANDLER_NOEC(vector43,43)
better_TRAPHANDLER_NOEC(vector44,44)
better_TRAPHANDLER_NOEC(vector45,45)
better_TRAPHANDLER_NOEC(vector46,46)
better_TRAPHANDLER_NOEC(vector47,47)
better_TRAPHANDLER_NOEC(vector48,48)
better_TRAPHANDLER_NOEC(vector49,49)
better_TRAPHANDLER_NOEC(vector50,50)
better_TRAPHANDLER_NOEC(vector51,51)
better_TRAPHANDLER_NOEC(vector52,52)
better_TRAPHANDLER_NOEC(vector53,53)
better_TRAPHANDLER_NOEC(vector54,54)


/*
 * Lab 3: Your code here for _alltraps
 */
_alltraps:
//;set up the trapframe structure
  pushl %ds
  //pushl $0x69 //padding
  pushl %es
  pushal
  movw $GD_KD,%ax
  movw %ax,%ds
  movw %ax,%es
  pushl %esp
  call trap

.data
.globl vectors
vectors:
  .long vector0
  .long vector1
  .long vector2
  .long vector3
  .long vector4
  .long vector5
  .long vector6
  .long vector7
  .long vector8
  .long vector9
  .long vector10
  .long vector11
  .long vector12
  .long vector13
  .long vector14
  .long vector15
  .long vector16
  .long vector17
  .long vector18
  .long vector19
  .long vector20
  .long vector21
  .long vector22
  .long vector23
  .long vector24
  .long vector25
  .long vector26
  .long vector27
  .long vector28
  .long vector29
  .long vector30
  .long vector31
  .long vector32
  .long vector33
  .long vector34
  .long vector35
  .long vector36
  .long vector37
  .long vector38
  .long vector39
  .long vector40
  .long vector41
  .long vector42
  .long vector43
  .long vector44
  .long vector45
  .long vector46
  .long vector47
  .long vector48 //lol syscalls
  .long vector49
  .long vector50
  .long vector51
  .long vector52
  .long vector53
  .long vector54
