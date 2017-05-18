-- ======================================================================
-- SCB_I2cCommSlave01.ctl generated from SCB_I2cCommSlave01
-- 05/18/2017 at 13:45
-- This file is auto generated. ANY EDITS YOU MAKE MAY BE LOST WHEN THIS FILE IS REGENERATED!!!
-- ======================================================================

-- TopDesign
-- =============================================================================
-- The following directives assign pins to the locations specific for the
-- CY8CKIT-041 kit.
-- =============================================================================

-- === I2C slave (SCB mode) ===
attribute port_location of \I2CS:scl(0)\ : label is "PORT(3,0)";
attribute port_location of \I2CS:sda(0)\ : label is "PORT(3,1)";

-- === RGB LED ===
attribute port_location of LED_RED(0)   : label is "PORT(3,4)";
attribute port_location of LED_GREEN(0) : label is "PORT(2,6)";
attribute port_location of LED_BLUE(0)  : label is "PORT(3,6)";

-- PSoC Clock Editor
-- Directives Editor
-- Analog Device Editor
