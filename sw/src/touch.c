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
#include "touch.h"

int touch_is_left(void)
{
    return !(touch_input_read() & (1 << TOUCH_LEFT_BIT));
}

int touch_is_right(void)
{
    return !(touch_input_read() & (1 << TOUCH_RIGHT_BIT));
}
