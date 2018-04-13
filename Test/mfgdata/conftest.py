# conftest.py
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:  COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
#              This program may not be reproduced, in whole or in part in any
#              form or any means whatsoever without the written permission of:
#                  BOSE CORPORATION
#                  The Mountain,
#                  Framingham, MA 01701-9168
#
"""
Conftest.py for Diagnostics
"""
import pytest

@pytest.fixture
def chrome_options(chrome_options):
    """
    This fixture would enable headless for all Chrome instances
    """
    chrome_options.add_argument('headless')
    return chrome_options
