-- ======================================================================
-- SCB_I2cCommMaster01.ctl generated from SCB_I2cCommMaster01
-- 05/15/2017 at 09:33
-- This file is auto generated. ANY EDITS YOU MAKE MAY BE LOST WHEN THIS FILE IS REGENERATED!!!
-- ======================================================================

-- TopDesign
-- =============================================================================
-- The following directives assign pins to the locations specific for the
-- CY8CKIT-042 kit.
-- =============================================================================

-- === I2C master (SCB mode) ===
attribute port_location of \I2CM:scl(0)\ : label is "PORT(3,0)";
attribute port_location of \I2CM:sda(0)\ : label is "PORT(3,1)";

-- === RGB LED ===
attribute port_location of LED_RED(0)   : label is "PORT(1,6)";
attribute port_location of LED_GREEN(0) : label is "PORT(0,2)";

-- PSoC Clock Editor
-- Directives Editor
-- Analog Device Editor
