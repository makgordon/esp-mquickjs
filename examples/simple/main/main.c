#include "esp_log.h"

#include "esp_mqjs.h"

static const char *TAG = "example";

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

void app_main(void)
{
    ESP_LOGI(TAG, "Starting JavaScript runtime...");
    
    esp_mqjs_run_script(script);
}
