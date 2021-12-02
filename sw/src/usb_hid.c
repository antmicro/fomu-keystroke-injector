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

#include "tusb.h"

#include "script.h"
#include "touch.h"
#include "rgb.h"
#include "utils.h"
#include "usb_hid.h"


//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

script_exec_state_t exec_state = SCRIPT_EXEC_IDLE;
int result = 0;

void hid_task(void)
{
    // Poll every 10ms
    static uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;
    uint32_t const btn = touch_is_left();

    if (board_millis() - start_ms < interval_ms) return; // not enough time
    start_ms += interval_ms;

    // Remote wakeup
    if (tud_suspended()) {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        if (btn) tud_remote_wakeup();
        else return;
    }

    if (!tud_hid_n_ready(ITF_KEYBOARD)) return;

    switch (exec_state) {
        case SCRIPT_EXEC_IDLE:
            if (btn) {
                script_rewind();
                exec_state = SCRIPT_EXEC_TOKEN;
            }
            break;
        case SCRIPT_EXEC_TOKEN:
            rgb_set(RGB_RED);
            result = script_exec();
            if (result) exec_state = SCRIPT_EXEC_EMPTY;
            else exec_state = SCRIPT_EXEC_IDLE;
            break;
        case SCRIPT_EXEC_EMPTY:
            rgb_set(RGB_YELLOW);
            if (result > 0) script_report_keystroke(0);
            exec_state = SCRIPT_EXEC_TOKEN;
            break;
        default:
            break;
    }
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    // TODO not Implemented
    (void) itf;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    // TODO set LED based on CAPLOCK, NUMLOCK etc...
    (void) itf;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) bufsize;
}
