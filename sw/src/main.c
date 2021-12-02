/*
Copyright (C) 2021 Antmicro

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

SPDX-License-Identifier: Apache-2.0
*/

#include <stdio.h>
#include <ctype.h>

#include <generated/soc.h>
#include <generated/csr.h>
#include <generated/mem.h>
#include <irq.h>

#include "tusb.h"
#include "usb_acm.h"
#include "usb_hid.h"
#include "usb_dfu.h"

#include "rgb.h"
#include "touch.h"
#include "flash.h"
#include "mode.h"
#include "utils.h"
#include "script.h"


volatile uint32_t system_ticks = 0;
volatile uint8_t rgb_ticks = 0;

void isr(void);
void loop(void);


static void timer_init(void)
{
	int t;

    timer0_en_write(0);
    t = CONFIG_CLOCK_FREQUENCY / 1000; // 1000 kHz tick
    timer0_reload_write(t);
    timer0_load_write(t);
    timer0_en_write(1);
    timer0_ev_enable_write(1);
    timer0_ev_pending_write(1);
    irq_setmask(irq_getmask() | (1 << TIMER0_INTERRUPT));
}

void isr(void)
{
    unsigned int irqs;

    irqs = irq_pending() & irq_getmask();

#if CFG_TUSB_RHPORT0_MODE == OPT_MODE_DEVICE
    if (irqs & (1 << USB_INTERRUPT)) {
        tud_int_handler(0);
    }
#endif
    if (irqs & (1 << TIMER0_INTERRUPT)) {
        system_ticks++;
        timer0_ev_pending_write(1);
    }
}

#if CFG_TUSB_OS == OPT_OS_NONE
uint32_t board_millis(void)
{
    return system_ticks;
}
#endif

void loop(void)
{
    work_mode_t mode = work_mode_get();

    switch (mode) {
        case WORK_MODE_PROG:
            acm_task();
            break;
        case WORK_MODE_EXEC:
            hid_task();
            break;
        default:
            break;
    }

    /* Change the combination of colors on the LED to the next one
     * to indicate the program still works
     */
    // rgb_ticks++;
    // if (rgb_ticks > 7) rgb_ticks = 0;
    // rgb_output_write(rgb_ticks);

    /* Button handling */
    if (touch_is_left()) rgb_set(rgb_get() | RGB_RED);
    else rgb_set(rgb_get() & (~RGB_RED));

    if (touch_is_right()) rgb_set(rgb_get() | RGB_BLUE);
    else rgb_set(rgb_get() & (~RGB_BLUE));

    return;
}

int main(int argc, char *argv[])
{
    work_mode_t mode = WORK_MODE_EXEC;

    rgb_set(RGB_RED);

    irq_setmask(0);
    irq_setie(1);
    timer_init();

    /* Set mode depending on touched button */
    if (touch_is_right()) {
        mode = WORK_MODE_PROG;
        rgb_set(RGB_CYAN);
    }

    /* Wait until user stops touching device */
    while (touch_is_right()) {}

    work_mode_set(mode);

    rgb_set(RGB_YELLOW);

    /* Initialize USB stack */
    tusb_init();

    /* Hook tinyUSB CDC-ACM to stdin/stdout */
    if (mode == WORK_MODE_PROG) {
        acm_flush(NULL);
    }

    flash_init();

    rgb_set(RGB_GREEN);

    /*** MAIN LOOP ***/
    for (;;) {
        tud_task();                /* USB stack task */
        if (tud_ready()) loop();   /* Application task */
    }

    return 0;
}
