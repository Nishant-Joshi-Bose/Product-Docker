Product Controller Testing
==========================


Background
-----------------------------------------------------------

Based off of the CastleTestUtils testing framework, these tests exist to simplify testing of the Product Controller.

NOTE: Do not include venv or pycache folder in git as they are generated every time the virtual environment is created.


Running Tests
----------------------

Like CastleTestUtils, these tests are run in a python virtual environment. The process is outlines below. Bash files are included for convenience. See the CastleTestUtils documentation for more information, as there are several other ways to execute these tests that are specified there.

1. Ensure that you have CastleTestUtils copied to your local invironment.
```
cd /scratch
git clone git@github.com:BoseCorp/CastleTools.git
```
2. Create the virtual environment.
```
virtualenv -p /usr/bin/python ./venv
source ./venv/bin/activate
pip install -r requirements.txt
```

3. Execute tests. Note that parameters need to be specified after each test. See Pytest documentation for other ways to run tests. Below is an example for running one test with its required parameters. (Change device ID)
```
pytest -sv AudioSettings/test_audio_settings.py --device-id 8857f8d --network-iface eth0 --target device
```

4. Deactivate virtual environment.
```
deactivate
```

Installation
-----------------------

You may need to run this code before you can use these tests.

1. Create the virtual environment and install the lateset version of CastleTestUtils. (Check latest version)
```
cd /scratch/CastleTestUtils/
make vsetup
source ./venv/bin/activate
make
pip install dist/CastleTestUtils-0.1.8.tar.gz
```

2. To test if a specific CastleTestUtils script is working properly, run this in the virtual environment. (Change device ID)
```
pytest -sv tests/test_target_frontdoor.py --device 8857f8d
```

3. To all CastleTestUtils scripts, run this in the virtual environment. (Change device ID)
```
make test device=8857f8d inf=eth0
```