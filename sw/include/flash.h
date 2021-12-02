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

#define FLASH_CMD_WEL_ENABLE 0x06
#define FLASH_CMD_ERASE_4K 0x20
#define FLASH_CMD_READ_STATUS_0 0x05
#define FLASH_CMD_READ_STATUS_1 0x35
#define FLASH_CMD_WRITE_STATUS 0x01
#define FLASH_CMD_READ_MFR 0x90
#define FLASH_CMD_READ_ID 0x9f
#define FLASH_CMD_WRITE_PAGE 0x02
#define FLASH_CMD_WRITE_PAGE_QUAD 0x32
#define FLASH_CMD_POWER_DOWN 0xb9
#define FLASH_CMD_WAKE_UP 0xab
#define FLASH_CMD_EXIT_QPI 0xff
#define FLASH_STATUS_WIP (1<<0)
#define FLASH_STATUS_WEL (1<<1)
#define FLASH_STATUS_BP0 (1<<2)
#define FLASH_STATUS_BP1 (1<<3)
#define FLASH_STATUS_BP2 (1<<4)
#define FLASH_STATUS_BP3 (1<<5)
#define FLASH_STATUS_BP4 (1<<6)
#define FLASH_STATUS_SRP0 (1<<7)
#define FLASH_STATUS_SRP1 (1<<8)
#define FLASH_STATUS_QE (1<<9)
#define FLASH_STATUS_LB (1<<10)
#define FLASH_STATUS_RESERVED0 (1<<11)
#define FLASH_STATUS_RESERVED1 (1<<12)
#define FLASH_STATUS_HPF (1<<13)
#define FLASH_STATUS_CMP (1<<14)
#define FLASH_STATUS_SUS (1<<15)
#define FLASH_EXIT_QPI_LOOP 8

void flash_master_begin(void);
void flash_master_end(void);
void flash_master_config(size_t len, size_t width, uint32_t mask);
uint32_t flash_master_transfer(uint32_t tx);
size_t flash_transfer_single(const uint8_t *tx_data, uint8_t *rx_data, size_t len);
size_t flash_transfer_quad(const uint8_t *tx_data, uint8_t *rx_data, size_t len);

uint16_t flash_read_status(void);
void flash_write_status(uint16_t status);
int flash_busy(void);
uint32_t flash_read_id(void);
void flash_write_enable(void);
void flash_erase_4k(uint32_t address);
size_t flash_write_page(uint32_t address, const void *src, size_t len);
void flash_reset(void);
void flash_init(void);
void flash_powerdown(void);
void flash_powerup(void);
