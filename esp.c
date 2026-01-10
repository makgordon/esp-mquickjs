#include <time.h>
#include <sys/time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "mquickjs.h"

static const char *TAG = "esp-mqjs";

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

static JSValue js_gc(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    JS_GC(ctx);
    return JS_UNDEFINED;
}

static int64_t get_time_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000 + (tv.tv_usec / 1000);
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

/* load a script */
static JSValue js_load(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv) { return JS_UNDEFINED; }
static JSValue js_setTimeout(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv) { return JS_UNDEFINED; }
static JSValue js_clearTimeout(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv) { return JS_UNDEFINED; }

#include "esp_stdlib.h"

static void js_log_func(void *opaque, const void *buf, size_t buf_len)
{
    fwrite(buf, 1, buf_len, stdout);
}

static int js_interrupt_handler(JSContext *ctx, void *opaque)
{
#ifdef ESP_PLATFORM
    static int64_t last_time = 0;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t cur_time = (int64_t)tv.tv_sec * 1000 + (tv.tv_usec / 1000);
    // Yield every 100ms to let the IDLE task run and reset the watchdog
    if (cur_time - last_time > 100) {
        vTaskDelay(1); 
        last_time = cur_time;
    }
#endif
    return 0;
}

void esp_mqjs_run_script(const char *script)
{
    size_t mem_size;
    uint8_t *mem_buf;
    JSContext *ctx;
    JSValue val;

    mem_size = 16 * 1024; // 16KB; // TODO: Kconfig option, test running in PSRAM
    mem_buf = malloc(mem_size);
    if (!mem_buf) {
        ESP_LOGE(TAG, "Failed to allocate memory");
        return;
    }

    ctx = JS_NewContext(mem_buf, mem_size, &js_stdlib);
    JS_SetLogFunc(ctx, js_log_func);
    JS_SetInterruptHandler(ctx, js_interrupt_handler);
    
    val = JS_Eval(ctx, script, strlen(script), "<input>", 0);
    if (JS_IsException(val)) {
        JSValue obj;
        obj = JS_GetException(ctx);
        JS_PrintValueF(ctx, obj, JS_DUMP_LONG);
        printf("\n");
    }

    JS_FreeContext(ctx);
    free(mem_buf);
}
