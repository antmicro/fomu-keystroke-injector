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

#include "storage.h"
#include "script_parse.h"

enum script_exec_state_e {SCRIPT_EXEC_IDLE, SCRIPT_EXEC_TOKEN, SCRIPT_EXEC_EMPTY, SCRIPT_EXEC_FINISH};
typedef enum script_exec_state_e script_exec_state_t;

int script_exec(void);
int script_test(void);
