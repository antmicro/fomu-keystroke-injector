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

#include <libbase/console.h>
#include "fomu.h"
#include "tusb.h"

int acm_getchar(FILE *stream);
int acm_putchar(char c, FILE *stream);
int acm_flush(FILE *stream);
int acm_rx_available(void);
int acm_tx_available(void);
void acm_init(void);
void acm_task(void);
