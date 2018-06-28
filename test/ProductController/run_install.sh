# Set up CastleTestUtils
cd /scratch/CastleTestUtils/
make vsetup
source ./venv/bin/activate
make
pip install dist/CastleTestUtils-0.1.7.tar.gz

# Run first test
#pytest -sv tests/test_target_frontdoor.py --device 8857f8d

# Run all tests
#make test device=8857f8d inf=eth0

# Now run your own tests if all passed