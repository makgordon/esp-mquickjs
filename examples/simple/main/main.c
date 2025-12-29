#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "mquickjs.h"

static const char *TAG = "example";

const size_t JS_MEMORY_SIZE = 10 * 1024; // 10KB
const char *script = 
    "/* Mandelbrot set */\n"
    "function mandelbrot(center_x, center_y, scale, w, h, max_it) {\n"
    "    var x1, y1, y2, i, x, y, cx, cy, fx, fy, t, c, s, c0;\n"
    "    var colors = [ 14, 15, 7, 8, 0, 4, 12, 5, 13, 1, 9, 3, 11, 10, 2, 6];\n"
    "    fx = scale * 0.5 / Math.min(w, h);\n"
    "    fy = fx * 2;\n"
    "    for(y1 = 0; y1 < h; y1++) {\n"
    "        s = \"\";\n"
    "        for(x1 = 0; x1 < w; x1++) {\n"
    "            for(y2 = 0; y2 < 2; y2++) {\n"
    "                cx = (x1 - w * 0.5) * fx + center_x;\n"
    "                cy = (y1 + y2 * 0.5 - h * 0.5) * fy + center_y;\n"
    "                x = 0; y = 0;\n"
    "                for(i = 0; i < max_it && x * x + y * y < 4; i++) {\n"
    "                    t = x * x - y * y + cx;\n"
    "                    y = 2 * x * y + cy;\n"
    "                    x = t;\n"
    "                }\n"
    "                if (i >= max_it) c = 0;\n"
    "                else c = colors[i % colors.length];\n"
    "                if (y2 == 0) c0 = c;\n"
    "            }\n"
    "            // Use ANSI colors and unicode block character\n"
    "            s += \"\\x1b[\" + (c0 >= 8 ? 82 + c0 : 30 + c0) + \";\" + (c >= 8 ? 92 + c : 40 + c) + \"m\\u2580\";\n"
    "        }\n"
    "        s += \"\\x1b[0m\";\n"
    "        print(s);\n"
    "    }\n"
    "}\n"
    "\n"
    "print('Generating Mandelbrot set...');\n"
    "var start = Date.now();\n"
    "mandelbrot(-0.75, 0.0, 2.0, 80, 25, 50);\n"
    "print('Time: ' + (Date.now() - start) + 'ms');\n";

// Helper for time functions
static int64_t get_time_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000 + (tv.tv_usec / 1000);
}

static int js_interrupt_handler(JSContext *ctx, void *opaque)
{
    static int64_t last_time = 0;
    int64_t cur_time = get_time_ms();
    // Yield every 100ms to let the IDLE task run and reset the watchdog
    if (cur_time - last_time > 100) {
        vTaskDelay(1); 
        last_time = cur_time;
    }
    return 0;
}

// -------------------------------------------------------------------------
// JS Function Definitions required by mqjs_stdlib.h
// These must be defined BEFORE including "mqjs_stdlib.h"
// -------------------------------------------------------------------------

static JSValue js_print(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    int i;
    JSValue v;
    
    for(i = 0; i < argc; i++) {
        if (i != 0)
            putchar(' ');
        v = argv[i];
        if (JS_IsString(ctx, v)) {
            JSCStringBuf buf;
            const char *str;
            size_t len;
            str = JS_ToCStringLen(ctx, &len, v, &buf);
            fwrite(str, 1, len, stdout);
        } else {
            JS_PrintValueF(ctx, argv[i], JS_DUMP_LONG);
        }
    }
    putchar('\n');
    return JS_UNDEFINED;
}

static JSValue js_date_now(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return JS_NewInt64(ctx, (int64_t)tv.tv_sec * 1000 + (tv.tv_usec / 1000));
}

static JSValue js_performance_now(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    return JS_NewInt64(ctx, get_time_ms());
}

static JSValue js_gc(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    JS_GC(ctx);
    return JS_UNDEFINED;
}

static JSValue js_load(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv) { return JS_UNDEFINED; }
static JSValue js_setTimeout(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv) { return JS_UNDEFINED; }
static JSValue js_clearTimeout(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv) { return JS_UNDEFINED; }

// -------------------------------------------------------------------------
// Include the standard library definition
// -------------------------------------------------------------------------
#include "mqjs_stdlib.h"

// -------------------------------------------------------------------------
// Main Application
// -------------------------------------------------------------------------

static void js_log_func(void *opaque, const void *buf, size_t buf_len)
{
    fwrite(buf, 1, buf_len, stdout);
}

void app_main(void)
{
    uint8_t *mem_buf;
    JSContext *ctx;
    JSValue val;

    mem_buf = malloc(JS_MEMORY_SIZE);
    if (!mem_buf) {
        ESP_LOGE(TAG, "Failed to allocate memory");
        return;
    }

    ctx = JS_NewContext(mem_buf, JS_MEMORY_SIZE, &js_stdlib);
    JS_SetLogFunc(ctx, js_log_func);
    JS_SetInterruptHandler(ctx, js_interrupt_handler);

    val = JS_Eval(ctx, script, strlen(script), "<input>", 0);
    if (JS_IsException(val)) {
        JSValue obj = JS_GetException(ctx);
        JS_PrintValueF(ctx, obj, JS_DUMP_LONG);
        printf("\n");
    }
    ESP_LOGI(TAG, "Script executed successfully.");

    JS_FreeContext(ctx);
    free(mem_buf);
}
