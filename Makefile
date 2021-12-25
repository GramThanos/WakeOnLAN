WakeOnLAN: WakeOnLAN.c
	$(CC) -o $@ $<

clean:
	rm WakeOnLAN

all: WakeOnLAN

.PHONY: WakeOnLan all
