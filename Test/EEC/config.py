''' Configuration file for the testcases.'''
CONFIG = {
    'network_interfaces_list_cmd'  : "ifconfig | sed 's/[ \t].*//;/^$/d'",
    'network_standby_timeout' : 65, # seconds until device standby
    'low_power_timeout' : 120, # seconds until device setup 2 * 60
}
