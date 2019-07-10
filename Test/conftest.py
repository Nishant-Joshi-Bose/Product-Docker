import os
import psutil
import pytest
from pytest_testrail.plugin import pytestrail

from CastleTestUtils.MockServices.MockFrontDoor.Server.mockfrontDoor import MockFrontDoor
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger

def start_service(cmd, service_name):                                                                              
    """                                                                                            
    Function to start a service                                                                                
    :return: return code of the process                                                                       
    """                                                                            
    print("Launching - {0} cmd={1}".format(service_name, cmd))                                                    
    ret = os.system(cmd)                                                                                                
    if ret != 0:                                                                                                        
        print("Failed to run {0}, ret code={1}".format(service_name, ret))                                        
    return ret                                                                                               
                                                                                                                        
def stop_service(service_name):                                                                                         
    """                                                                                                                 
    Function to stop a service                                                                                          
    """                                                                                                                 
    killed = False
    for proc in psutil.process_iter():
        if proc.name() == service_name:
            print("Got {0} running - Terminating".format(proc.name()))
            proc.kill()
            killed = True
    if not killed:
        print("Failed to kill {0} ".format(service_name))

@pytest.fixture(scope="module")
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
def eddie_product_controller(request):
    """
    Start the product controller
     
    1. Move the checked-in files as needed by ProductController
    2. Start the Product Controller
    """
    # Move json files to correct directories
    print("Moving files to correct directories. Please make sure json files are in your current directory")
    os.system("mkdir -p /opt/Bose/etc")
    os.system("mv -v KeyConfiguration.json /var/run")
    os.system("mv -v *.json chimes /opt/Bose/etc")
    
    # Run product controller
    os.chdir("/scratch/CastleProducts/builds/Debug/x86_64/bin")
    print("Starting product controller")
    cmd = "env BOSE_DPRINT_CONF='set stdout' ./ProductController"
    start_service(cmd, "Product_Controller")
    def teardown():
        """teardown call for stopping service"""
        stop_service("Product_Controller")
        
        
        
        
        
