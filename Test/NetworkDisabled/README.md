# Eddie NetworkDisabled feature

## Synopsis
This test module will test network disabled feature and ensure that it works correctly on the Eddie product.


### Installation

Initial setup for the repository is as follows:
```bash
# Clone the respository
git clone git@github.com:BoseCorp/Eddie.git
cd Eddie
# Get the correct components
make generated_components
```
All steps assume your are in the directory of your Eddie repository (`$EDDIE_DIR`).

Next, we need to setup the Python environment to use.
```bash
cd $EDDIE_DIR/Test
# Setup your virtual environment
virutalenv -p /usr/bin/python2 .venv
$EDDIE_DIR/Test/.venv/bin/activate
# Install current Python requirements
pip install requirements.txt
```

Running the tests requires configuration information that should be known prior to startup. We should know below params,
- Communication type to the product (`ADB`)
- LPM debug port path (`/dev/ttyUSB?`).
- Router used from "Configs/conf_wifiProfiles.ini" (`--router=bproduct`)

```bash
cd $EDDIE_DIR/Test/NetworkDisabled
pytest --junit=network_disabled.xml --target=device --device-id=<ADB device id> --lpm-port=<lpm device port> --router=bproduct
```
This will run all `test_network_disabled.py` inside the `NetworkDisabled` folder and generate JUnit style output to
`network_disabled.xml`.

### Test Descriptions
Test suite "test_network_disabled" can be found in the `NetworkDisabled` test module:
- `test_network_disabled_success`
- `test_network_disabled_playing_state`
- `test_network_disabled_lps_from_playable_state[Idle]`
- `test_network_disabled_lps_from_playable_state[NetworkStandby]`
- `test_network_disabled_lps_from_setup_state`


### List of Files under Test/NetworkDisabled :


conftest.py                     = Contains fixtures used in network disabled test script (test_network_disabled.py)

test_network_disabled.py        = Automated script for network disabled feature

README.md                       = This Readme file
