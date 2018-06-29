# Eddie Language Tests

## Synopsis

This test module will cover all Language related tests to ensure that we are able to set every Language correctly on the Eddie product.

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
communication type to the product (`ADB`).

```bash
cd $EDDIE_DIR/Test/Language
pytest -vs --junit=language.xml \
       --device-id=<adb-device-id>
       --router <network router>
       test_language.py
```

This will run all `test_*` inside the `Language` folder and generate JUnit style output to
`language.xml`.
