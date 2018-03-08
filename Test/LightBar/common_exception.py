# common_exception.py
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:  COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
#              This program may not be reproduced, in whole or in part in any
#              form or any means whatsoever without the written permission of:
#                  BOSE CORPORATION
#                  The Mountain,
#                  Framingham, MA 01701-9168
#
"""
This Module contains the user defined exception class for the lightbar test.
"""
class SerialPortError(Exception):
    """ Exception for Serial Port Error """
    pass


class IPAddressError(Exception):
    """ Exception for IPAddress Error """
    pass
