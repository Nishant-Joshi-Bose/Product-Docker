# bootsequencing/conf_bootsequencing.py
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
Configuration dictionary for Boot Sequencing Tests.

"""

CONFIG = {
    'Grafana': {
        'server_ip': '10.140.21.35',
        'server_port': 2003,
        'event_port': 81,
        'table_name': 'StartupTimingECO2',
        'sub_table_name': 'Local'
    }
}
