"""
Configuration file for Diagnostics Page
"""
CONFIG = {
    'diagnostics_diag'             : "diag",
    'fs_version_command'           : "cat /opt/Bose/etc/BoseVersion.json",
    'mfg_data_command'             : "cat /persist/mfg_data.json",
    'expected_text'                  : {
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
