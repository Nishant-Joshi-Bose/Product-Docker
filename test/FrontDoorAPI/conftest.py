"""
Conftest.py for FrontDoorAPI folder

"""


def pytest_addoption(parser):
    """ Adding Command line options """
    parser.addoption("--ip-address",
                     action="store",
                     default=None,
                     help="IP Address of Target under test")
