import os
import time
import psutil
import pytest
import subprocess

from CastleTestUtils.IPCUtils import ipcmessagerouter
from CastleTestUtils.STSClientServer.Server.STSServiceProxy import STSServiceProxy
from CastleTestUtils.STSClientServer.Server.STSAccountProxy import STSAccountProxy
from CastleTestUtils.MockServices.MockFrontDoor.Server.mockfrontDoor import MockFrontDoor
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.MockServices.mock_lpm.mock_lpm import MockLPM
from CastleTestUtils.MockServices.mock_ap_server.mock_ap_server import MockAPServer

LOGGER = get_logger(__name__)


@pytest.fixture(scope="session")
def start_all_mocks(request, start_mock_frontDoor,
                             start_mock_lpm,
                             start_cli_server,
                             start_mock_apserver,
                             eddie_product_controller):
    """
    Starts all the mocks in a specific order
    """
    LOGGER.info("Booting the ProductController")
    time.sleep(3)
    json_msg = {"header":{"device":"",
                          "resource":"/ui/alive",
                          "method":"PUT",
                          "msgtype":"REQUEST",
                          "reqID":0,
                          "version":1.000000,
                          "status":200},
                "body":{"count" : 1621134}}
    start_mock_frontDoor.send_message(json_msg)   
    time.sleep(5)
    
    LOGGER.info("Start serving STSService")
    router = ipcmessagerouter.ipcmessagerouterserver(port=30030)
    sts_service = STSServiceProxy(router)
    sts_service.connect()

    time.sleep(3)
    sts_service.close()

def start_service(cmd, service_name):
    """
    Function to start a service
    :return: return code of the process
    """
    LOGGER.info("Launching - {0} cmd={1}".format(service_name, cmd))
    ret = os.system(cmd)
    if ret != 0:
        LOGGER.info("Failed to run {0}, ret code={1}".format(service_name, ret))
    return ret

def stop_service(service_name):
    """
    Function to stop a service
    """
    killed = False
    for proc in psutil.process_iter():
        if proc.name() == service_name:
            LOGGER.info("Got {0} running - Terminating".format(proc.name()))
            proc.kill()
            killed = True
    if not killed:
        LOGGER.info("Failed to kill {0} ".format(service_name))

@pytest.fixture(scope="session")
def start_mock_frontDoor(request):
    """
    Starts a mock local FrontDoor server at port 8084
    """
    mockfd = None
    def teardown():
        """
        Destroys the previously created mock local FrontDoor server
        """
        if mockfd:
            mockfd.close()
    request.addfinalizer(teardown)

    mockfd = MockFrontDoor(PORT=8084)
    mockfd.run()
    return mockfd


@pytest.fixture(scope="session")
def start_mock_lpm(request):
    """
    Starts a mock lpm at port 17023
    """
    def teardown():
        """
        Destroys previous mock lpm
        """
        if mock_lpm:
            mock_lpm.close()
    request.addfinalizer(teardown)
    mock_lpm = MockLPM(port=17023)
    mock_lpm.listen()

    return mock_lpm

@pytest.fixture(scope="session")
def eddie_product_controller(request):
    """
    Start the product controller
    1. Move the checked-in files as needed by ProductController
    2. Start the Product Controller

    TODO: Change all os.system calls to subprocess.
          Ensure the file-system is cleaned up during the start / completion of session
    """
    # Move json files to correct directories
    LOGGER.info("Moving files to correct directories. Please make sure json files are in your current directory.")
    exec_path = os.path.abspath(os.path.join(os.path.dirname(__file__), "../builds/Coverage/x86_64/bin"))
    mock_product_perisistence_path = "/mnt/nv/product-persistence"
    mock_keyconfiguration_path = "/scratch/CastleProducts/builds/Coverage/x86_64/KeyConfiguration.json"
    # PAF - I map the opt and persist directory pulled from target
    #subprocess.Popen(['sudo', 'mkdir', '-p', '/opt/Bose/etc'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=False)
    #subprocess.Popen(['sudo', 'mkdir', '-p', '/persist'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=False)
    #subprocess.Popen(['sudo', 'cp', 'mfg_data.json', '/persist'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=False)

    # I will cp the Mock ./mnt/nv/product-persistence to exec_path for now
    #subprocess.Popen(['sudo', 'cp', '-r', 'product-persistence', exec_path] , stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=False)
    subprocess.Popen([ 'cp', '-r', mock_product_perisistence_path, exec_path] , stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=False)

    #subprocess.Popen(['sudo', 'cp', 'KeyConfiguration.json', '/var/run'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=False)
    subprocess.Popen(['sudo', 'cp', mock_keyconfiguration_path, '/var/run'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=False)
    #subprocess.Popen(['sudo', 'cp', '-r', '*.json', 'chimes', '/opt/Bose/etc'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=False)


    time.sleep(5)
    os.chdir(exec_path)
    LOGGER.info("Starting product controller")
    cmd = "env BOSE_DPRINT_CONF='set stdout; all debug' ./Professor &"
    #cmd = "strace -tt -yy -e trace=ipc -s 128 -E \"BOSE_DPRINT_CONF=\'set stdout; all debug\'\" ./Professor &"
    #cmd = "env BOSE_DPRINT_CONF='set stdout; all debug' strace -o strace -ff -tt -yy -e trace=open,read,write,close,ipc,network -x -s 4096  ./Professor &"
    start_service(cmd, "ProductController")
    def teardown():
        """teardown call for stopping service"""
        stop_service("ProductController")
    request.addfinalizer(teardown)

@pytest.fixture(scope="session")
def start_cli_server(request):
    """
    Start the CLI-Server
    """
    exec_path = os.path.abspath(os.path.join(os.path.dirname(__file__), "./builds/CliService"))
    os.chdir(exec_path)
    LOGGER.info("Starting CLI Server")
    cmd = "env BOSE_DPRINT_CONF='set stdout; all debug' ./CLIServer &"
    time.sleep(3)
    start_service(cmd, "CLIServer")
    def teardown():
        """teardown call for stopping service"""
        stop_service("CLIServer")
    request.addfinalizer(teardown)

@pytest.fixture(scope='session')
def start_sts_service(request):
    """
    Fixture to start Mock STSService
    """
    LOGGER.info("Start serving STSService")
    router = ipcmessagerouter.ipcmessagerouterserver(port=30030)
    sts_service = STSServiceProxy(router)
    sts_service.connect()

    def teardown():
        """ Close STSService Proxy object """
        LOGGER.info("Closing connection to STSService")
        sts_service.close()
    request.addfinalizer(teardown)

    return sts_service


@pytest.fixture(scope='session')
def start_sts_account(request):
    """
    Fixture to start Mock STSAccountProxy
    """
    LOGGER.info("Start serving STSAccountProxy")
    router = ipcmessagerouter.ipcmessagerouterserver(port=30031)
    sts_account = STSAccountProxy(router)
    sts_account.connect()

    def teardown():
        """ Close STSAccount Proxy object """
        LOGGER.info("Closing connection to STSAccountProxy")
        sts_account.close()
    request.addfinalizer(teardown)

    return sts_account

@pytest.fixture(scope='session')
def start_mock_apserver(request):
    LOGGER.info("Starting the AP Server")
    ap_server = MockAPServer(port=40099,logger=LOGGER)
    ap_server.listen()

    def teardown():
        LOGGER.info("Closing connection to MockAPServer")
        ap_server.close()
    request.addfinalizer(teardown)
    
@pytest.fixture(scope='session')
def start_generic_mock_service(request):
    """
    Fixture to start Generic Mock Service
    """
    LOGGER.info("Starting Generic Mock Service")
    msp_data = os.path.join(os.path.dirname(os.path.realpath(__file__)), "msp_source_conf.json")
    GMS = GenericMockService(msp_data)
    GMS.run()
    GMS.wait_for_connect(11)
    yield(GMS)
