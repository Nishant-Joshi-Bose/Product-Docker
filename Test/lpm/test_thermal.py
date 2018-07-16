"""
Unit tests for LPM thremals.

Note: This requires the RivieraLpmService's Python folder to be in the path.

Note: In order to use IPC communications you must follow the setup instructions
in the README from the Python folder in RivieraLpmService.
"""

import threading
import pytest

import AutoLpmServiceMessages_pb2 as AutoIPCMessages
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger

logger = get_logger(__file__)


@pytest.mark.usefixtures('riviera_enabled_ipc', 'lpm_ipc_client', 'ip_address_wlan')
def test_get_thermal_data(lpm_ipc_client):
    """
    Get the amp temperature over IPC and verify that it is a sensible value.

    :param lpm_tap_client: The LPM Tap Client fixture
    :return: None
    """

    # Used to wait for asynchronus requests to come back.
    _lbcs_xfer_event = threading.Event()

    def get_thermal_data_cb(resp):
        """
        Callback for LBCSAnimDBVersionReq.
        :param: resp Response from service in form of IpcSystemTemperatureData_t, which contains
            one to many IpcThermalData_t which reference IpcThermalType_t and IpcThermalLocation_t.
        """
        assert resp, "Missing/invalid thermal response data."

        # Eddie only has one thermal value: the amplifier.
        assert (resp.length > 0), "Missing/invalid thermal response data."
        assert (resp.thermalData[0].location == AutoIPCMessages.IPC_THERMAL_LOCATION_CONSOLE_INTERNAL_AMP), \
            "Thermal data 'location' is invalid."
        assert (resp.thermalData[0].type == AutoIPCMessages.IPC_THERMAL_TYPE_THERMISTOR_TEMP_DEG_C), \
            "Thermal data 'temperature type' is invalid."
        assert (resp.thermalData[0].value > 0 and resp.thermalData[0].value < 100), \
            "Thermal data 'value' is out of acceptable range."

        logger.info("Successfully read amplifier temperature: %dC" % resp.thermalData[0].value)

        _lbcs_xfer_event.set()

    _lbcs_xfer_event.clear()
    lpm_ipc_client.GetThermal(get_thermal_data_cb)
    _lbcs_xfer_event.wait(10)
