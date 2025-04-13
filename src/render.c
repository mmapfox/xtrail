#pragma once

#include <X11/Xlib.h>

#include "./args.c"
#include "./global.c"
#include "./mouse.c"
#include "./render_methods.c"
void printBinary(int n) {
    // Assuming a 32-bit integer
    for (int i = sizeof(n) * 8 - 1; i >= 0; i--) {
        // Use bitwise AND to check if the ith bit is set
        int bit = (n >> i) & 1;
        printf("%d", bit);
    }
    printf("\n");
}
void render_cursor(ConfigArgs *args, Canvas *c) {
    // hide area around mouse
    rFCircle(c, c->mouse->state->p, args->mouse_empty_area, -1);

    if (args->cursor_size==0) return;

    float visibility = 1;
    float maskhold = 1;
    for (int i = 0; i < c->mouse->listc; ++i) {
        MouseState mstat = mouseState(c->mouse, i);
        maskhold += !!(0x700 & mstat.mask); // check for mouse clicks
    }
    maskhold /= c->mouse->listc;

    Point p_current = mouseState(c->mouse, 0).p;
    Point p_cursor_mid = { .x  = 0, .y = 0 };
    Point p_weighted = { .x  = 0, .y = 0 };
    for (int i = 0; i < c->mouse->listc; ++i) {
        Point p = mouseState(c->mouse, i).p;
        p_weighted.x += p.x;
        p_weighted.y += p.y;
    }
    p_weighted.x /= c->mouse->listc;
    p_weighted.y /= c->mouse->listc;
    p_cursor_mid.x += args->cursor_size * 3 * (1 - maskhold) + p_weighted.x;
    p_cursor_mid.y += args->cursor_size * 3 + p_weighted.y;

    int dx = p_cursor_mid.x - p_current.x;
    int dy = p_cursor_mid.y - p_current.y;
    double distance = sqrt(dx * dx + dy * dy);
    if (distance==0) distance = 1;
    double scale_factor = scale_factor = args->cursor_size / distance;

    p_cursor_mid.x = p_current.x + (int)round(dx * scale_factor);
    p_cursor_mid.y = p_current.y + (int)round(dy * scale_factor);


    if (args->cursor_blob) {
        // void rTaperedGradLine(Canvas *c, Point point1, float color1, float thickness1, Point point2, float color2, float thickness2) {
        visibility = 1 - c->mouse->lastMove / args->cursor_auto_hide + 5;
        if (visibility>1) visibility = 1;
        if (visibility>0) {
            rTaperedGradLine(c, p_current, 1, 1, p_cursor_mid, 1,
                             args->cursor_size * (visibility - maskhold * 0.5));
        }
    }
}

void render(ConfigArgs *args, Canvas *c) {
    // do not render mouse if it's currently hidden
    if (c->mouse->hidden || c->mouse->forbidRenderTicks > 0 || c->mouse->listc == 0) return;

    float distTotal = 0;
    float distPrecalc[c->mouse->listc - 1];
    for (int i = 0; i < c->mouse->listc - 1; ++i) {
        distPrecalc[i] = pointGetDistance(mouseState(c->mouse, i).p, mouseState(c->mouse, i + 1).p);
        distTotal += distPrecalc[i];
    }
    float distMax = MIN(distTotal, args->trail_length);

    if (args->type_trail) {
        float distCurrent = 0;
        for (int i = 0; i < c->mouse->listc - 1; ++i) {
            if (distCurrent >= args->trail_length) continue;
            Point p1 = mouseState(c->mouse, i).p;
            Point p2 = mouseState(c->mouse, i + 1).p;
            float dist = distPrecalc[i];
            if (dist == 0) continue;
            if (dist + distCurrent > args->trail_length) {
                float allowedLength = args->trail_length - distCurrent;
                p2 = pointInterpolate(p1, p2, 1 - (dist - allowedLength) / dist);
            }
            float g1 = distCurrent / distMax;
            distCurrent += dist;
            float g2 = distCurrent / distMax;
            if (g1 > 1 || g2 > 1 || g1 < 0 || g2 < 0) continue; // division may lead to very big numbers, so let's ignore such lines
            rTaperedGradLine(c, p1, 1 - g1, args->trail_thickness - g1 * args->trail_thickness, p2, 1 - g2,
                             args->trail_thickness - g2 * args->trail_thickness);
        }
    }

    if (args->type_dots) {
        float distCurrent = 0;
        for (int i = 0; i < c->mouse->listc; ++i) {
            if (distCurrent >= args->trail_length) continue;
            Point p = mouseState(c->mouse, i).p;
            float dist = distPrecalc[i];
            distCurrent += dist;
            float g = distCurrent / distMax;
            rRadialGrad(c, p, (1 - g) * (args->trail_thickness - 1), 0, 1 - g);
        }
    }

    render_cursor(args, c);
}
