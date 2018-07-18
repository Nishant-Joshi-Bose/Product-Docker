# Eddie Manufacturing Tests

A suite of tests to ensure proper functionality of the Manufacturing Service Page.

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

## Running the Tests

We need to assume that your device is available to the host computer and there is a wireless network to connect to.

```bash
cd $EDDIE_DIR/Test/manufacturing

python -B -m pytest \
       --verbose \
       --capture=no \
       --device-id=<ADB Device ID> \
       --router=<WiFi Configuration Name> \
       test_*.py
```
