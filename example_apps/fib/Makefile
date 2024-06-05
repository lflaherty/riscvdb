AS=riscv64-elf-as
CC=riscv64-elf-gcc
LD=riscv64-elf-ld

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
	rm -f $(OUTPUT)
