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
#include "rgb.h"

void rgb_set(rgb_color_t color)
{
    rgb_output_write(color);
    return;
}

rgb_color_t rgb_get(void)
{
    return rgb_output_read() & 0x7;
}
