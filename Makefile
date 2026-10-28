CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy


CFLAGS=-mcpu=cortex-m4 -mthumb -Wall -Wextra -O0 -ffreestanding -nostdlib
LDFLAGS=-T linker.ld -nostdlib -Wl,-Map=kernel.map


OBJS= \
	startup/startup_stm32.o \
	app/main.o \
	app/robot_task_stub.o \
	drivers/uart.o \
	drivers/gpio.o \
	drivers/systick.o \
	kernel/rtos.o \
	kernel/task.o \
	kernel/scheduler.o \
	kernel/port.o \
	kernel/portasm.o


all: kernel.elf kernel.bin


kernel.elf: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o kernel.elf


kernel.bin: kernel.elf
	$(OBJCOPY) -O binary kernel.elf kernel.bin


%.o: %.c
	$(CC) $(CFLAGS) -I drivers -I kernel -I app -c $< -o $@


%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(OBJS) kernel.elf kernel.bin kernel.map