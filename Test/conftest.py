
def pytest_addoption(parser):
    parser.addoption("--ip-address", 
		      action="store", 
		      default=None,
                      help="IP Address of Target under test")
