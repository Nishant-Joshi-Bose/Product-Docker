"""
Unit tests for LPM thremals.

Note: This requires the RivieraLpmService's Python folder to be in the path.

Note: In order to use IPC communications you must follow the setup instructions 
in the README from the Python folder in RivieraLpmService.
"""

import pytest
import random
import threading
import time

import RivieraLpmService.lpm_client as LpmClient
import RivieraLpmService.AutoLpmServiceMessages_pb2 as AutoIPCMessages
import RivieraLpmService.LpmServiceMessages_pb2 as IPCMessages

from CastleTestUtils.LoggerUtils.log_setup import get_logger

# Required parameter: ip address of IPC server.
_ip_address = pytest.config.getoption('--ip-address')
if _ip_address is None:
    pytest.fail("IP address is required: pytest -sv <test.py> --ip-address <0.0.0.0>")

# Logging utility.
_logger = get_logger(__file__)

# Used to wait for asynchronus requests to come back.
_lbcs_xfer_event = threading.Event()

# The remote LPM Service with which we will communicate.
_lpm_ipc = LpmClient.LpmClient(_ip_address)
assert _lpm_ipc, "Failed to open LPM IPC interface at ip address %s" % _ip_address

#
# Callbacks.
#

def get_thermal_data_cb(resp):
	"""
	Callback for LBCSAnimDBVersionReq.
	:param: resp Response from service in form of IpcSystemTemperatureData_t, which contains
		one to many IpcThermalData_t which reference IpcThermalType_t and IpcThermalLocation_t.
	"""
	assert resp, "Missing/invalid thermal response data."

	# Eddie only has one thermal value: the amplifier.
	assert (resp.length > 0), "Missing/invalid thermal response data."
	assert (resp.thermalData[0].location == AutoIPCMessages.IPC_THERMAL_LOCATION_CONSOLE_INTERNAL_AMP), "Thermal data 'location' is invalid."
	assert (resp.thermalData[0].type == AutoIPCMessages.IPC_THERMAL_TYPE_THERMISTOR_TEMP_DEG_C), "Thermal data 'temperature type' is invalid."
	assert (resp.thermalData[0].value > 0 and resp.thermalData[0].value < 100), "Thermal data 'value' is out of acceptable range."

	_logger.info("Successfully read amplifier temperature: %dC" % resp.thermalData[0].value)

	_lbcs_xfer_event.set()

#
# Test entry points.
#

def test_get_thermal_data():
	"""
	Get the amp temperature over IPC and verify that it is a sensible value.
	"""

	_lbcs_xfer_event.clear()
	_lpm_ipc.GetThermal(get_thermal_data_cb)
	_lbcs_xfer_event.wait(10)
