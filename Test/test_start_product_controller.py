"""
Automated test to check the boot-status of the virtual speaker
"""

import pytest
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
LOGGER = get_logger(__name__)

@pytest.mark.usefixtures("start_mock_frontDoor")
@pytest.mark.usefixtures("eddie_product_controller")
def test_boot_status(eddie_product_controller):
    """
    1. Start the related mocks.
       mock FrontDoor
       mock CLIServer
    2. Start the Product Controller - Eddie.
    """
    LOGGER.info("Getting the boot status")
