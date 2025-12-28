#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "mquickjs.h"

// Helper for time functions
static int64_t get_time_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000 + (tv.tv_usec / 1000);
}

// -------------------------------------------------------------------------
// JS Function Definitions required by mqjs_stdlib.h
// These must be defined BEFORE including "mqjs_stdlib.h"
// -------------------------------------------------------------------------

static JSValue js_print(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    int i;
    for(i = 0; i < argc; i++) {
        if (i != 0) putchar(' ');
        JS_PrintValueF(ctx, argv[i], JS_DUMP_LONG);
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
    printf("Initializing mquickjs...\n");

    size_t mem_size = 64 * 1024; // 64KB
    void *mem_buf = malloc(mem_size);
    if (!mem_buf) {
        printf("Failed to allocate memory\n");
        return;
    }

    // Initialize context with the standard library definition from mqjs_stdlib.h
    JSContext *ctx = JS_NewContext(mem_buf, mem_size, &js_stdlib);
    JS_SetLogFunc(ctx, js_log_func);

    const char *script = "print('Hello from mquickjs!'); var a = 1+2; print('1+2=', a); print('Time:', Date.now());";
    printf("Evaluating script: %s\n", script);

    JSValue val = JS_Eval(ctx, script, strlen(script), "<input>", 0);

    if (JS_IsException(val)) {
        JSValue obj = JS_GetException(ctx);
        JS_PrintValueF(ctx, obj, JS_DUMP_LONG);
        printf("\n");
    } else {
        printf("Script executed successfully.\n");
    }

    JS_FreeContext(ctx);
    free(mem_buf);
    printf("Done.\n");
}
