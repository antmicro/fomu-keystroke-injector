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
#include "script.h"
#include "flash.h"
#include "usb_acm.h"


FILE acm_io = FDEV_SETUP_STREAM(acm_putchar, acm_getchar, acm_flush, _FDEV_SETUP_RW);
struct __file * const __iob[3] = {&acm_io, &acm_io, &acm_io};


int acm_rx_available(void)
{
    return (tud_cdc_connected() && tud_cdc_available());
}

int acm_tx_available(void)
{
    return (tud_cdc_connected() && tud_cdc_write_available());
}

int acm_getchar(FILE *stream)
{
    while (!acm_rx_available()) {};
    return tud_cdc_read_char();
}

int acm_putchar(char c, FILE *stream)
{
    while (!acm_tx_available()) {};
    tud_cdc_write_char(c);
    return 0;
}

int acm_flush(FILE *stream)
{
    tud_cdc_write_flush();
    tud_cdc_read_flush();
    return 0;
}


void acm_task(void)
{
    if (acm_rx_available()) {
        switch ((char)readchar()) {
            case 'r':
            case 'R':
                script_rewind();
                break;
            case 'b':
            case 'B':
                for (int i = 0; i < 8; i++) printf("%04x ", script_next_token());
                printf("\r\n");
                break;
            case 't':
            case 'T':
                script_test();
                break;
            case 's':
            case 'S':
                printf("init: %04x\r\n", flash_read_status());
                flash_init();
                printf("busy: %04x\r\n", flash_read_status());
                while (flash_busy()) {};
                printf("done: %04x\r\n", flash_read_status());
                break;
            case 'i':
            case 'I':
                printf("flash id: %08lx\r\n", flash_read_id());
                break;
            default:
                break;
        }
        acm_flush(NULL);
    }
}
