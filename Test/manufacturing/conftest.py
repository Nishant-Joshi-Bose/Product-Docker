# manufacturing/conftest.py
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:  COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
#              This program may not be reproduced, in whole or in part in any
#              form or any means whatsoever without the written permission of:
#                  BOSE CORPORATION
#                  The Mountain,
#                  Framingham, MA 01701-9168

import pytest

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils import rivieraUtils

LOGGER = get_logger(__name__)


@pytest.mark.usefixtures('riviera')
@pytest.fixture(scope='function')
def riviera_service_enabled(riviera):
    """
    Sets the Service Flag on the Riviera Device

    :param riviera: Riviera Device Object
    :return: None
    """
    riviera.communication.executeCommand('/opt/Bose/bin/rw')

    output = riviera.communication.executeCommand('/usr/bin/mfg_data_set service true').strip()
    assert 'pass' in output, "Setting service did not pass: {}".format(output)

    output = riviera.communication.executeCommand('/usr/bin/mfgdata get service').strip()
    assert output == 'true', "Manufacturing service flag is not true: {}".format(output)

    yield

    # Delete the Servie Flag at the end of the test, if it is not already gone
    riviera.communication.executeCommand('/opt/Bose/bin/rw')
    riviera.communication.executeCommand('/usr/bin/mfgdata del service')
    riviera.communication.executeCommand('/opt/Bose/bin/rw -r')


@pytest.mark.usefixtures('riviera')
@pytest.fixture(scope='function')
def riviera_no_service(riviera):
    """
    Ensures that the Riviera Device does not have the service flag set

    :param riviera: Riviera Device Object
    :return: None
    """
    riviera.communication.executeCommand('/opt/Bose/bin/rw')
    riviera.communication.executeCommand('/usr/bin/mfgdata del service')
    riviera.communication.executeCommand('/opt/Bose/bin/rw -r')

    yield
