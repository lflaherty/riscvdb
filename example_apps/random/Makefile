AS=riscv64-unknown-elf-as
CC=riscv64-unknown-elf-gcc
LD=riscv64-unknown-elf-ld

CFLAGS=-march=rv32i -mabi=ilp32

APP_NAME=rand

all: $(APP_NAME).elf

$(APP_NAME).elf: $(APP_NAME).o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(APP_NAME).o
	rm -f $(APP_NAME).elf
