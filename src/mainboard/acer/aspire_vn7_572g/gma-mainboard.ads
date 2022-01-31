-- SPDX-License-Identifier: GPL-2.0-only

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is

   ports : constant Port_List :=
     (eDP,
      HDMI1,
      others => Disabled);

end GMA.Mainboard;
