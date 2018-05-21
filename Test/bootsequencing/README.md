# Eddie Bootup Sequence Timing Test

## Synopsis
This test will cover some basics tests to ensure that we can properly reboot the device into a good running state.

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
Running the tests requires configuration information that should be known prior to startup. We should know below params,
- `ADB` Device Identifiers
- Router used from "Configs/conf_wifiProfiles.ini" (`--router=bproduct`)

```bash
cd $EDDIE_DIR/Test/bootsequencing
python -B -m pytest \
       --junit=boot.xml \
       --verbose \
       --device-id=<ADB device id> \
       --router=bproduct \
       test_boot_sequence_timing.py
```
This will run all `test_boot_sequence_timing.py` inside the `bootsequencing` folder and generate JUnit style output to `boot.xml`.

## Test Descriptions
Test suite "test_boot_sequence_timinig.py" can be found in the `bootsequencing` test module:
- `test_base_reboot_time`
- `test_network_connection_time`
- `test_boot_state_time`


## List of Files under Test/bootsequencing :
`conftest.py` = Fixtures used by Boot Sequencing

`test_boot_sequence_timing.py` = Automated script for boot timing

`stateutils.py` = Extra utilies used with these tests

`README.md` = This Readme file
