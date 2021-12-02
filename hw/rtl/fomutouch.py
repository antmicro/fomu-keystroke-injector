# -*- coding: utf-8 -*-

# Copyright (C) 2021 Antmicro
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0

from migen import Module, TSTriple, Cat
from migen.fhdl.specials import Instance
from litex.soc.interconnect.csr import AutoCSR, CSRStatus, CSRStorage
from litex.soc.integration.doc import ModuleDoc
from litex.build.generic_platform import Pins, Subsignal

### Touch buttons
class FomuTouch(Module, AutoCSR):
    # Type for input pins, from ICE Technology Library Manual, pages 87-90
    SB_IO_TYPE_SIMPLE_INPUT = 0b000001
    def __init__(self, pads):
        self.input = CSRStatus(2, description="Input values for pads 0 and 3")
        # Use touch pins 1+2 to ground pulled-up inputs
        self.comb += [
            pads[1].eq(0),
            pads[2].eq(0)
        ]
        # Left touch button
        self.specials += Instance('SB_IO',
            i_PACKAGE_PIN=pads[0],
            i_OUTPUT_ENABLE=0b0,
            o_D_IN_0=self.input.status[0],
            p_PIN_TYPE=self.SB_IO_TYPE_SIMPLE_INPUT,
            p_PULLUP=0b1
        )
        # Right touch button
        self.specials += Instance('SB_IO',
            i_PACKAGE_PIN=pads[3],
            i_OUTPUT_ENABLE=0b0,
            o_D_IN_0=self.input.status[1],
            p_PIN_TYPE=self.SB_IO_TYPE_SIMPLE_INPUT,
            p_PULLUP=0b1
        )
