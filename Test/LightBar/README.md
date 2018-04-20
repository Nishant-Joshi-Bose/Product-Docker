# Eddie LightBar Tests

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

# Running Tests
## Commands to perform the lightbar animation

```shell session
$ pytest -vs test_lightbar_scenario.py \
             --target=device \
             --network-iface eth0 \
             --lpm-port <lpm port of device>
```

## Input information of Lightbar Animation
```shell session
nextValue: Animation pattern names
transition: transition value of animation pattern
repeat: Value for animation pattern to be repeated or not
data: Animation pattern data
expected Output: Animation pattern expected Output
```
