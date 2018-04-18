"""
Organization: BOSE CORPORATION
Copyright: COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
This program may not be reproduced, in whole or in part in any
form or any means whatsoever without the written permission of:
BOSE CORPORATION
The Mountain,
Framingham, MA 01701-9168

Common conftest file for Eddie Test
"""

import pytest
import time
from CastleTestUtils.CAPSUtils.TransportUtils.commonBehaviorHandler import CommonBehaviorHandler
from CastleTestUtils.CAPSUtils.TransportUtils.messageCreator import MessageCreator
from CastleTestUtils.CAPSUtils.TransportUtils.responseHandler import ResponseHandler
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.PassportUtils.passport_utils import *
from CastleTestUtils.PassportUtils.passport_api import PassportAPIUsers
from global_resources_data import RESOURCES

logger = get_logger(__name__)

def pytest_addoption(parser):
    """
    Command line options for the pytest tests in this module.

    :param parser: Parser used for method.
    :return: None
    """
    parser.addoption("--config", action="store", dest="conf", default='config.py',
                     help="Test configuration file")

@pytest.fixture(scope='class')
def get_config(request):
    """
    This fixture will return the global resource data with specific to single resource to create object of handler
    """
    conf = {}
    exec(open(request.config.option.conf).read(), conf)
    CONFIG = conf['CONFIG']
    for source_name, params in CONFIG['music'].iteritems():
        if params['active']:
            resource = RESOURCES[(params['resourcename'])]
            break
    return resource

@pytest.fixture(scope='class')
def music_sources(request):
    """
    This fixture will return the music sources name to play on Eddie
    """
    conf = {}
    exec(open(request.config.option.conf).read(), conf)
    CONFIG = conf['CONFIG']
    sources = {}
    for source_name, params in CONFIG['music'].iteritems():
        if params['active']:
            sources[source_name] = params['resourcename']
    return sources

@pytest.fixture(scope='class')
def device_type(frontDoor):
    """
    Use front door API to obtain device type.
    """
    logger.info("device_type")
    sys_info = frontDoor.getInfo()
    device_type = sys_info["body"]["productType"]
    logger.debug("Device type: %s", device_type)
    return device_type

@pytest.fixture(scope='class')
def service_name(request):
    """
    Get service name from config file.
    """
    logger.info("service_name")
    conf = {}
    exec(open(request.config.option.conf).read(), conf)
    CONFIG = conf['CONFIG']
    for source_name, params in CONFIG['music'].iteritems():
        if params['active'] and params['resourcename'] != '':
            resource = RESOURCES[(params['resourcename'])]
            break
    return resource['provider']

@pytest.fixture(scope='class')
def message_creator(request, service_name):
    """
    Get MessageCreator instance.
    """
    logger.info("message_creator")
    return MessageCreator(service_name, logger)

@pytest.fixture(scope='class')
def response_handler(request, service_name, get_config):
    """
    Get ResponseHandler instance.
    """
    logger.info("response_handler")
    return ResponseHandler(service_name, get_config['name'], logger)

@pytest.fixture(scope='class')
def common_behavior_handler(response_handler, frontDoor, message_creator):
    """
    Get CommonBehaviorHandler instance.
    """
    logger.info("common_behavior_handler")

    return CommonBehaviorHandler(frontDoor, response_handler, message_creator, logger)

@pytest.fixture(scope='class')
def passport_user(request, frontDoor):

    logger.info("passport_user")
    passport_base_url = request.config.getoption('--passport-base-url')
    apikey = request.config.getoption('--api-key')
    logger.info("Bose Person ID : %s ", frontDoor._bosepersonID)
    passportUser = PassportAPIUsers(frontDoor._bosepersonID, apikey, frontDoor._access_token, passport_base_url, logger)
    return passportUser

@pytest.fixture(scope='class')
def multiple_music_service_account(request, passport_user, get_config, common_behavior_handler):
    """
    Add multiple music service account
    :param request: pytest request fixture
    :param passport_user: fixture returns reference to current PassportAPIUsers
    :param get_config: fixture returns dictionary of current config loaded from either global resources or config file
    """
    logger.info("multiple_music_service_account")
    conf = {}
    exec(open(request.config.option.conf).read(), conf)
    CONFIG = conf['CONFIG']
    multiple_account_Id = []
    for source_name, params in CONFIG['music'].iteritems():
        if params['active'] and params['resourcename'] != '' and source_name != 'TUNEIN':
            resource = RESOURCES[(params['resourcename'])]
            account_id = passport_user.add_service_account(service=source_name, accountID=resource['name'], account_name=resource['provider_account_id'], refresh_token=resource['secret'])
            assert account_id and account_id != "", "Fail to add music service account."
            multiple_account_Id.append(account_id)
    common_behavior_handler.performCloudSync()

    def multiple_remove_music_service():
        """
        Multiple Remove music service account
        """
        logger.info("multiple_remove_music_service")
        for index, account_id in enumerate(multiple_account_Id):
            # get account id for music service
            assert passport_user.remove_service_account(account_id), "Fail to remove music account from passport account."

    request.addfinalizer(multiple_remove_music_service)



@pytest.fixture(scope='class')
def verify_device_source(request, common_behavior_handler):
    """
    This fixture waits till source is ready on device and fails if not found ready.
    :param common_behavior_handler: fixture returns reference to commonBehaviourHandler
    :param service_name: fixture returns service_name specified from command line
    """
    logger.info("service_name")
    conf = {}
    exec(open(request.config.option.conf).read(), conf)
    CONFIG = conf['CONFIG']
    for source_name, params in CONFIG['music'].iteritems():
        if params['active'] and params['resourcename'] != '':
            resource = RESOURCES[(params['resourcename'])]
            common_behavior_handler.checkSourceStatus(source_name, resource['name'])
