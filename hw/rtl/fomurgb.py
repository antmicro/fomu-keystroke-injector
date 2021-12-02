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

### RGB LED controller
class FomuRGB(Module, AutoCSR):
    # iCE40 LED driver hard logic
    # https://www.latticesemi.com/-/media/LatticeSemi/Documents/ApplicationNotes/IK/ICE40LEDDriverUsageGuide.ashx?document_id=50668

    # Current modes
    RGBA_CURRENT_MODE_FULL = '0b0'
    RGBA_CURRENT_MODE_HALF = '0b1'
    # Current levels in Full / Half mode
    RGBA_CURRENT_04MA_02MA = '0b000001'
    RGBA_CURRENT_08MA_04MA = '0b000011'
    RGBA_CURRENT_12MA_06MA = '0b000111'
    RGBA_CURRENT_16MA_08MA = '0b001111'
    RGBA_CURRENT_20MA_10MA = '0b011111'
    RGBA_CURRENT_24MA_12MA = '0b111111'

    def __init__(self, pads):
        self.output = CSRStorage(3)
        self.specials += Instance('SB_RGBA_DRV',
            i_CURREN=0b1,
            i_RGBLEDEN=0b1,
            i_RGB0PWM=self.output.storage[0],
            i_RGB1PWM=self.output.storage[1],
            i_RGB2PWM=self.output.storage[2],
            o_RGB0=pads.r,
            o_RGB1=pads.g,
            o_RGB2=pads.b,
            p_CURRENT_MODE=self.RGBA_CURRENT_MODE_HALF,
            p_RGB0_CURRENT=self.RGBA_CURRENT_08MA_04MA,
            p_RGB1_CURRENT=self.RGBA_CURRENT_08MA_04MA,
            p_RGB2_CURRENT=self.RGBA_CURRENT_08MA_04MA
        )
