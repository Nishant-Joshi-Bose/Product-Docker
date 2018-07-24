# ShepherdStartUp/conftest.py
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
conftest.py for the Eddie Shepherd Startup Tests
"""


def pytest_addoption(parser):
    """
    Command line options for the pytest tests in this module.
    :param parser: Parser used for method.
    :return: None
    """
    parser.addoption('--zipfile',
                     action='store',
                     type=str,
                     required=True,
                     help='Path to Zip file needed for Bonjourupdate')
    parser.addoption('--updatecnt',
                     action='store',
                     type=int,
                     default=2,
                     help='Set count to perform Bonjour Update that many times')
