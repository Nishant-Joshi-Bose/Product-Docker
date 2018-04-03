import pytest

def pytest_addoption(parser):
    """
    Command line options for the pytest tests in this module.

    :param parser: Parser used for method.
    :return: None
    """
    parser.addoption('--zipfile', action='store', type=str, required=True, help='Path to Zip file needed for Bonjourupdate')
    parser.addoption('--updatecnt', action='store', type=int, default=2, help='Set count to perform Bonjour Update that many times')
