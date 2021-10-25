#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>

#define BRIGHTNESS "/sys/class/backlight/amdgpu_bl0/brightness"
#define MAX_BRIGHTNESS "/sys/class/backlight/amdgpu_bl0/max_brightness"
#define ALPHA 3

int global_argc;
char **global_argv;

int file_to_int(const char *filename, int *out) {
    FILE* ifile = fopen(filename, "r");    
    if (!ifile) {
        fprintf(stderr, "%s: Failed to open %s: %s\n", global_argv[0], filename, strerror(errno));
        return -EINVAL;
    }
    char buffer[65];
    ssize_t ret = fread(buffer, 1, sizeof(buffer)-1, ifile);
    if (ret == 0) {
        fprintf(stderr, "%s: Failed to read %s: ret = %ld\n", global_argv[0], filename, ret);
        fclose(ifile);
        return -EINVAL;
    };
    buffer[ret] = '\0';

    fclose(ifile);

    *out = strtol(buffer, NULL, 10);
    if (errno) {
        fprintf(stderr, "%s: Failed to convert \"%s\"to int: %s\n", global_argv[0], buffer, strerror(errno));
        return -EINVAL;
    }
    return 0;
}

int int_to_file(const char *filename, int val) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "%s: Failed to open %s: %s\n", global_argv[0], filename, strerror(errno));
        return -EINVAL;
    }

    int ret = fprintf(file, "%d\n", val);
    if (ret < 0) {
        fprintf(stderr, "%s: Failed to write to file %s: %s\n", global_argv[0], filename, strerror(errno));
        fclose(file);
        return -EINVAL;
    }

    fclose(file);
    return 0;
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))
int get_new_brightness(int current_brightness_i, int max_brightness_i, float alpha, int delta_i) {
    float current_brightness = (float) current_brightness_i / (float) max_brightness_i;
#ifdef DEBUG
    printf("current_brightness: %f\n", current_brightness);
#endif
    float linear_brightness = logf(current_brightness*(expf(alpha) - 1) + 1) / alpha;
#ifdef DEBUG
    printf("linear_brightness: %f\n", linear_brightness);
#endif
    linear_brightness += delta_i / 100.f;
    linear_brightness = MIN(linear_brightness, 1.f);
    linear_brightness = MAX(linear_brightness, 0.f);
#ifdef DEBUG
    printf("new linear_brightness: %f\n", linear_brightness);
#endif

    float new_brightness = (expf(alpha*linear_brightness) - 1.f) / (expf(alpha) - 1.f);
#ifdef DEBUG
    printf("new_brightness: %f\n", new_brightness);
#endif
    return (int) roundf(new_brightness*max_brightness_i);
}
#undef MAX
#undef MIN

int main(int argc, char *argv[]) {
    global_argc = argc;
    global_argv = argv;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s [+-]%%\n", argv[0]);
        return 1;
    }

    int max_brightness;
    int brightness;

    int ret = file_to_int(MAX_BRIGHTNESS, &max_brightness);
    if (ret)
        return -ret;

    ret = file_to_int(BRIGHTNESS, &brightness);
    if (ret)
        return -ret;

    long delta = strtol(argv[1], NULL, 10);
    if (delta < -100 || delta > 100) {
        fprintf(stderr, "%s: Invalid delta %ld, must be between -100 and 100!", argv[0], delta);
        return 1;
    }

    int new_brightness = get_new_brightness(brightness, max_brightness, ALPHA, delta);
    return int_to_file(BRIGHTNESS, new_brightness);
}
