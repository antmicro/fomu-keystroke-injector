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
#include "class/dfu/dfu.h"

#include "utils.h"
#include "mode.h"
#include "storage.h"
#include "rgb.h"

#include "usb_dfu.h"


uint16_t block_num = 0;

uint8_t dfu_program(uint8_t alt, uint16_t num, uint8_t const* buffer, uint16_t len)
{
    storage_partition_t part = storage_get_partition(alt);
    size_t block_offset = num * CFG_TUD_DFU_XFER_BUFSIZE;

    if (block_offset+len > part.size) {
        /* Block won't fit inside partition */
        return DFU_STATUS_ERR_ADDRESS;
    }

    rgb_set(RGB_RED);
    /* TODO: remove assumption: dfu block size <= flash erase size */
    storage_erase(&part, block_offset, STORAGE_ERASE_SIZE);

    rgb_set(RGB_YELLOW);
    if (len) {
        storage_write(&part, block_offset, buffer, len);
    }

    /* TODO: verify if block was properly programmed on the flash */

    rgb_set(RGB_GREEN);

    return DFU_STATUS_OK;
}

/*** DFU CALLBACKS - used by tinyUSB ***/

uint32_t tud_dfu_get_timeout_cb(uint8_t alt, uint8_t state)
{
    if ( state == DFU_DNBUSY ) {
        return 100;
    }
    else if (state == DFU_MANIFEST) {
        return 0;
    }

    return 0;
}

void tud_dfu_download_cb(uint8_t alt, uint16_t block, uint8_t const* data, uint16_t length)
{
    uint8_t ret;
    printf("DFU %d: block %u, len %u\r\n", alt, block, length);

#if DFU_VERBOSE
    for(uint16_t i=0; i<length; i++)
    {
        printf("DFU: [%u][%u]: %x\r\n", block, i, (uint8_t)data[i]);
    }
#endif

    block_num = block;
    ret = dfu_program(alt, block, data, length);
    tud_dfu_finish_flashing(ret);
}

#define UPLOAD_SIZE (25)
const uint8_t upload_test[UPLOAD_SIZE] = "Upload should be disabled";

uint16_t tud_dfu_upload_cb(uint8_t alt, uint16_t block, uint8_t* data, uint16_t length)
{
    storage_partition_t part = storage_get_partition(alt);
    size_t block_offset = block * CFG_TUD_DFU_XFER_BUFSIZE;

    return storage_read(&part, block_offset, data, length);
}

void tud_dfu_manifest_cb(uint8_t alt)
{
    uint8_t ret = DFU_STATUS_OK;
    printf("DFU %d: download complete\r\n", alt);
    rgb_set(RGB_CYAN);

    /* If the last block is at the end of 4KiB sector,
     * then erase the next sector to make sure there's empty space
     * filled with byte 0xff to have a stop mark for the script,
     * even when it's taking exactly n*4096 bytes of space.
     */
    if (alt == 0) {
        block_num++;
        ret = dfu_program(alt, block_num, NULL, 0);
    }
    tud_dfu_finish_flashing(ret);
}

void tud_dfu_runtime_reboot_to_dfu_cb(void)
{
    int start_time = board_millis();

    rgb_set(RGB_RED);

    /* Disconnect and wait 0.5 second before reconnecting USB */
    tud_disconnect();

    while (board_millis() - start_time < 500) {};
    work_mode_set(WORK_MODE_PROG);

    tud_connect();

    rgb_set(RGB_YELLOW);

    while (!tud_ready()) {};

    rgb_set(RGB_GREEN);
}

void tud_dfu_abort_cb(uint8_t alt)
{
    printf("DFU %d: transfer aborted\r\n", alt);
    rgb_set(RGB_RED);
}

void tud_dfu_detach_cb(void)
{
    printf("DFU: detach requested\r\n");
    rgb_set(RGB_BLUE);
}
