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

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.LpmUtils.Lpm import Lpm
from CastleTestUtils.NetworkUtils.network_base import NetworkBase
from CastleTestUtils.RivieraUtils import rivieraUtils
from CastleTestUtils.RivieraUtils.device_utils import get_ip_address


LOGGER = get_logger(__name__)

global animation_values


@pytest.fixture(scope='function')
def ip_address(request, device_id, wifi_config):
    """
    Gathers the IP Address of the device.

    :param request:
    :param device_id:
    :param wifi_config:
    :return:
    """
    LOGGER.debug("Determining which IP Address method to use.")
    network_base = NetworkBase(None, device=device_id, logger=LOGGER)

    # The command line parameter will take over everything
    if request.config.getoption('--ip-address'):
        LOGGER.debug("Assuming you passed in command line argument.")
        sourced_ip_address = request.config.getoption('--ip-address')
        LOGGER.debug("Returning %s as IP Address.", sourced_ip_address)
        return sourced_ip_address

    # Look for Ethernet information
    elif request.config.getoption('--network-iface') == 'eth0':
        LOGGER.debug("Assuming use of Ethernet IP Address.")

        ethernet_ip_address = network_base.check_inf_presence(interface='eth0')
        LOGGER.debug("Ethernet IP Address: %s", ethernet_ip_address)
        return ethernet_ip_address

    # Assuming Wifi information now
    elif request.config.getoption("--router"):

        interface = 'wlan0'
        router = request.config.getoption("--router")
        ssid = wifi_config.get(router, 'ssid')
        security = wifi_config.get(router, 'security')
        password = wifi_config.get(router, 'password')

        wlan_ip_address = get_ip_address(device_id, interface, ssid, security, password)
        assert wlan_ip_address, "IP Address not acquired through WLAN0."

        return wlan_ip_address

    # We have not used this function appropriately
    else:
        options = ['--ip-address', '--network-iface', '--router']
        pytest.fail("You must pass in appropriate option:\n\t{}"
                    .format('\n\t'.join(options)))


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


@pytest.fixture(scope='session')
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
    LOGGER.debug("Removing localhost restriction on port 17023.")
    riviera_device.communication.executeCommand("sed 's/localhost\:17023/\:17023/' -i /opt/Bose/etc/services.json")
    riviera_device.communication.executeCommand('sync')
    time.sleep(3)

    output = riviera_device.communication.executeCommand('jq . /opt/Bose/etc/services.json').strip()
    if 'localhost:17023' in output:
        pytest.fail("Not able to remove LPMService localhost restriction.")

    # Reboot device because changes will not stick with start/stop of Bose
    riviera_device.communication.executeCommand('/opt/Bose/bin/PlatformReset')
    riviera_device.communication.waitForRebootDevice()

    # Yield
    yield

    # Conclusion of test, revert open IPC port (17023) access
    riviera_device.communication.executeCommand('/opt/Bose/bin/rw')
    riviera_device.communication.executeCommand("sed 's/\:17023/localhost\:17023/' -i /opt/Bose/etc/services.json")
    riviera_device.communication.executeCommand('sync')


@pytest.mark.usefixtures('riviera_enabled_ipc', 'ip_address')
@pytest.fixture(scope='function')
def lpm_ipc_client(ip_address):
    """
    Generates an IPC Network Client to the Device.

    :param ip_address: The IP Address of the Device
    :yield: LPM IPC Client connection
    """
    lpm_client = LpmClient.LpmClient(ip_address)

    yield lpm_client

    # Close the client
    del lpm_client


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

    global animation_values
    animation_values = None

    def get_animation_data_cb(resp):
        """
        Callback for IPC_LBCS_DB_INDEX_DATA_RESP.

        :param: resp Response from service in form of LBCSDbIndexDataResp_t
        """
        global animation_values
        LOGGER.debug("Animation Data: %s", animation_values)
        LOGGER.debug("Response: %s", resp)

        assert resp, "No valid animation data response received."

        global animation_values
        animation_values = resp
        LOGGER.debug("Animation Data: %s", animation_values)
        lbcs_xfer_event.set()

    lpm_ipc_client.RegisterEvent(AutoIPCMessages.IPC_LBCS_DB_INDEX_DATA_RESP,
                                 IPCMessages.LBCSDbIndexDataResp_t,
                                 get_animation_data_cb)

    lbcs_xfer_event.clear()

    lpm_ipc_client.LBCSGetDBIndexData()

    lbcs_xfer_event.wait(10)

    LOGGER.debug("Animation Data: %s", animation_values)

    assert animation_values, "Animation Data never set."

    yield animation_values
