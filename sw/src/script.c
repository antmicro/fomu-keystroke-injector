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

#include "rgb.h"
#include "utils.h"
#include "script.h"


/* script_exec reads a single token and tries to execute is.
 * Returns:
 * -1 if delay was set/is still active
 *  0 if there are no more tokens to be executed
 *  1 if a keystroke token was sent
 */
int script_exec(void)
{
    static uint32_t interval_ms = 0;
    static uint32_t start_ms = 0;
    uint16_t token;

    /* Don't read the next token if previous one set a delay */
    if (board_millis() - start_ms < interval_ms) return -1;
    start_ms = board_millis();

    /* Get token with the next keystroke/delay */
    /* UINT16_MAX equals erased flash space,
     * so there will be no more tokens in memory
     */
    token = script_next_token();

    if (token == UINT16_MAX) {
        return 0;
    }

    /* Don't send any keystroke if the token contained delay value */
    interval_ms = script_check_delay(token);
    if (interval_ms) {
        return -1;
    }

    /* Execute keystroke */
    script_report_keystroke(token);
    return 1;
}

/* script_test reads a single token and prints it's meaning to the standard output.
 * Returns:
 * -1 if token contained delay
 *  0 if script execution was ended (empty storage space)
 *  1 if token contained keystroke
 */
int script_test(void)
{
    /* Get token with the next keystroke/delay */
    /* UINT16_MAX equals erased flash space,
     * so there will be no more tokens in memory
     */
    uint16_t token = script_next_token();
    printf("TOKEN: %04x\r\n", token);

    if (token == UINT16_MAX) {
        printf("END OF SCRIPT\r\n");
        return 0;
    }

    int delay = script_check_delay(token);
    if (delay) {
        printf("DELAY: %d\r\n", delay);
        return -1;
    }
    else {
        script_print_keystroke(token);
        return 1;
    }

    return 0;
}
