"""
Automated test to check the boot-status of the virtual speaker
"""
import time
import json

import pytest

from CastleTestUtils.IPCUtils import ipcmessagerouter
from CastleTestUtils.STSClientServer.Server.STSServiceProxy import STSServiceProxy
from CastleTestUtils.STSClientServer.Server.STSAccountProxy import STSAccountProxy

from CastleTestUtils.FrontDoorAPI.FrontDoorAPI import FrontDoorAPI
from CastleTestUtils.NetworkUtils.Telnet import TelnetSession
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
LOGGER = get_logger(__name__)

@pytest.mark.usefixtures("start_mock_frontDoor")
@pytest.mark.usefixtures("start_mock_lpm")
@pytest.mark.usefixtures("start_cli_server")
@pytest.mark.usefixtures("start_mock_apserver")
@pytest.mark.usefixtures("eddie_product_controller")


#@pytest.mark.usefixtures("start_all_mocks")
def test_boot_status(start_mock_frontDoor):
    LOGGER.info("Getting the boot status")
    def deadletteroffice(env):  #env is IPCMessageEnvelopBase.proto
        print("DEAD LETTER OFFICE got called")
        print(env.msg_type, env.msg_module_id, env.msg_id, env.msg_contents)

    time.sleep(7)
    # Professor may not respond to /ui/alive so nooping for now
    """
    json_msg = {"header":{"device":"",
                          "resource":"/ui/alive",
                          "method":"PUT",
                          "msgtype":"REQUEST",
                          "reqID":0,
                          "version":1.000000,
                          "status":200},
                "body":{"count" : 1621134}}
    start_mock_frontDoor.send_message(json_msg)   
    time.sleep(7)
    """
    LOGGER.info("Start serving STSService")
    #router = ipcmessagerouter.ipcmessagerouterserver(port=30030, dlo=deadletteroffice)
    router = ipcmessagerouter.ipcmessagerouterserver(port=30030, dlo=deadletteroffice, multi_client_server=True)
    sts_service = STSServiceProxy(router)
    sts_service.createAccountProxy_set("PAF_Account")
    sts_service.connect()

    #router_2 = ipcmessagerouter.ipcmessagerouterserver(port=30031,dlo=deadletteroffice)
    router_2 = ipcmessagerouter.ipcmessagerouterserver(port=30031,dlo=deadletteroffice, multi_client_server=True)
    sts_account = STSAccountProxy(router_2)
    sts_account.connect()

    #### - ACTUAL TESTS BEGIN - ####

    LOGGER.info("PAF sleep(2)")
    time.sleep(2)
    telnet_obj = TelnetSession(host="0.0.0.0", port=17000)
    telnet_obj.write(command = "product boot_status")
    result = telnet_obj.read()
    LOGGER.info("The result is: %s", result)

    assert result, "Unable to get the Boot status, Product Controller probably died"

    sts_service.close()
    sts_account.close()
