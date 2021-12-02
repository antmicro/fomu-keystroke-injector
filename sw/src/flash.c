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

#include <generated/csr.h>
#include <liblitespi/spiflash.h>
#include <string.h>

#include "utils.h"
#include "flash.h"

static uint8_t tx_buf[SPIFLASH_MODULE_PAGE_SIZE + 4];

/*** LiteSPI master controller API ***/

void flash_master_begin(void)
{
    spiflash_core_master_cs_write(1);
}

void flash_master_end(void)
{
    spiflash_core_master_cs_write(0);
}

void flash_master_config(size_t len, size_t width, uint32_t mask)
{
    /* Set master/PHY configuration */
    spiflash_core_master_phyconfig_len_write(len);
    spiflash_core_master_phyconfig_width_write(width);
    spiflash_core_master_phyconfig_mask_write(mask);
}

uint32_t flash_master_transfer(uint32_t tx)
{
    while (!spiflash_core_master_status_tx_ready_read());
    spiflash_core_master_rxtx_write(tx);
    while (!spiflash_core_master_status_rx_ready_read());
    return spiflash_core_master_rxtx_read();
}

size_t flash_transfer_single(const uint8_t *tx_data, uint8_t *rx_data, size_t len)
{
    size_t i;
    uint8_t response;

    if ((len == 0) || (tx_data == NULL)) return 0;

    flash_master_config(8, 1, 0x1);

    for (i = 0; i < len; i++) {
        response = flash_master_transfer(tx_data[i]);
        if (rx_data != NULL) rx_data[i] = response;
    }

    return i;
}

size_t flash_transfer_quad(const uint8_t *tx_data, uint8_t *rx_data, size_t len)
{
    size_t i;
    uint8_t response;

    /* Transfer data (if any) */
    if ((len == 0) || (tx_data == NULL)) return 0;

    flash_master_config(8, 4, 0xf);

    for (i = 0; i < len; i++) {
        response = flash_master_transfer(tx_data[i]);
        if (rx_data != NULL) rx_data[i] = response;
    }

    return i;
}

/*** SPI flash API ***/

void flash_write_enable(void)
{
    flash_master_begin();

    tx_buf[0] = FLASH_CMD_WEL_ENABLE;
    flash_transfer_single(tx_buf, NULL, 1);

    flash_master_end();
}

uint16_t flash_read_status(void) {
    uint8_t rx_buf[2];
    uint8_t status[2];

    flash_master_begin();

    tx_buf[0] = FLASH_CMD_READ_STATUS_0;
    tx_buf[1] = 0;
    flash_transfer_single(tx_buf, rx_buf, 2);
    status[0] = rx_buf[1];

    tx_buf[0] = FLASH_CMD_READ_STATUS_1;
    tx_buf[1] = 0;
    flash_transfer_single(tx_buf, rx_buf, 2);
    status[1] = rx_buf[1];

    flash_master_end();

    return ((status[1] << 8) | status[0]);
}

void flash_write_status(uint16_t status) {
    flash_write_enable();

    flash_master_begin();

    tx_buf[0] = FLASH_CMD_WRITE_STATUS;
    tx_buf[1] = (status);
    tx_buf[2] = (status >> 8);

    flash_transfer_single(tx_buf, NULL, 3);

    flash_master_end();
}

int flash_busy(void) {
    return flash_read_status() & FLASH_STATUS_WIP;
}

uint32_t flash_id;

uint32_t flash_read_id(void) {
    uint8_t rx_buf[6];
    flash_id = 0;

    flash_master_begin();

    tx_buf[0] = FLASH_CMD_READ_MFR;
    tx_buf[1] = 0;
    tx_buf[2] = 0;
    tx_buf[3] = 0;
    tx_buf[4] = 0;
    tx_buf[5] = 0;
    flash_transfer_single(tx_buf, rx_buf, 6);
    flash_id = (flash_id << 8) | rx_buf[4];   // Manufacturer ID
    flash_id = (flash_id << 8) | rx_buf[5];   // Device ID

    tx_buf[0] = FLASH_CMD_READ_ID;            // Read device id
    tx_buf[1] = 0;                            // Manufacturer ID (again)
    tx_buf[2] = 0;
    tx_buf[3] = 0;
    flash_transfer_single(tx_buf, rx_buf, 4);
    flash_id = (flash_id << 8) | rx_buf[2];   // Memory Type
    flash_id = (flash_id << 8) | rx_buf[3];   // Memory Size

    flash_master_end();

    return flash_id;
}

void flash_erase_4k(uint32_t address)
{
    flash_write_enable();

    flash_master_begin();

    tx_buf[0] = FLASH_CMD_ERASE_4K;
    tx_buf[1] = (address >> 16);
    tx_buf[2] = (address >> 8);
    tx_buf[3] = (address >> 0);
    flash_transfer_single(tx_buf, NULL, 4);

    flash_master_end();
}

size_t flash_write_page(uint32_t address, const void *src, size_t len)
{
    const uint8_t *data = src;
    size_t count = MIN(len, SPIFLASH_MODULE_PAGE_SIZE);

    if (!count) return 0;

    flash_write_enable();

    flash_master_begin();

#ifdef SPIFLASH_MODULE_QUAD_CAPABLE
    tx_buf[0] = FLASH_CMD_WRITE_PAGE_QUAD;
#else
    tx_buf[0] = FLASH_CMD_WRITE_PAGE;
#endif
    tx_buf[1] = (address >> 16);
    tx_buf[2] = (address >> 8);
    tx_buf[3] = (address >> 0);

    flash_transfer_single(tx_buf, NULL, 4);
#ifdef SPIFLASH_MODULE_QUAD_CAPABLE
    flash_transfer_quad(data, NULL, count);
#else
    flash_transfer_single(data, NULL, count);
#endif

    flash_master_end();

    return count;
}

void flash_reset(void) {
    // Writing 0xff eight times is equivalent to exiting QPI mode,
    // or if CFM mode is enabled it will terminate CFM and return
    // to idle.
    unsigned int i;

    flash_master_begin();

    for (i = 0; i < FLASH_EXIT_QPI_LOOP; i++) tx_buf[i] = FLASH_CMD_EXIT_QPI;
    flash_transfer_single(tx_buf, NULL, FLASH_EXIT_QPI_LOOP);

    // Some SPI parts require this to wake up
    tx_buf[0] = FLASH_CMD_WAKE_UP;
    flash_transfer_single(tx_buf, NULL, 1);

    flash_master_end();
}

void flash_init(void) {
    // Reset the SPI flash, which will return it to SPI mode even
    // if it's in QPI mode, and ensure the chip is accepting commands.
    //flash_reset();

    //flash_read_id();

#ifdef SPIFLASH_MODULE_QUAD_CAPABLE
    //flash_write_status(0x0743);
    flash_write_status(FLASH_STATUS_QE);
#endif

    return;
}

void flash_powerdown(void) {
    flash_master_begin();

    tx_buf[0] = FLASH_CMD_POWER_DOWN;
    flash_transfer_single(tx_buf, NULL, 1);

    flash_master_end();
}

void flash_powerup(void) {
    flash_master_begin();

    tx_buf[0] = FLASH_CMD_WAKE_UP;
    flash_transfer_single(tx_buf, NULL, 1);

    flash_master_end();
}
