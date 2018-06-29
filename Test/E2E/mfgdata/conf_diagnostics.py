"""
Configuration file for Diagnostics Page
"""
CONFIG = {
    'diagnostics_diag'             : "diag",
    'fs_version_command'           : "cat /opt/Bose/etc/BoseVersion.json",
    'lpm_version_command'           : "lpm pt vr",
    'network_interfaces_list_cmd'  : "ifconfig | sed 's/[ \t].*//;/^$/d'",
    'mfg_data_command'             : "cat /persist/mfg_data.json",
    'expected_text'                  : {
        'rndis1_ip_address'          : "203.0.113.1",
        'main_header'             : "Diagnostics",
        'software_version_text'   : "Software Version",
        'lpm_version_text'        : "LPM Versions",
        'expected_hash_text'      : "development or production",
        'diagnostics_page_title'  : "Diagnostics",},
    'locator'                  : {
        'manufacturing_data_locator'             : "//li[contains(text(), 'Manufacturing Data')]",
        'partial_link_locator'   : "all",
        'manufacturing_data_all' : "//pre",
        'software_version_header': 'body > li:nth-child(3)',
        'lpm_version_header': 'body > li:nth-child(4)',
        'lpm_versions_locator': "body > li:nth-child(4) > div > table",
        'mfg_data_locator': "body > li:nth-child(5) > div > table",
        'diagnostics_header' : 'body > h1',
        'display_link_element' : "Display",
        'done_button': "Done",
        'left_display_test' : 'td.showimg',}
}
