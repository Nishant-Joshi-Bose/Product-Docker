"""
Configuration file for Diagnostics Page
"""
CONFIG = {
    'diagnostics_diag'             : "diag",
    'fs_version_command'           : "cat /opt/Bose/etc/BoseVersion.json",
    'network_interfaces_list_cmd'  : "ifconfig | sed 's/[ \t].*//;/^$/d'",
    'mfg_data_command'             : "cat /persist/mfg_data.json",
    'expected_text'                  : {
        'usb_ip_address'          : "203.0.113.1",
        'main_header'             : "Diagnostics",
        'software_version_text'   : "Software Version",
        'diagnostics_page_title'  : "Diagnostics",},
    'locator'                  : {
        'manufacturing_data_locator'             : "//li[contains(text(), 'Manufacturing Data')]",
        'partial_link_locator'   : "all",
        'manufacturing_data_all' : "//pre",
        'software_version_header': 'body > li:nth-child(3)',
        'table_locator': "table :first-of-type",
        'diagnostics_header' : 'body > h1',
        'left_display_test' : 'td.showimg',}
}
