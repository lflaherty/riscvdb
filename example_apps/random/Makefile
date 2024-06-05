CC=riscv32-unknown-elf-gcc

CFLAGS=-march=rv32i -mabi=ilp32 -lc

APP_NAME=rand

all: $(APP_NAME).elf

$(APP_NAME).elf: $(APP_NAME).c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(APP_NAME).elf
