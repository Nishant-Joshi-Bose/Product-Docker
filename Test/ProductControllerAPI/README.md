# Eddie Product Controller API Tests

## Synopsis
This test module will cover all ProductController APIs to ensure that API works correctly on the Eddie product. 


## Setting Up Test Environment

Clone the Eddie Repository:
```bash
# Clone the respository
git clone git@github.com:BoseCorp/Eddie.git
cd Eddie
```
All steps assume your are in the directory of your Eddie repository (`$EDDIE_DIR`).

Generate a Virtual Environment and install used packages.
```bash
cd $EDDIE_DIR/Test

# Setup your virtual environment
virutalenv -p /usr/bin/python2.7 .venv
source $EDDIE_DIR/Test/.venv/bin/activate

# Install current Python requirements
pip install --requirement requirements.txt
```

## Execution of Tests
Running the tests requires configuration information that should be known prior to startup. We should know the 
communication type to the product (`ADB` and `wlan0`).

```bash
cd $EDDIE_DIR/Test/ProductControllerAPI
pytest --junit=product_controller_apis.xml \
       --target=device \
       --device-id=<adb-device-id> \
       --network-iface=wlan0 \
       --router=<router-id from Test/Configs/conf_wifiProfiles.ini>
```
This will run all `test_*` inside the `ProductControllerAPI` folder and generate JUnit style output to 
`product_controller_apis.xml`. 

### Test Descriptions
Many test suites can be found in the `ProductControllerAPI` test module:
- `test_system_info.py`
- `test_system_state.py`
- `test_system_power_control.py`
- `test_system_setup.py`
- `test_system_power_timeouts.py`

`test_system_info.py` contains tests related to system info API and ensuring API works from different device states. 
The following self descriptive test names can be found in the suite:
- `test_system_info_setup_state`
- `test_system_info_playing_from_amazon`
- `test_system_info_playing_from_aux`

`test_system_state.py` contains tests related to system state API and ensuring API works from different device states.
The following self descriptive test names can be found in the suite:
- `test_system_state_playing_from_amazon`
- `test_system_state_playing_from_aux`
- `test_system_state_network_standby`
- `test_system_state_factory_default`
- `test_system_state_setup_state`

`test_system_info.py` contains tests related to system info API and ensuring API works from different device states.
The following self descriptive test names can be found in the suite:
- `test_system_power_control_error`
- `test_system_power_control_aux_play`
- `test_system_power_control_sts_playing`
- `test_system_power_control_notification_in_aux`
- `test_system_power_control_notification_in_sts`

`test_system_setup.py` contains tests related to system setup API and ensuring API works from different device states.
The following self descriptive test names can be found in the suite:
- `test_system_setup_from_setup_state`
- `test_system_setup_from_selected_state`
- `test_system_setup_from_idle_state`
- `test_system_setup_cli_command`
- `test_system_setup_errors`

`test_system_power_timeouts.py` contains tests related to system power timeouts API and ensuring API works from different device states.
The following self descriptive test names can be found in the suite:
- `test_system_power_timeouts_errors`
- `test_system_power_timeouts_from_setup_state`
- `test_system_power_timeouts_from_selected_state`
- `test_system_power_timeouts_from_idle_state`
- `test_system_power_timeouts_behaviour`
