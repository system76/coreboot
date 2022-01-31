-- SPDX-License-Identifier: GPL-2.0-or-later

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is

   ports : constant Port_List :=
     (HDMI1,  -- DVI-D connector
      HDMI3,  -- HDMI connector
      Analog, -- D-Sub connector
      others => Disabled);

end GMA.Mainboard;
