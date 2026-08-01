CFLAGS = -Wall -Wextra

WakeOnLAN: WakeOnLAN.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f WakeOnLAN

all: WakeOnLAN

.PHONY: WakeOnLAN all clean
