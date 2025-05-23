#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int   trail_length;
    int   trail_thickness;
    int   color;
    int   pos_count;
    int   cursor_size;
    float cursor_auto_hide;
    float refresh_rate;
    float mouse_refresh_rate;
    char  mouse_separate_thread;
    int   mouse_interpolation_factor;
    int   dither;
    int   mouse_empty_area;

    char type_trail;
    char type_dots;
    char cursor_blob;

    char quit;
} ConfigArgs;

// TODO: add cursor-size and cursor-auto-hide

void print_help() {
    printf(
        "Usage: xtrail [OPTIONS]\n"
        "Options:\n"
        "  --help                       Display this help message\n"
        "  --trail-length <length>      Set trail length to <length>\n"
        "  --trail-thickness <px>       Set trail thickness to <px>\n"
        "  --color <hex>                Set color to <hex> (e.g. 0x7F7F7F)\n"
        "  --mouse-hcount <count>       Set position history count to <count>\n"
        "  --cursor-size <size>         Set cursor size"
        "  --cursor-auto-hide <0|rate>  Set if and how quickly shall cursor disappear"
        "  --refresh-rate <fps>         Set refresh rate count to <fps>\n"
        "  --mouse-refresh-rate <hz>    Set mouse refresh rate count to <hz> (e.g. 240.00)\n"
        "  --no-dither                  Disable dithering\n"
        "  --mouse-share-thread         Synchronous rendering and mouse pooling\n"
        "  --mouse-smooth-factor <val>  Adjusts the smoothness of mouse movements (e.g. 0 or 2)\n"
        "  --mouse-empty-area <px>      Sets area around mouse which is not obfuscated by trails\n"
        "Render type options:\n"
        "  --trail                      Render trail \"trail\" type\n"
        "  --dots                       Render trail \"dots\" type\n"
        "  --cursor-blob                Render cursor \"blob\" type\n");
}

const char *helpStrs[] = {"--help", "-help", "help", "--h", "-h", "h", "--?", "-?", "?", NULL};

ConfigArgs parseArgs(int argc, char *argv[]) {
    ConfigArgs config;
    config.trail_length = 800;
    config.trail_thickness = 8;
    config.color = 0x7F7F7F;
    config.pos_count = 60;
    config.cursor_size = 20;
    config.cursor_auto_hide = 15;
    config.refresh_rate = -1;
    config.mouse_refresh_rate = 250;
    config.dither = 1;
    config.mouse_interpolation_factor = 0;
    config.mouse_separate_thread = 1;
    config.mouse_empty_area = 15;
    config.quit = 0;

    config.type_trail = 0;
    config.type_dots = 0;
    config.cursor_blob = 0; // remove as default

    char typeHasSet = 0;

    for (int i = 1; i < argc; i++) {
        for (int j = 0; helpStrs[j] != NULL; j++) {
            if (strcmp(argv[i], helpStrs[j]) == 0) {
                print_help();
                config.quit = 1;
                return config;
            }
        }
        if (strcmp(argv[i], "--mouse-hcount") == 0) {
            config.pos_count = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--trail-length") == 0) {
            config.trail_length = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--trail-thickness") == 0) {
            config.trail_thickness = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--color") == 0) {
            config.color = strtol(argv[++i], NULL, 0);
        } else if (strcmp(argv[i], "--cursor-auto-hide") == 0) {
            config.cursor_auto_hide = atof(argv[++i]);
        } else if (strcmp(argv[i], "--refresh-rate") == 0) {
            config.refresh_rate = atof(argv[++i]);
        } else if (strcmp(argv[i], "--mouse-refresh-rate") == 0) {
            config.mouse_refresh_rate = atof(argv[++i]);
        } else if (strcmp(argv[i], "--no-dither") == 0) {
            config.dither = 0;
        } else if (strcmp(argv[i], "--mouse-smooth-factor") == 0) {
            config.mouse_interpolation_factor = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--mouse-share-thread") == 0) {
            config.mouse_separate_thread = 0;
        } else if (strcmp(argv[i], "--mouse-empty-area") == 0) {
            config.mouse_empty_area = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--trail") == 0) {
            config.type_trail = typeHasSet = 1;
        } else if (strcmp(argv[i], "--dots") == 0) {
            config.type_dots = typeHasSet = 1;
        } else {
            printf("Warning [parseArgs]: Unknown argument \"%s\", check \"--help\".\n", argv[i]);
        }
    }

    if (!typeHasSet) config.type_trail = 1;

    return config;
}
