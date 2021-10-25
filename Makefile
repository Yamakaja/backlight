set_brightness: set_brightness.c
	$(CC) $^ -o $@ -static -lm -Wextra -Wall -O3 -ffast-math -fno-pie

set_brightness_dbg: set_brightness.c
	$(CC) $^ -o $@ -static -lm -Wextra -Wall -ffast-math -fno-pie -DDEBUG -g -Og
