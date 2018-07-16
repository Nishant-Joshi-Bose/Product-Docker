# lightbar_helper.py
#
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
Automated Tests for LightBar Animations for Eddie
"""
import os
import re
import time

from CastleTestUtils.SupportUtils.FileUtils.CsvFileSupport import read_rows_as_dictionary_list_from_csv_file
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.OAuthUtils.OAuthUtils import UserAccount
from CastleTestUtils.FrontDoorAPI.FrontDoorQueue import FrontDoorQueue
from CastleTestUtils.RivieraUtils.rivieraUtils import RivieraUtils


ANIMATION_NAME = 'Animation Name'
INVALID_ANIMATION = "animation not supported"
PLAY_STOP_ANIMATION_TIMEOUT = 15

LIGHTBAR_API = "/ui/lightbar"
METHOD_PUT = "PUT"
LB_HISTORY = 'lb history'

LOGGER = get_logger(__file__)


def get_notification_in_list(front_door_queue, notif_id):
    """
    function to get specific notification list.
    :param front_door_queue: FrontDoorQueue instance connected to device
    :param notif_id: Keyword or list of keyword to
                     find out specific notification
    :return notif_list: list of received notifications for particular notif_id
    """
    # Giving the option to just enter one id or a list
    if not isinstance(notif_id, list):
        notif_id = [notif_id]

    notif_list = list()

    while not front_door_queue.queueEmpty():
        notification = front_door_queue.getNotification()

        if notification["header"]["resource"] in notif_id:
            notif_list.append(notification)

    return notif_list


def negative_animation_data():
    """
    This function will return the list of dictionary of data
    from the csv file.
    :return lightbar_data : List of negative animation data
    """
    LOGGER.info("Get the negative lightbar data")

    current_directory = os.path.dirname(os.path.realpath(__file__))
    negative_scenarios_file_name = os.path.join(current_directory, 'lightbar_negative_testdata.csv')
    lightbar_data = read_rows_as_dictionary_list_from_csv_file(negative_scenarios_file_name)[1]

    return lightbar_data


def get_animation_from_lpm(serial_handler, animation_value):
    """
    Fetch the last animation value played in device through LPM logs.
    :param serial_handler : Instance of LPM
    :param animation_value : expected animation value
    :return serial_last_animation : animation value from LPM logs
    """
    serial_last_animation_string = 'Animation Name: (.+?), S'

    serial_last_animation = None

    for retry in range(PLAY_STOP_ANIMATION_TIMEOUT):
        LOGGER.debug("Verifying from LPM serial logs while retry: %s", retry)

        serial_outputs = serial_handler.execute_command(LB_HISTORY)
        LOGGER.debug("History Serial Log: %s", serial_outputs)

        serial_animations = [serial_output for serial_output in serial_outputs
                             if ANIMATION_NAME in serial_output]

        serial_last_animation = re.search(serial_last_animation_string, serial_animations[-1]).group(1)

        if serial_last_animation == animation_value:
            LOGGER.debug("Expected Animation (%s), Played Animation (%s)", animation_value, serial_last_animation)
            return serial_last_animation

    LOGGER.debug("Expected animation value %s not played in serial logs.", animation_value)
    return serial_last_animation


def animation_in_notification(front_door_queue, expected_animation_value):
    """
    Fetch the last animation value played in device through API notifications.

    :param front_door_queue : Instance of FrontDoorQueue
    :param expected_animation_value : expected animation value
    :return current_animation_value : animation value from API Notifications
    """
    lb_notifications = get_notification_in_list(front_door_queue, LIGHTBAR_API)

    for notification_value in lb_notifications:
        try:
            current_animation_value = notification_value["body"]["currentValue"]["value"]
            if current_animation_value == expected_animation_value:
                LOGGER.debug("Animation value received: %s", current_animation_value)
                return current_animation_value

            LOGGER.debug("Current (%s) does not match Expected (%s).",
                         current_animation_value, expected_animation_value)
        except KeyError:
            LOGGER.debug("Notification not found in %s.", notification_value)
            continue


def get_lightbar_animations(device_id, ip_address, environment):
    """
    Get the list of supported animations

    Steps:--
    1.. Create a passport user
    2.. Add product to the user
    3.. Get the supported lightbar animations
    4.. Delete the passport user and close the FrontDoor connection.

    :param device_id: Device id
    :param ip_address: Device ip address
    :param environment: Galapagos environment
    :return animations: Supported animations
    """
    # create the user to get the supported animations values
    LOGGER.info("passport_user_details")
    fname = "lightbar"
    lname = "automation"
    _dname = str(time.time() * 1000)
    LOGGER.debug("_dname is: %s", _dname)
    email = "LightBar_Automation" + _dname + "@bose.com"
    password = "bose901"
    LOGGER.debug("email is: %s", email)
    gigya_url = "https://ingress-platform.live-aws-useast1.bose.io/dev/svc-id-gen-pub/" + \
        environment + "/id-user-accounts-core/userAccounts/"

    LOGGER.info("Creating passport user for lightbar response")
    user_account = UserAccount(url=gigya_url, logger=LOGGER)
    user_account.create_user_account(email, password, lname, fname)

    riviera_utils = RivieraUtils('ADB', device=device_id)
    response = riviera_utils.link_product(environment=environment, ip_address=ip_address,
                                          email=email, password=password)
    LOGGER.info("Response is %s", response)
    assert riviera_utils.wait_for_galapagos_activation(timeout=120), "Galapagos status error"
    LOGGER.info("frontDoorQueue")
    front_door = FrontDoorQueue(ip_address, email=email, password=password)

    # get the supported lightbar animations
    lightbar_response = front_door.getActiveAnimation()
    animation_values = lightbar_response["properties"]["supportedValues"]
    front_door.close()

    # delete the user account
    LOGGER.info("Deleting User-Account")
    user_account.delete_user_account(email, password)

    return animation_values
