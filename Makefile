set_brightness: set_brightness.c
	$(CC) $^ -o $@ -static -lm -Wextra -Wall -O3 -ffast-math -fno-pie
