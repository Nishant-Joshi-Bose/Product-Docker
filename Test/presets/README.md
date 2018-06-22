# Eddie Presets

## Synopsis
This test module will cover all available checks to ensure that the preset storage and playback works correctly on the
Eddie product. Tests are meant to be run during release build pipelines and when


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

Running the tests requires configuration information that should be known prior to startup. We should know the
device ID and router that the device will be connecting to.
```bash
cd $EDDIE_DIR/Test/presets
python -B -m pytest \
       --verbose \
       --junit=presets.xml \
       --device-id <ADB Device ID> \
       --router <Router ID>
```
This will run all `test_*` inside the `preset` folder and generate JUnit style output to `presets.xml`.

### Test Descriptions
Two test suites can be found in the `presets` test module:
- `test_preset_storage.py`
- `test_preset_playback.py`

`test_preset_storage.py` contains tests related to storage various preset values and ensuring they are stored properly.
The following self descriptive test names can be found in the suite:
- `test_store_same_all_presets`
- `test_store_random_station_on_all_keys`
- `test_storing_persists_after_reboot`
- `test_overwrite_existing_presets`

`test_preset_playback.py` contains tests related to the playback of various presets on the Eddie product. The following
self descriptive test names can be found in the suite:
- `test_single_stored_preset_playback`
- `test_single_provider_multiple_stations_playback`
- `test_play_preset_after_power_cycle`
- `test_play_empty_preset_with_nothing_playing`
- `test_play_empty_preset_with_music_playing`

Details and step-by-step procedures for each of these tests can be found in TestRail in the
[Jenkins Eddie Automation Test Suite](https://boseqa.testrail.net/index.php?/suites/view/973) under Presets.

#### System Configuration
Currently we have to assume the Eddie product that will be communicated with will have the following configuration:
- An ADB connection to the Eddie product through the debug port
- The Eddie product has been activated (*MADI* or *SADI*)
- The Eddie product has had its manufacturing data set
- The account has multiple music service accounts available; see `services.py`
