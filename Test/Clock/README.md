# Eddie Timezone Tests

## Synopsis
This test module will cover all Timezone related tests to ensure that every timezone works correctly on the Eddie product. 


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
We will need pytz python module for executions
pip install --requirement requirements.txt
```

## Execution of Tests
Running the tests requires configuration information that should be known prior to startup. We should know the 
communication type to the product (`ADB`).

```bash
cd $EDDIE_DIR/Test/Timezone
pytest -vs --junit=timezone.xml \
       --target=device \
       --device-id=<adb-device-id>
       test_timzone.py
```
This will run all `test_*` inside the `Timezone` folder and generate JUnit style output to 
`timezone.xml`. 
