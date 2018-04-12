import pytest
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
    execfile(request.config.option.conf, conf)
    CONFIG = conf['CONFIG']
    for sourceName, params in CONFIG['music'].iteritems():
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
    execfile(request.config.option.conf, conf)
    CONFIG = conf['CONFIG']
    sources = []
    for sourceName, params in CONFIG['music'].iteritems():
        if params['active']:
            sources.append(sourceName)
    return sources

@pytest.fixture(scope='class')
def device_type(frontDoor):
    """
    Use front door API to obtain device type.
    """
    logger.info("device_type")
    sysInfo = frontDoor.getInfo()
    device_type = sysInfo["body"]["productType"]
    logger.debug("Device type: %s", device_type)
    return device_type

@pytest.fixture(scope='class')
def service_name(request):
    """
    Get service name from config file.
    """
    logger.info("service_name")
    conf = {}
    execfile(request.config.option.conf, conf)
    CONFIG = conf['CONFIG']
    for sourceName, params in CONFIG['music'].iteritems():
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
    return MessageCreator(service_name)

@pytest.fixture(scope='class')
def response_handler(request, service_name, get_config):
    """
    Get ResponseHandler instance.
    """
    logger.info("response_handler")
    return ResponseHandler(service_name, get_config['name'])

@pytest.fixture(scope='class')
def common_behavior_handler(response_handler, frontDoor, message_creator):
    """
    Get CommonBehaviorHandler instance.
    """
    logger.info("common_behavior_handler")

    return CommonBehaviorHandler(frontDoor, response_handler, message_creator)

@pytest.fixture(scope='class')
def passport_user(request, common_behavior_handler):
    """
    Add new passport user
    :param request: pytest request fixture
    """
    logger.info("passport_user")
    passport_base_url = request.config.getoption('--passport-base-url')
    apikey = request.config.getoption('--api-key')

    # Create passport account
    bosepersonID = create_passport_account(passport_base_url, "Eddie_MultiRoom", "Bose", apikey)
    passportUser = PassportAPIUsers(bosepersonID, apikey, passport_base_url)

    def delete_passport_user():
        """
        Delete Passport User Account once test completed
        """
        logger.info("delete_passport_user")
        assert passportUser.delete_users(), "Fail to delete person id: %s" % bosepersonID
        common_behavior_handler.performCloudSync()

    request.addfinalizer(delete_passport_user)
    return passportUser

@pytest.fixture(scope='class')
def multiple_music_service_account(request, passport_user, get_config):
    """
    Add multiple music service account
    :param request: pytest request fixture
    :param passport_user: fixture returns reference to current PassportAPIUsers
    :param get_config: fixture returns dictionary of current config loaded from either global resources or config file
    """
    logger.info("multiple_music_service_account")
    conf = {}
    execfile(request.config.option.conf, conf)
    CONFIG = conf['CONFIG']
    multiple_account_Id = []
    for sourceName, params in CONFIG['music'].iteritems():
        if params['active'] and params['resourcename'] != '' and sourceName != 'TUNEIN':
            resource = RESOURCES[(params['resourcename'])]
            accountId = passport_user.add_service_account(service=sourceName, accountID=resource['name'], account_name=resource['provider_account_id'], refresh_token=resource['secret'])
            assert accountId and accountId != "", "Fail to add music service account."
            multiple_account_Id.append(accountId)

    def multiple_remove_music_service():
        """
        Multiple Remove music service account
        """
        logger.info("multiple_remove_music_service")
        for index, accountId in enumerate(multiple_account_Id):
            # get account id for music service
            assert passport_user.remove_service_account(accountId), "Fail to remove music account from passport account."

    request.addfinalizer(multiple_remove_music_service)


@pytest.fixture(scope='class')
def add_device_to_passport(passport_user, device_guid, device_type, common_behavior_handler):
    """
    Add device to passport account
    :param passport_user: fixture returns reference to current PassportAPIUsers
    :param device_id: fixture returns device_id
    :param device_type: fixture returns device_type
    """
    logger.info("add_device_to_passport")
    assert passport_user.add_product(device_guid, device_type), "Failed to add device to passport account."
    common_behavior_handler.performCloudSync()

@pytest.fixture(scope='class')
def verify_device_source(request, common_behavior_handler):
    """
    This fixture waits till source is ready on device and fails if not found ready.
    :param common_behavior_handler: fixture returns reference to commonBehaviourHandler
    :param service_name: fixture returns service_name specified from command line
    """
    logger.info("service_name")
    conf = {}
    execfile(request.config.option.conf, conf)
    CONFIG = conf['CONFIG']
    for sourceName, params in CONFIG['music'].iteritems():
        if params['active'] and params['resourcename'] != '':
            resource = RESOURCES[(params['resourcename'])]
            common_behavior_handler.checkSourceStatus(sourceName, resource['name'])
