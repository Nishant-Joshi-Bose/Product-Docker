# factory_utils.py
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
Utility class for working with Eddie factory default.
"""
SYSTEM_STATE_NOTIF = "/system/state"
SYSTEM_BUTTON_NOTIF = "/system/buttonEvent"
EVENT_FACTORY = "FACTORY_DEFAULT"
EVENT_STATE_CANCEL = "CANCEL"
EVENT_STATE_OK = "COMPLETED"
EVENT_STATE_COUNTDOWN = "COUNTDOWN"


def get_notification_in_list(fdq, notif_id):
    """
    function to get specific notification list.

    :param fdq: FrontDoorQueue instance connected to device
    :param notif_id: Notification is reseived as resource in header
    :return sys_state_notif_list: list of received notifications of notif_id
    """
    sys_state_notif_list = list()
    while not fdq.queueEmpty():
        notification = fdq.getNotification()
        if notification["header"]["resource"] == notif_id:
            sys_state_notif_list.append(notification)

    return sys_state_notif_list


def get_last_system_state_notification(fdq):
    """
    function to get last system state notification and return.

    :param fdq: FrontDoorQueue instance connected to device
    :return sys_state: system state received in notification
    """
    notif_list = get_notification_in_list(fdq, SYSTEM_STATE_NOTIF)
    if len(notif_list):
        notification = notif_list.pop()
        return notification["body"]["state"]


def get_factory_default_event_notification(fdq):
    """
    function to get buttonEvent state notification and return.

    :param fdq: FrontDoorQueue instance connected to device
    :return event_state: factoryDefault buttonEvent state
    """
    notif_list = get_notification_in_list(fdq, SYSTEM_BUTTON_NOTIF)
    while len(notif_list):
        notification = notif_list.pop()
        if notification["body"]["event"] == EVENT_FACTORY:
            return notification["body"]["state"]

    return None


def get_event_notification_after_countdown(fdq, event, counts):
    """
    function to verify COUNTDOWN buttonEvents and return last state.

    :param fdq: FrontDoorQueue instance connected to device
    :param event: Event is needs to be check
    :param counts: Expected No of counts to be generated
    :return counts: No of counts not received
    :return event_state: last event state
    """
    notif_list = list(fdq._q.queue)
    notif_list.reverse()
    while len(notif_list):
        notification = notif_list.pop()
        if notification["header"]["resource"] != SYSTEM_BUTTON_NOTIF:
            continue
        if notification["body"]["event"] == event:
            if notification["body"]["state"] == EVENT_STATE_COUNTDOWN and notification["body"]["value"] == counts:
                counts = counts - 1
            else:
                return counts, notification["body"]["state"]

    return counts, None
