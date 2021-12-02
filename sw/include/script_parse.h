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

void script_rewind(void);
uint16_t script_next_token(void);
uint8_t script_check_delay(uint16_t token);
void script_print_keystroke(uint16_t token);
void script_report_keystroke(uint16_t token);
