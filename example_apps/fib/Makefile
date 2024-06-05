CC=riscv32-unknown-elf-gcc
AS=riscv32-unknown-elf-as
LD=riscv32-unknown-elf-ld

ASFLAGS=-march=rv32i
CFLAGS=-march=rv32i -mabi=ilp32
LDFLAGS=-m elf32lriscv
TEXT=0x00000000
DATA=0x00010000

APP_NAME=fib

all: $(APP_NAME).elf

$(APP_NAME).elf: start.o $(APP_NAME).o
	$(LD) $(LDFLAGS) -Ttext $(TEXT) -Tdata $(DATA) -o $@ $^

$(APP_NAME).o: $(APP_NAME).c
	$(CC) $(CFLAGS) -c -o $@ $^

start.o: start.s
	$(AS) $(ASFLAGS) -o $@ $^

clean:
	rm -f start.o $(APP_NAME).o
	rm -f $(APP_NAME).elf
