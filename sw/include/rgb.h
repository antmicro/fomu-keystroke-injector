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

enum rgb_color_e {RGB_OFF, RGB_GREEN, RGB_RED, RGB_YELLOW, RGB_BLUE, RGB_CYAN, RGB_MAGENTA, RGB_WHITE};
typedef enum rgb_color_e rgb_color_t;

void rgb_set(rgb_color_t color);
rgb_color_t rgb_get(void);
