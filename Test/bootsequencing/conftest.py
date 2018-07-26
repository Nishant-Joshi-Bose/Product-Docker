# bootsequencing/conftest.py
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
PyTest configuration and fixtures used by the Boot Sequencing Tests.

"""
import pytest

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.MemoryUtils.graphite import Graphite

from conf_bootsequencing import CONFIG

LOGGER = get_logger(__file__)


@pytest.fixture(scope='session')
def graphite():
    """
    Uses the local configuration to generate a session fixture to a Graphite Database

    :return: Graphite connection object.
    """
    graphite_session = Graphite(CONFIG['Grafana']['server_ip'], CONFIG['Grafana']['server_port'],
                                CONFIG['Grafana']['event_port'])
    yield graphite_session

    if graphite_session:
        graphite_session.close()
