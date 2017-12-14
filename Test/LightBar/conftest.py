import pytest
from configure import conf

def pytest_generate_tests(metafunc):
    # lpm port for LPM logs
    conf["LPMport"] = pytest.config.getoption("--lpm_port")
