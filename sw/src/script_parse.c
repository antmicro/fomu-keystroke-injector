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

#include "tusb.h"

#include "usb_hid.h"
#include "storage.h"
#include "script_parse.h"

size_t script_cursor = 0;

void script_rewind(void)
{
    script_cursor = 0;
}

uint16_t script_next_token(void)
{
    storage_partition_t part = storage_get_partition(0);
    uint16_t *script_arr = (uint16_t *)(SPIFLASH_BASE + part.start);
    uint16_t script_op;

    script_op = script_arr[script_cursor];
    printf("stor_tok: [%4x]: %04x\r\n", script_cursor, script_op);

    if (script_op != UINT16_MAX) script_cursor++;

    return script_op;
}

uint8_t script_check_delay(uint16_t token)
{
    const uint8_t keycode = token & UINT8_MAX;
    const uint8_t modifier = token >> 8;

    /* Report no key pressed if there's no keystroke */
    if (!keycode) {
        /* If key is empty, then modifier byte contains
         * delay before the next token (in milliseconds).
         * Delays bigger than 255 ms are split by encoder
         * into multiple delay tokens.
         */
        return modifier;
    }

    return 0;
}

void script_print_keystroke(uint16_t token)
{
    uint8_t const conv_table[128][2] = { HID_KEYCODE_TO_ASCII };
    const uint8_t keycode = token & UINT8_MAX;
    const uint8_t modifier = token >> 8;
    char ch = ((modifier & KEYBOARD_MODIFIER_LEFTSHIFT) || (modifier & KEYBOARD_MODIFIER_RIGHTSHIFT)) ? conv_table[keycode][1] : conv_table[keycode][0];

    printf("%02x + %c\r\n", modifier, ch);
}

void script_report_keystroke(uint16_t token)
{
    uint8_t keycode[6] = { (token & UINT8_MAX) };
    const uint8_t modifier = token >> 8;

    /* Check if keycode is not reserved */
    if ((keycode[0] > HID_KEY_GUI_RIGHT)
        || ( (HID_KEY_EXSEL < keycode[0]) && (keycode[0] < HID_KEY_CONTROL_LEFT) )
        ) {
        /* Reserved */
    }
    else {
        tud_hid_keyboard_report(ITF_KEYBOARD, modifier, keycode);
    }

    return;
}
