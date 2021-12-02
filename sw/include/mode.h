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

enum work_mode_e {WORK_MODE_PROG, WORK_MODE_EXEC};
typedef enum work_mode_e work_mode_t;

work_mode_t work_mode_get(void);
void work_mode_set(work_mode_t new_mode);
