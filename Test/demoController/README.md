# Eddie Demo Controller Tests

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

```bash
cd demoController
pytest test_demo.py -vs \
                    --target=device \
                    --network-iface='eth0' \
                    --device-id=61b5457
```
