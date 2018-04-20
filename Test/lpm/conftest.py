# lpm/conftest.py
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:  COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
#              This program may not be reproduced, in whole or in part in any
#              form or any means whatsoever without the written permission of:
#                  BOSE CORPORATION
#                  The Mountain,
#                  Framingham, MA 01701-9168

import threading
import time

import pytest

import AutoLpmServiceMessages_pb2 as AutoIPCMessages
import LpmServiceMessages_pb2 as IPCMessages
import lpm_client as LpmClient

from CastleTestUtils.RivieraUtils import rivieraUtils
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.LpmUtils.Lpm import Lpm


logger = get_logger(__name__)


@pytest.mark.usefixtures('riviera_enabled_ipc', 'ip_address_wlan')
@pytest.fixture(scope='function')
def lpm_ipc_client(ip_address_wlan):
    """
    Generates an IPC Network Client to the Device.

    :param ip_address_wlan: The IP Address (WLAN) of the Device
    :yield: LPM IPC Client connection
    """
    lpm_client = LpmClient.LpmClient(ip_address_wlan)

    yield lpm_client

    # Close the client
    del lpm_client


@pytest.fixture(scope='session')
def lpm_serial_client(request):
    """
    Generates an LPM Serial Client using the supplied port.

    :param request: PyTest command line request object
    :yield: LPM Serial Client connection
    """
    lpm_serial_path = request.config.getoption('--lpm-port')

    # LPM tap client.
    lpm_serial = Lpm(lpm_serial_path)

    yield lpm_serial

    # Close the client
    del lpm_serial


@pytest.fixture(scope='module')
def riviera_enabled_ipc(request):
    """
    Sets up a Riviera Device with IPC external communication enabled on
      port 17023.

    Returns to the device to its original state on close.

    :param request: PyTest command line request options
    :return: None
    """
    device_id = request.config.getoption('--device-id')

    # Make a Riviera Object
    riviera_device = rivieraUtils.RivieraUtils('ADB', device=device_id)

    # We will be modifying the `/persist` directory to enable IPC port access
    riviera_device.communication.executeCommand('/opt/Bose/bin/rw')
    time.sleep(1)

    # Modify the /opt/Bose/etc/services.xml file
    # IPC communication runs on Port 17023 within the Riviera.
    # At initially install, we only allow communication internal.
    # Removing `localhost` allows IPC port access to the outside world
    riviera_device.communication.executeCommand("sed -i 's/localhost\:17023/\:17023/' /opt/Bose/etc/services.json")

    # Restart shepherd
    riviera_device.communication.executeCommand('/opt/Bose/bin/stop')
    time.sleep(1)
    riviera_device.communication.executeCommand('/opt/Bose/bin/start')
    time.sleep(4)

    # Yield
    yield

    # Conclusion of test, revert open IPC port (17023) access
    riviera_device.communication.executeCommand('/opt/Bose/bin/rw')
    riviera_device.communication.executeCommand("sed -i 's/\:17023/localhost\:17023/' /opt/Bose/etc/services.json")


@pytest.fixture(scope='function')
@pytest.mark.usefixtures('riviera_enabled_ipc', 'lpm_ipc_client')
def animation_data(lpm_ipc_client):
    """
    Determines the Lightbar Aninmation Data on the Device.

    :param lpm_ipc_client: An IPC Network Connection to the device.
    :return: Animation Database values
    """
    # Used to wait for asynchronus requests to come back.
    lbcs_xfer_event = threading.Event()

    animation_values = None
    global animation_values

    def get_animation_data_cb(resp):
        """
        Callback for IPC_LBCS_DB_INDEX_DATA_RESP.

        :param: resp Response from service in form of LBCSDbIndexDataResp_t
        """
        global animation_values
        logger.debug("Animation Data: {}".format(animation_values))
        logger.debug("Response: {}".format(resp))

        assert resp, "No valid animation data response received."

        global animation_values
        animation_values = resp
        logger.debug("Animation Data: {}".format(animation_values))
        lbcs_xfer_event.set()

    lpm_ipc_client.RegisterEvent(AutoIPCMessages.IPC_LBCS_DB_INDEX_DATA_RESP,
                          IPCMessages.LBCSDbIndexDataResp_t,
                          get_animation_data_cb)

    lbcs_xfer_event.clear()

    lpm_ipc_client.LBCSGetDBIndexData()

    lbcs_xfer_event.wait(10)

    logger.debug("Animation Data: {}".format(animation_values))

    assert animation_values, "Animation Data never set."

    yield animation_values
