"""
Configuration file for Memory Consumption tests
"""

CONFIG = {
	   #FILE_NAME : Need to change path based on machine, You can create folder any where and change path here
    'Memory_Consumption': {
        'trailing_delay'         : 5,
        'FILE_NAME'              : 'MemoryConsumptionDetails.csv',
        'GRAPHITE_Memory_Table'  : "ProfessorMemoryConsumptionECO2",
        'GRAPHITE_CPU_Table'     : "ProfessorCPUUsageECO2",
        'GRAPHITE_DISK_Table'     : "ProfessorDiskECO2",
        'GRAPHITE_IO_Table'     : "ProfessorIOECO2",
        'GRAPHITE_PROCESS_Table' : "ProfessorProcessECO2",
        'GRAPHITE_WEBKITBROWSER_CSS_GRAPHIC_STRESS_PROCESS_Table' : "ProcessNumberGraphicStressECO2",
        'IsMandatoryDataPush'    : False,
        'FILE_COLUMN'            : {
            'Build'           : '0.0.0.0',
            'Mem Used Before' : 0,
            'Mem Free Before' : 0,
            'CPU Used Before'  : 0.0,
            'Disk Usage': [],
            'IO': [],
            'Mem Used After'   : 0,
            'Mem Free After'   : 0,
            'CPU Used After'   : 0.0}},

    # DEVICE & EMAIL PARAMETERS
    'Device_Param': {
        'SEND_FROM'     : 'ProfessorMemTest@bose.com',
        # Add more emails by putting a ";" then the address in the string
        'SEND_TO'       : "lia_esper@bose.com; barry_clark@bose.com",
        'MAIL_SERVER'   : 'smtp.bose.com',
        'DEVICE_IP'     : '203.0.113.1',
        'DEVICE_PORT'   : '23',
        'USER_NAME'     : "root",
        'HTTP_PORT'     : 8090,
        'CARBON_SERVER' : 'ssg-dashboard.bose.com',
        'CARBON_PORT'   : 2003,
        'CARBON_HTTP_PORT' : 81},

    #MultiRoom Setup
    'Devices': {
        'MASTER_MAC_ID' : '544A16D43733',
        'MASTER_DEVICE_IP' : '192.168.1.49',
        'SLAVE_MAC_ID' : 'D05FB8A845E4',
        'SLAVE_DEVICE_IP' : '192.168.1.50'},

    #Build Version Path
    'Build_Version': {
        'PATH' : '/opt/Bose'},
    
    #webkit command
    'WEBKIT': {
        'CMD1': '/opt/Bose/webkit/target_scripts/lcd/css-3d-number-rotation.html',
        'CMD2' : 'pkill -f /opt/Bose/webkit/target_scripts/lcd/css-3d-number-rotation.html; echo $?',
        'CMD3' : "./run_wpe.sh TEST5 0.5 PORTRAIT'"
    },

    #Madrid Credentials
    'Gigya_Account': {
        'Email': 'a4vauto_testing@bose.com',
        'Password': '02bosebose'
    },

    #Playback source Information
    'Source_Data': '{"source":"IHEART",\
                    "sourceAccount":"bfb895f5-67bd-446d-bcd3-1900eb715621",\
                    "preset":{"type":"stationurl",\
                                "location":"/playback/containerType/live/containerId/1097/containerName/KISS 108",\
                                "name":"KISS 108",\
                                "presetable":"true",\
                                "containerArt":"http://i.iheart.com/v3/re/assets/images/1097.png"},\
                    "playbackLocation":{"type":"stationurl",\
                                "location":"/playback/containerType/live/containerId/1097/containerName/KISS 108",\
                                "name":"KISS 108",\
                                "presetable":"true",\
                                "containerArt":"http://i.iheart.com/v3/re/assets/images/1097.png"}}'

}
