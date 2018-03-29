#pragma once

namespace PGCErrorCodes
{

// The codes below should match those listed in system_identifiers.txt
// in the bose-apis repo. If you want to reserve a new code, make a
// pull request to modify system_identifiers.txt.
enum
{
    ERROR_CODE_FRONT_DOOR = 0,
    ERROR_CODE_PRODUCT_CONTROLLER_COMMON = 1,
    ERROR_CODE_PRODUCT_CONTROLLER_CUSTOM = 2,
    ERROR_CODE_CAPS = 3,
    ERROR_CODE_SOFTWARE_UPDATE = 4,
    ERROR_CODE_DEMO_CONTROLLER = 5,

    ERROR_CODE_VIDEO_MANAGER = 100,
    ERROR_CODE_QUICKSET = 101,
    ERROR_CODE_RCS = 102,
};

// Subcodes are defined by the component, so these can be anything we want.
enum
{
    ERROR_SUBCODE_GENERAL = 0,
    ERROR_SUBCODE_ACCESSORIES = 1,
    ERROR_SUBCODE_AIQ = 2,
    ERROR_SUBCODE_CEC = 3,
    ERROR_SUBCODE_OPTICAL_AUTOWAKE = 4,
};

}

