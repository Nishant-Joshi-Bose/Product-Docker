# test_memoryConsumption.py
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
End-to-End test showing CPU and Memory consumption for the Riviera product.
"""

import csv
import re
import os
import time
import logging
import threading
from datetime import datetime, time as dttime
import pexpect

import pytest

import hashlib

from conf_memoryConsumption import CONFIG
from CastleTestUtils.RivieraUtils.rivieraCommunication import ADBCommunication
from CastleTestUtils.MemoryUtils.graphiteServerUtils import GraphiteUtils
from CastleTestUtils.MemoryUtils.memoryInfo import MemoryInfo
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils.hardware.keys import keypress
from CastleTestUtils.SupportUtils import emailSupport


from enum import IntEnum


class Keys(IntEnum):
    """
    Enum of all hardware key number identifiers.
    """
    BLUETOOTH = 1
    AUX = 2
    VOLUME_UP = 3
    MULTIFUNCTION = 4
    VOLUME_DOWN = 5
    ACTION = 6
    PRESET_1 = 65
    PRESET_2 = 66
    PRESET_3 = 67
    PRESET_4 = 68
    PRESET_5 = 69
    PRESET_6 = 70

adb = ADBCommunication()

global BRANCH
global process_details
global logger

logger = get_logger(__name__, "MemoryConsumption.log", level=logging.INFO, fileLoglevel=logging.DEBUG)

def get_memory_details(device, column_name_1="Mem Used Before",
                     column_name_2="Mem Free Before",
                     column_name_3="CPU Used Before", run_time=1):
    """
    Get Memory information via Top command and iterate loop for 5 minutes to take average of it.
    """
    try:
        global process_details
        range_iterate = 0
        memory_free = 0
        memoryInfo = MemoryInfo(device)
        logger.info("run_time Play : " + str(run_time))
        t_end = time.time() + 60 * int(run_time)
        while time.time() < t_end:
            logger.info(" -------- get_memory_details top -n 1 temp -------- ")
            cpu_data = memoryInfo.getCPUUsage()
            CONFIG["Memory_Consumption"]["FILE_COLUMN"][column_name_3] += float(cpu_data)
            range_iterate = range_iterate + 1
            time.sleep(CONFIG["Memory_Consumption"]["trailing_delay"])
            logger.info(" --------   Calling memory_capture function -------- ")
            memory_free, memory_used = memoryInfo.getMemoryUsage()
            CONFIG["Memory_Consumption"]["FILE_COLUMN"][column_name_2] += int(memory_free)
            logger.info("Memory Details :" + column_name_2 + " : "+ str(memory_free))
            CONFIG["Memory_Consumption"]["FILE_COLUMN"][column_name_1] += int(memory_used)
            logger.info("Memory Details :" + column_name_1 + " : "+ str(memory_used))
            logger.info(" -------- Calling getTop10Proces function -------- ")
            process_details = memoryInfo.getTop10Process()
        logger.debug(CONFIG["Memory_Consumption"]["FILE_COLUMN"][column_name_1])
        logger.debug(CONFIG["Memory_Consumption"]["FILE_COLUMN"][column_name_2])
        logger.debug(CONFIG["Memory_Consumption"]["FILE_COLUMN"][column_name_3])
        CONFIG["Memory_Consumption"]["FILE_COLUMN"][column_name_1] /= range_iterate
        CONFIG["Memory_Consumption"]["FILE_COLUMN"][column_name_2] /= range_iterate
        CONFIG["Memory_Consumption"]["FILE_COLUMN"][column_name_3] /= range_iterate
    except Exception as exception:
        error_string = "get_memory_details: Memory Details Capture Unsuccessful - %s" % str(exception)
        logger.error(error_string)
        assert False, error_string

def get_build_version(riviera_utils, request):
    """
    Get the Software Version of the Device for Grafana display purpose
    return: FS_VERSION string
    """
    try:
        logger.info(" ------------ Software Build Version ---------- ")
        fs_version = riviera_utils.getDeviceFsVersion()
        return fs_version.split()[0]
    except Exception as exception:
        error_string = "get_build_version: Get Build Version Unsuccessful - %s" % str(exception)
        logger.error(error_string)
        assert False, error_string

def write_csv_file():
    """
    Write CSV file and if file exist then append data in existing file
    """
    try:
        file_mode = 'a'
        file_exists = os.path.isfile(CONFIG["Memory_Consumption"]["FILE_NAME"])

        if not file_exists:
            file_mode = 'w'

        with open(CONFIG["Memory_Consumption"]["FILE_NAME"], file_mode) as csvfile:
            fieldnames = ['Branch', 'Build', 'Date', 'ubi0:rootfs usage',
                          'Mem Used Before', 'Mem Free Before',
                          'CPU Used Before', 'Mem Used After',
                          'Mem Free After', 'CPU Used After']
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
            if not file_exists:
                writer.writeheader()
            writer.writerow({'Branch': BRANCH, 'Build': CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Build'],
                             'Date': time.strftime("%m/%d/%Y"),
                             'Mem Used Before': str(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Mem Used Before']) + 'K',
                             'Mem Free Before': str(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Mem Free Before']) + 'K',
                             'CPU Used Before': str(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['CPU Used Before']) + '%',
                             'Mem Used After': str(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Mem Used After']) + 'K',
                             'Mem Free After': str(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Mem Free After']) + 'K',
                             'CPU Used After': str(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['CPU Used After']) + '%'})
    except Exception as exception:
        error_string = "write_csv_file: Write CSV File Unsuccessful - %s" % str(exception)
        logger.error(error_string)
        assert False, error_string


def send_mail_from_files():
    """
    Send mail to the configured email address showing collected during run.

    :return: None
    """
    files = [CONFIG["Memory_Consumption"]["FILE_NAME"]]
    subject = "Trunk Nightly Build Memory Report"
    body = "Please find the attached memory report for Professor\n\nView the Grafana board for this test here:\n" \
           "http://ssg-dashboard.bose.com/d/SiehovKik/memory-cpu-disk-io-usage-overview-professor?orgId=1&from=now-3d&to=now"
    emailSupport.send_email(sender=CONFIG["Device_Param"]["SEND_FROM"],
                            to=CONFIG["Device_Param"]["SEND_TO"],
                            subject=subject,
                            msg_body=body,
                            attach=files)
    logger.info(" -------- send email Completed -------- ")


def push_data_to_grafana(branch, table_name=None):
    """
    Pushes the dataset to the Graphite instance.

    :param branch: Un-used
    :param table_name: Table Name to use
    :return: None
    """
    graphite = GraphiteUtils(CONFIG["Device_Param"]["CARBON_SERVER"],
                             CONFIG["Device_Param"]["CARBON_PORT"],
                             CONFIG["Device_Param"]["CARBON_HTTP_PORT"])

    logger.info(" -------- Graph Data Process Start -------- ")
    timestamp = int(time.time())

    data = build_memory_data(timestamp, table_name)
    graphite.graphite_push_data(data)

    data = build_cpu_data(timestamp, table_name)
    graphite.graphite_push_data(data)

    data = build_disk_data(timestamp, table_name)
    graphite.graphite_push_data(data)

    data = build_io_data(timestamp, table_name)
    graphite.graphite_push_data(data)

    event_data = build_event_data(timestamp, table_name)
    graphite.graphite_push_event(event_data)

    data = build_process_data(timestamp, table_name)
    graphite.graphite_push_data(data)

    logger.info(" -------- Graph Data Process End  -------- ")


def build_memory_data(timestamp, table_name=None):
    """
    Building Memory Data
    """
    memory_table = CONFIG["Memory_Consumption"]["GRAPHITE_Memory_Table"]
    if not table_name:
        lines = [memory_table + '.Build {0} {1}'.format(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Build'], timestamp),
                 memory_table + '.memory_usedBefore {0} {1}'.format(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Mem Used Before'], timestamp),
                 memory_table + '.memory_freeBefore {0} {1}'.format(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Mem Free Before'], timestamp),
                 memory_table + '.memory_usedAfter {0} {1}'.format(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Mem Used After'], timestamp),
                 memory_table + '.memory_freeAfter {0} {1}'.format(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Mem Free After'], timestamp)]
        message = '\n'.join(lines) + '\n'
        logger.info(message)
    else:
        lines = [memory_table + '.' + table_name + '.Build {0} {1}'.format(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Build'], timestamp),
                 memory_table + '.' + table_name + '.memory_usedBefore {0} {1}'.format(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Mem Used Before'], timestamp),
                 memory_table + '.' + table_name + '.memory_freeBefore {0} {1}'.format(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Mem Free Before'], timestamp),
                 memory_table + '.' + table_name + '.memory_usedAfter {0} {1}'.format(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Mem Used After'], timestamp),
                 memory_table + '.' + table_name + '.memory_freeAfter {0} {1}'.format(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Mem Free After'], timestamp)]
        message = '\n'.join(lines) + '\n'
        logger.info(message)
    return message


def build_cpu_data(timestamp, table_name=None):
    """
    Building CPU Data
    """
    cpu_table = CONFIG["Memory_Consumption"]["GRAPHITE_CPU_Table"]
    if not table_name:
        lines = [cpu_table + '.Build {0} {1}'.format(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Build'], timestamp),
                 cpu_table + '.CPUUsedBefore {0} {1}'.format(str(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['CPU Used Before'])[:-1], timestamp),
                 cpu_table + '.CPUUsedAfter {0} {1}'.format(str(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['CPU Used After'])[:-1], timestamp)]
        message = '\n'.join(lines) + '\n'
        logger.info(message)
    else:
        lines = [cpu_table + '.' + table_name + '.Build {0} {1}'.format(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Build'], timestamp),
                 cpu_table + '.' + table_name + '.CPUUsedBefore {0} {1}'.format(str(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['CPU Used Before'])[:-1], timestamp),
                 cpu_table + '.' + table_name + '.CPUUsedAfter {0} {1}'.format(str(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['CPU Used After'])[:-1], timestamp)]
        message = '\n'.join(lines) + '\n'
        logger.info(message)
    return message

def build_disk_data(timestamp, table_name=None):
    """
    Building CPU Data
    """
    disk_table = CONFIG["Memory_Consumption"]["GRAPHITE_DISK_Table"]
    if not table_name:
        lines = [disk_table + '.Build {0} {1}'.format(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Build'], timestamp)]
        for i in range(0, len(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Disk Usage'])):
            d, p = CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Disk Usage'][i]
            hash = hashlib.md5(d.encode())
            lines.append(disk_table + '.DiskUsage.Part{0} {1} {2}'.format(hash.hexdigest(), p[:-1], timestamp))
        message = '\n'.join(lines) + '\n'
        logger.info(message)
    else:
        lines = [disk_table + '.' + table_name + '.Build {0} {1}'.format(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Build'], timestamp)]
        for i in range(0, len(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Disk Usage'])):
            d, p = CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Disk Usage'][i]
            hash = hashlib.md5(d.encode())
            lines.append(disk_table + '.' + table_name + '.DiskUsage.Part{0} {1} {2}'.format(hash.hexdigest(), p[:-1], timestamp))
        message = '\n'.join(lines) + '\n'
        logger.info(message)
    return message

def build_io_data(timestamp, table_name=None):
    """
    Building CPU Data
    """
    disk_table = CONFIG["Memory_Consumption"]["GRAPHITE_IO_Table"]
    if not table_name:
        lines = [disk_table + '.Build {0} {1}'.format(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Build'], timestamp)]
        for i in range(0, len(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['IO'])):
            d, tps, rps, wps, r, w = CONFIG["Memory_Consumption"]["FILE_COLUMN"]['IO'][i]
            lines.append(disk_table + '.IO.Write.{0} {1} {2}'.format(d, w, timestamp))
            lines.append(disk_table + '.IO.Read.{0} {1} {2}'.format(d, r, timestamp))
        message = '\n'.join(lines) + '\n'
        logger.info(message)
    else:
        lines = [disk_table + '.' + table_name + '.Build {0} {1}'.format(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Build'], timestamp)]
        for i in range(0, len(CONFIG["Memory_Consumption"]["FILE_COLUMN"]['IO'])):
            d, tps, rps, wps, r, w = CONFIG["Memory_Consumption"]["FILE_COLUMN"]['IO'][i]
            lines.append(disk_table + '.' + table_name + '.IO.Write.{0} {1} {2}'.format(d, w, timestamp))
            lines.append(disk_table + '.' + table_name + '.IO.Read.{0} {1} {2}'.format(d, r, timestamp))
        message = '\n'.join(lines) + '\n'
        logger.info(message)
    return message

def build_event_data(timestamp, table_name=None):
    """
    Building Event Data to push Build Version
    """
    if not table_name:
        eventpara = {'what': 'Version Number', 'tags': 'ECO2VersionNumber',
                     'data': CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Build'],
                     'when': timestamp}
        logger.info(eventpara)
    else:
        eventpara = {'what': 'Version Number',
                     'tags': 'ECO2StressTestVersionNumber',
                     'data': CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Build'],
                     'when': timestamp}
        logger.info(eventpara)
    return eventpara


def build_process_data(timestamp, table_name=None):
    """
    Building Process data to process Top 10 Processes
    """
    if table_name is "NumberGraphicStress":
        process_table = CONFIG["Memory_Consumption"]["GRAPHITE_WEBKITBROWSER_CSS_GRAPHIC_STRESS_PROCESS_Table"]
    else:
        process_table = CONFIG["Memory_Consumption"]["GRAPHITE_PROCESS_Table"]

    global process_details

    for processname in process_details:
        process_details[processname] = int(int(process_details[processname].split(':')[0]) / int(process_details[processname].split(':')[1]))

    a1_sorted_keys = sorted(process_details, key=process_details.get, reverse=True)
    i = 0
    for a1_sorted_key in a1_sorted_keys:
        if i >= 10:
            break
        if i == 0:
            lines = [process_table + '.' + a1_sorted_key +' {0} {1}'.format(str(process_details[a1_sorted_key]), timestamp)]
        else:
            lines.append(process_table + '.' + a1_sorted_key +' {0} {1}'.format(str(process_details[a1_sorted_key]), timestamp))
        i = i + 1
    logger.debug(lines)
    message = '\n'.join(lines) + '\n'
    logger.info(message)
    return message


def memory_capture(device, when, run_time=1):
    """
    Memory Capture Data Before and After
    """
    if when == "before":
        get_memory_details(device, "Mem Used Before", "Mem Free Before", "CPU Used Before", run_time)
    elif when == "after":
        get_memory_details(device, "Mem Used After", "Mem Free After", "CPU Used After", run_time)
        time.sleep(CONFIG["Memory_Consumption"]["trailing_delay"])
    assert True, "Memory Consumption has been captured."
    # TODO: Insert restore factory default Code here


def getDiskUsage(adb_obj):
    """
    Get Disk space information via df -h command and fetch CPU % via regular experssion.
    """
    try:
        resp = adb_obj.executeCommand("df -h")
        print "Disk Usage :" + resp
        if resp:
            resp = re.sub(' +', ' ', resp)
            resp = re.findall(r'([^\s]+)', resp)
            print "Disk Usage rootfs Usage:" + resp[0]
            ds = []
            for v in range(7, len(resp), 6):
                ds.append((resp[v] + " -> " + resp[v + 5], resp[v + 4]))
            return ds
    except Exception as e:
        print "getRootfsUsage: Get Root fs Unsuccessful - %s" % str(e)
        assert False, "getRootfsUsage: Get Root fs Unsuccessful - %s" % str(e)


def getIOWrite(adb_obj):
    """
    Gets IO statistics
    """
    try:
        resp = adb_obj.executeCommand("iostat -d -k")
        print "iostat :" + resp
        if resp:
            resp = re.sub(' +', ' ', resp)
            resp = re.findall(r'([^\s]+)', resp)
            ds = []
            for v in range(13, len(resp), 6):
                ds.append((resp[v], resp[v + 1], resp[v + 2], resp[v + 3], resp[v + 4], resp[v + 5]))
            return ds
    except Exception as e:
        print "getIOWrite: Get iostat Unsuccessful - %s" % str(e)
        assert False, "getIOWrite: Get iostat Unsuccessful - %s" % str(e)


def check_is_data_push():
    """
    Determines if we are going to push data to the server.

    :return: None
    """
    global IsDataPush

    IsDataPush = True
    now = datetime.now()
    now_time = now.time()
    if now_time >= dttime(00, 00) and now_time <= dttime(06, 30):
        IsDataPush = True
        logger.info("Is DataPush is True")
    else:
        IsDataPush = False
    logger.info("Extra Parameter IsMandatoryDataPush : " + str(IsMandatoryDataPush))


def comman_line_resources(request):
    """
    Determines the command line items for the pytest run.

    :param request: Pytest request
    :return: None
    """
    global BRANCH
    global IsMandatoryDataPush
    global IsDataPush
    global run_time
    BRANCH = request.config.getoption("--branch")
    IsMandatoryDataPush = request.config.getoption("--IsMandatoryDataPush")
    run_time = request.config.getoption("--test-duration")

def press_key(device_ip, key_id, press_time):
    """
    :param deviceip: Device IP of Professor.
    :param keyid: Key Id of Professor
    :param presstime: time in ms.

    - Common function to press key on Professor
    """
    tap = pexpect.spawn("telnet %s 17000" % device_ip)
    keypress.press_key(tap, key_id, press_time)

def get_preset_stored(product_settings):
    """
    Gets the presets ID numbers stored in the product settings dictionary.

    :param product_settings: The dictionary object representing the product settings
    :return: List of preset numbers
    """
    presets = []
    try:
        for preset_item in product_settings['ProductSettings']['presets']['presets']:
            presets.append(preset_item)
    except KeyError:
        # If we can't find anything, just return empty list, don't Raise
        logger.warning('Not able to find presets in %s', product_settings)
    finally:
        return presets

def store_presets(device_ip, frontdoor, riviera_utils):
    """
    store_presets stores the same Tunein station on all presets
    param device_ip: Device_ip fixture returns IP address 
    param frontdoor: A frontdoor device fixture
    param riviera_utils: riviera_utils fixture returns RivieraUtils object
    """
    logger.info("store_presets")
    # We want to make sure the Key Press store is properly stored
    DELAY_AFTER_PRESET_SET = 10

    data = CONFIG["Source_Data"]
    frontdoor.sendPlaybackRequest(data)

    time.sleep(3)

    now_playing_list = frontdoor.getNowPlaying()
    assert now_playing_list["body"]["state"]["status"].lower() == "play"

    preset_key_start = Keys.PRESET_1.value
    preset_key_end = Keys.PRESET_6.value
    preset_range = lambda start, end: range(preset_key_start, preset_key_end+1)
    for preset_key in preset_range(preset_key_start, preset_key_end):
        press_key(device_ip, preset_key, 5000)

    # We want to ensure the Key Press was received and stored in local file store.
    time.sleep(DELAY_AFTER_PRESET_SET)

    current_product_settings = riviera_utils.get_product_settings()
    presets = get_preset_stored(current_product_settings)
    assert len(presets) == 6, \
        'Incorrect number of presets in {}. Anticipated 6, found {}.'.format(current_product_settings, len(presets))
    device_off(frontdoor)


def play_stored_presets(device_ip, frontdoor, run_time):
    """
    Play TuneIn Station shuffling 1-6 presets
    param tap: A PExpect Telnet connection to the Device
    param frontdoor: A frontdoor device fixture
    param run_time: Duration of the test run
    """
    #set volume to 0
    volume_data = get_volume_content()
    frontdoor.sendVolume(volume_data)
    volume_response = frontdoor.getVolume()
    assert volume_response["body"]["value"] == 0

    # Duration to start a preset
    key_press_duration = 250

    # Run preset (1-6) with 5 mins interval
    t_end = time.time() + 60 * int(run_time)
    while time.time() < t_end:
        preset_key_start = Keys.PRESET_1.value
        preset_key_end = Keys.PRESET_6.value
        preset_range = lambda start, end: range(preset_key_start, preset_key_end+1)
        for preset_key in preset_range(preset_key_start, preset_key_end):
            press_key(device_ip, preset_key, key_press_duration)
            t_preset_checking = time.time() + 60 * 5
            while time.time() < t_preset_checking and time.time() < t_end:
                continue

    # Stop Tunein station
    device_off(frontdoor)

def device_off(frontdoor):
    """
    Turn Device OFF
    param frontdoor: A frontdoor device fixture
    """
    resp = frontdoor.stopPlaybackRequest()
    return resp

def get_volume_content():
    """
    Volume Payload set to 0
    return: Data
    """
    data = '{ "defaultOn" : 0, \
               "max" : 0, \
               "min" : 0,\
               "muted" : false, \
               "value" : 0 }'
    return data


@pytest.mark.usefixtures("frontdoor", "perform_cloud_sync", "device_ip", "riviera_utils")
def test_iheartradio_memory_consumption(frontdoor, perform_cloud_sync, device_ip, riviera_utils, request):
    """
    Perform Bonjour Update on Jenkins Level
    This functions calls memory_capture(before) method to Capture Memory Consumption
    After that it plays Tune in with different presets (1-6 every 5 mins) station using playTuneInMusic and it calls
    memory_capture (after) method to get CPU / Memory Usage After
    Writes the data to a csv file. Sends an email, Device is off, Push the data
    to Grafana
    
    param frontdoor: Returns frontdoor object
    param perform_cloud_sync: perform_cloud_sync fixture
    param: device_ip: device_ip fixture returns device ip of the device
    param: riviera_utils: riviera_utils fixture returns the riviera utility object
    param: pkill_webkit_process: pkill_webkit_process fixture to kill existing webkit process if any 
    Setup two Devices one as Master and other as Slave to create Multiroom scenario to capture Memory Consumption - TBD
    TODO:
    1. Add Multi-Room setup
    2. Add logic to factoryDefault system & call in memory_capture_after
    """
    try:
        device = request.config.getoption("--device")
        memoryInfo = MemoryInfo(device)

        logger.info("--- Calling test_tunein_memory_consumption ---")
        comman_line_resources(request)
        check_is_data_push()

        logger.info(" -------- Calling memory_capture_before --------")

        # before, collect root fs usage info, write and flush to new file
        ds = getDiskUsage(memoryInfo.adb)
        i = 0
        for d, p in ds:
            CONFIG["Memory_Consumption"]["FILE_COLUMN"]["Disk Usage"].append((d, p))
            i += 1
        CONFIG['Memory_Consumption']['FILE_COLUMN']['Build'] = get_build_version(riviera_utils, request)
        logger.info(" ------------ Build Version ---------- " + CONFIG["Memory_Consumption"]["FILE_COLUMN"]['Build'])

        beforeIO = getIOWrite(memoryInfo.adb)

        # capture stats before the device is used
        memory_capture(device, "before", run_time)

        # Store Presets before playing presets
        store_presets(device_ip, frontdoor, riviera_utils)

        # Created two Separate Threads to capture Memory Info along with Music Play
        logger.info(" -------- Starting music_play_process -------- ")
        music_play_process = threading.Thread(target=play_stored_presets, args=(device_ip, frontdoor, run_time, ))
        music_play_process.daemon = True
        music_play_process.start()
        logger.info(" -------- Starting memory_capture_process After -------- ")
        memory_capture_process = threading.Thread(target=memory_capture, args=(device, "after", run_time, ))
        memory_capture_process.daemon = True
        memory_capture_process.start()
        music_play_process.join()
        memory_capture_process.join()

        afterIO = getIOWrite(memoryInfo.adb)

        for i in range(0, len(beforeIO)):
            d1, tps1, rps1, wps1, r1, w1 = beforeIO[i]
            d2, tps2, rps2, wps2, r2, w2 = afterIO[i]
            CONFIG["Memory_Consumption"]["FILE_COLUMN"]['IO'].append((d1, (float(tps1) + float(tps2)) / 2,
                (float(rps1) + float(rps2)) / 2, (float(wps1) + float(wps2)) / 2, int(r2), int(w2)))

        # after capture, collect results and send emails, send to grafana
        logger.info(" -------- After Completed into CSV -------- ")
        write_csv_file()
        logger.info(" -------- CSV File Write Completed -------- ")
        send_mail_from_files()
        logger.info(" -------- Turn Device Off -------- ")
        device_off(frontdoor)
        if IsDataPush or IsMandatoryDataPush == 'True':
            push_data_to_grafana(BRANCH, table_name=None)

    except Exception as exception:
        error_string = "test_tunein_memory_consumption: Memory Consumption Capture Unsuccessful - %s" % str(exception)
        logger.error(error_string)
        assert False, error_string
