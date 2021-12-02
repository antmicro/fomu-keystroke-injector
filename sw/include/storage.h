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

#include <stdint.h>

#include "generated/soc.h"
#include "generated/mem.h"

#define STORAGE_ERASE_SIZE 4096
#define STORAGE_PROGRAM_SIZE 256

typedef struct storage_partition_s {
    size_t start;
    size_t size;
} storage_partition_t;

uint8_t storage_max_partitions(void);
storage_partition_t storage_get_partition(uint8_t number);
void storage_write(const storage_partition_t* part, size_t offset, const void* src, size_t size);
size_t storage_read(const storage_partition_t* part, size_t offset, const void* dest, size_t size);
void storage_erase(const storage_partition_t* part, size_t offset, size_t size);


