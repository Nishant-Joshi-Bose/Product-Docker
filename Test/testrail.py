# :Organization:  BOSE CORPORATION
#
# :Copyright:  COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
#              This program may not be reproduced, in whole or in part in any
#              form or any means whatsoever without the written permission of:
#                  BOSE CORPORATION
#                  The Mountain,
#                  Framingham, MA 01701-9168
#
"""
Utility class for connecting to Testrail server and query for test cases
"""
import urllib2
import json
import base64
import os
import ConfigParser

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
logger = get_logger(__name__)

def retrieve_test_cases():
    """
    retrieve_test_cases method will connect to Testrail server and query
    for test cases title and test case id using suite and project id
    return: test case id and test case title in a list format
    """
    # Testrail Authentication info
    config = ConfigParser.SafeConfigParser()
    config_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../"))
    config_file = config_dir + "/" + "testrailsettings.cfg"
    config.read(config_file)

    email = config.get('API', 'email')
    logger.info("The email config is %s", email)
    password = config.get('API', 'password')
    logger.info("The password config is %s", password)
    base_url = config.get('API', 'url')
    logger.info("The base_url config is %s", base_url)
    project_id = config.get('TESTRUN', 'project_id')
    logger.info("The project_id config is %s", project_id)
    suite_id = config.get('TESTRUN', 'suite_id')
    logger.info("The suite_id config is %s", suite_id)

    # The path of TestRail's API
    url = '{}index.php?/api/v2/'.format(base_url)
    # The API method that is called
    uri = url + 'get_cases/{}&suite_id={}'.format(project_id, suite_id)

    # Authorization
    auth = base64.b64encode('%s:%s' % (email, password))
    # Library for opening testrail URL
    request = urllib2.Request(uri)
    # Header to the request. They are added to the list of headers sent to the server
    request.add_header('Authorization', 'Basic %s' % auth)
    request.add_header('Content-Type', 'application/json')
    # Open the testrail URL
    response = urllib2.urlopen(request).read()
    # JSON response of the queried API
    result = json.loads(response)
    # Key Value Pair for testid and testname
    tests = []
    for item in result:
        logger.info('{}, {}'.format(item['id'], item['title']))
        testid = "C"+format(item['id'])
        testname = format(item['title'])
        value = {testid:testname}
        tests.append(value)
    return tests
