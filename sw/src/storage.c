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
#include <stdint.h>
#include <string.h>

#include "generated/csr.h"

#include "flash.h"
#include "storage.h"
#include "utils.h"


storage_partition_t partitions[] = {
    /* Partition 0: script */
    {.start = PART_SCRIPT, .size = SPIFLASH_SIZE - PART_SCRIPT},
    /* Partition 1: gateware/firmware/software */
    {.start = PART_GFS, .size = PART_SCRIPT - PART_GFS},
#ifdef PART_INSTALL
    /* Partition 2: (test mode only) Flash start (for foboot overwriting) */
    {.start = PART_INSTALL, .size = PART_GFS},
#endif
};

const uint8_t partitions_len = ARRAY_SIZE(partitions);

storage_partition_t storage_get_partition(uint8_t number)
{
    storage_partition_t empty = {.start = 0, .size = 0};

    if (number < partitions_len) return partitions[number];
    return empty;
}

void storage_erase(const storage_partition_t* part, size_t offset, size_t size)
{
    size_t address = part->start + offset;
    size_t i;

    if (size == 0 || part == NULL) return;
    /* Check if address is aligned to sector boundary */
    if ((address & ~(STORAGE_ERASE_SIZE - 1)) != address) return;
    /* Check if size is a multiply of sector size */
    if ((size % STORAGE_ERASE_SIZE) != 0) return;

    printf("stor_erase: %d bytes at %08x\r\n", size, address);
    for (i = 0; i < size; i += STORAGE_ERASE_SIZE) {
        while (flash_busy()) {};
        flash_erase_4k(address + i);
    }
}

void storage_write(const storage_partition_t* part, size_t offset, const void* src, size_t size)
{
    size_t address = part->start + offset;
    size_t i = 0, write_size;

    if (size == 0 || src == NULL || part == NULL) return;
    /* Check if address is aligned to page boundary */
    if ((address & ~(STORAGE_PROGRAM_SIZE - 1)) != address) return;

    printf("stor_write: %8p -> %08x (%d)\r\n", src, address, size);
    while (i < size) {
        write_size = MIN(size-i, STORAGE_PROGRAM_SIZE);
        while (flash_busy()) {};
        flash_write_page(address + i, src + i, write_size);
        i += write_size;
    }
}

size_t storage_read(const storage_partition_t* part, size_t offset, const void* dest, size_t size)
{
    size_t address = part->start + offset;

    if (size == 0 || dest == NULL || part == NULL) return 0;

    printf("stor_read: %08x -> %8p (%d)\r\n", address, dest, size);
    memcpy((void *)dest, (const void *)SPIFLASH_BASE + address, size);

    return size;
}
