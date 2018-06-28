#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:     COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
#                 This program may not be reproduced, in whole or in part in any
#                 form or any means whatsoever without the written permission of:
#                     BOSE CORPORATION
#                     The Mountain,
#                     Framingham, MA 01701-9168
#
"""
    This is the test script file which contains audio settings tests.
"""

import json
import pytest
import time

from AudioSettingsBase import *


"""
	Test Structure:

		TEST 1: Verify that GLOBAL remains in place when source is changed
		1. Set source to Bluetooth
		2. Set to GLOBAL with value of 10
		3. Change source to TV
		4. Verify, should be GLOBAL with value of 10

		TEST 2: Verify that if in SESSION, previous persistance is set after source change
		5. Set to SESSION with value of 20
		6. Change source to Bluetooth
		7. Verify, should be GLOBAL with value of 10

		TEST 3: Verify that CONTENT_ITEM settings remain after source change
		8. Change source to TV
		9. Set to CONTENT_ITEM with a value of 30 (PRODUCT_TV)
		10. Change source to Bluetooth
		11. Set to CONTENT_ITEM with a value of 40 (BLUETOOTH_)
		12. Change source to TV
		13. Verify, should be CONTENT_ITEM with value of 30 (PRODUCT_TV)
		14. Change source to Bluetooth
		15. Verify, should be CONTENT_ITEM with value of 40 (BLUETOOTH_)
"""

# surround
@pytest.mark.usefixtures("frontDoor_conn")
def test_audio_bass(frontDoor_conn):
	"""
	Test Objective:
		Verify that the bass behaves properly for persistence and source changes
	"""

	######################### TEST 1 #########################
	# Verify that GLOBAL remains in place when source is changed

	setSource(frontDoor_conn,"BLUETOOTH","")
	set_bass(frontDoor_conn, "GLOBAL", 10)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_bass(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 2 #########################
	# Verify that if in SESSION, previous persistance is set after source change

	set_bass(frontDoor_conn, "SESSION", 20)
	setSource(frontDoor_conn,"BLUETOOTH","")
	verify_bass(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 3 #########################
	# Verify that CONTENT_ITEM settings remain after source change

	setSource(frontDoor_conn,"PRODUCT","TV")
	set_bass(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"BLUETOOTH","")
	set_bass(frontDoor_conn, "CONTENT_ITEM", 40)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_bass(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"BLUETOOTH","")
	verify_bass(frontDoor_conn, "CONTENT_ITEM", 40)

# treble
@pytest.mark.usefixtures("frontDoor_conn")
def test_audio_treble(frontDoor_conn):
	"""
	Test Objective:
		Verify that the treble behaves properly for persistence and source changes
	"""
	
	######################### TEST 1 #########################
	# Verify that GLOBAL remains in place when source is changed

	setSource(frontDoor_conn,"BLUETOOTH","")
	set_treble(frontDoor_conn, "GLOBAL", 10)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_treble(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 2 #########################
	# Verify that if in SESSION, previous persistance is set after source change

	set_treble(frontDoor_conn, "SESSION", 20)
	setSource(frontDoor_conn,"BLUETOOTH","")
	verify_treble(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 3 #########################
	# Verify that CONTENT_ITEM settings remain after source change

	setSource(frontDoor_conn,"PRODUCT","TV")
	set_treble(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"BLUETOOTH","")
	set_treble(frontDoor_conn, "CONTENT_ITEM", 40)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_treble(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"BLUETOOTH","")
	verify_treble(frontDoor_conn, "CONTENT_ITEM", 40)

# center
@pytest.mark.usefixtures("frontDoor_conn")
def test_audio_center(frontDoor_conn):
	"""
	Test Objective:
		Verify that the center behaves properly for persistence and source changes
	"""
	
	######################### TEST 1 #########################
	# Verify that GLOBAL remains in place when source is changed

	setSource(frontDoor_conn,"BLUETOOTH","")
	set_center(frontDoor_conn, "GLOBAL", 10)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_center(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 2 #########################
	# Verify that if in SESSION, previous persistance is set after source change

	set_center(frontDoor_conn, "SESSION", 20)
	setSource(frontDoor_conn,"BLUETOOTH","")
	verify_center(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 3 #########################
	# Verify that CONTENT_ITEM settings remain after source change

	setSource(frontDoor_conn,"PRODUCT","TV")
	set_center(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"BLUETOOTH","")
	set_center(frontDoor_conn, "CONTENT_ITEM", 40)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_center(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"BLUETOOTH","")
	verify_center(frontDoor_conn, "CONTENT_ITEM", 40)

# surround
@pytest.mark.usefixtures("frontDoor_conn")
def test_audio_surround(frontDoor_conn):
	"""
	Test Objective:
		Verify that the surround behaves properly for persistence and source changes
	"""
	
	######################### TEST 1 #########################
	# Verify that GLOBAL remains in place when source is changed

	setSource(frontDoor_conn,"BLUETOOTH","")
	set_surround(frontDoor_conn, "GLOBAL", 10)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_surround(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 2 #########################
	# Verify that if in SESSION, previous persistance is set after source change

	set_surround(frontDoor_conn, "SESSION", 20)
	setSource(frontDoor_conn,"BLUETOOTH","")
	verify_surround(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 3 #########################
	# Verify that CONTENT_ITEM settings remain after source change

	setSource(frontDoor_conn,"PRODUCT","TV")
	set_surround(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"BLUETOOTH","")
	set_surround(frontDoor_conn, "CONTENT_ITEM", 40)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_surround(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"BLUETOOTH","")
	verify_surround(frontDoor_conn, "CONTENT_ITEM", 40)

# avsync
@pytest.mark.usefixtures("frontDoor_conn")
def test_audio_avsync(frontDoor_conn):
	"""
	Test Objective:
		Verify that the avsync behaves properly for persistence and source changes
	"""
	
	######################### TEST 1 #########################
	# Verify that GLOBAL remains in place when source is changed

	setSource(frontDoor_conn,"BLUETOOTH","")
	set_avsync(frontDoor_conn, "GLOBAL", 10)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_avsync(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 2 #########################
	# Verify that if in SESSION, previous persistance is set after source change

	set_avsync(frontDoor_conn, "SESSION", 20)
	setSource(frontDoor_conn,"BLUETOOTH","")
	verify_avsync(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 3 #########################
	# Verify that CONTENT_ITEM settings remain after source change

	setSource(frontDoor_conn,"PRODUCT","TV")
	set_avsync(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"BLUETOOTH","")
	set_avsync(frontDoor_conn, "CONTENT_ITEM", 40)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_avsync(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"BLUETOOTH","")
	verify_avsync(frontDoor_conn, "CONTENT_ITEM", 40)

# gainOffset
@pytest.mark.usefixtures("frontDoor_conn")
def test_audio_gainOffset(frontDoor_conn):
	"""
	Test Objective:
		Verify that the gainOffset behaves properly for persistence and source changes
	"""
	
	######################### TEST 1 #########################
	# Verify that GLOBAL remains in place when source is changed

	setSource(frontDoor_conn,"BLUETOOTH","")
	set_gainOffset(frontDoor_conn, "GLOBAL", 10)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_gainOffset(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 2 #########################
	# Verify that if in SESSION, previous persistance is set after source change

	set_gainOffset(frontDoor_conn, "SESSION", 20)
	setSource(frontDoor_conn,"BLUETOOTH","")
	verify_gainOffset(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 3 #########################
	# Verify that CONTENT_ITEM settings remain after source change

	setSource(frontDoor_conn,"PRODUCT","TV")
	set_gainOffset(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"BLUETOOTH","")
	set_gainOffset(frontDoor_conn, "CONTENT_ITEM", 40)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_gainOffset(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"BLUETOOTH","")
	verify_gainOffset(frontDoor_conn, "CONTENT_ITEM", 40)

# subWooferGain
@pytest.mark.usefixtures("frontDoor_conn")
def test_audio_subwooferGain(frontDoor_conn):
	"""
	Test Objective:
		Verify that the subWooferGain behaves properly for persistence and source changes
	"""
	
	######################### TEST 1 #########################
	# Verify that GLOBAL remains in place when source is changed

	setSource(frontDoor_conn,"BLUETOOTH","")
	set_subwooferGain(frontDoor_conn, "GLOBAL", 10)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_subwooferGain(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 2 #########################
	# Verify that if in SESSION, previous persistance is set after source change

	set_subwooferGain(frontDoor_conn, "SESSION", 20)
	setSource(frontDoor_conn,"BLUETOOTH","")
	verify_subwooferGain(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 3 #########################
	# Verify that CONTENT_ITEM settings remain after source change

	setSource(frontDoor_conn,"PRODUCT","TV")
	set_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"BLUETOOTH","")
	set_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 40)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"BLUETOOTH","")
	verify_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 40)

# surroundDelay
@pytest.mark.usefixtures("frontDoor_conn")
def test_audio_surroundDelay(frontDoor_conn):
	"""
	Test Objective:
		Verify that the surroundDelay behaves properly for persistence and source changes
	"""
	######################### TEST 1 #########################
	# Verify that GLOBAL remains in place when source is changed

	setSource(frontDoor_conn,"BLUETOOTH","")
	set_surroundDelay(frontDoor_conn, "GLOBAL", 10)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_surroundDelay(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 2 #########################
	# Verify that if in SESSION, previous persistance is set after source change

	set_surroundDelay(frontDoor_conn, "SESSION", 20)
	setSource(frontDoor_conn,"BLUETOOTH","")
	verify_surroundDelay(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 3 #########################
	# Verify that CONTENT_ITEM settings remain after source change

	setSource(frontDoor_conn,"PRODUCT","TV")
	set_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"BLUETOOTH","")
	set_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 40)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"BLUETOOTH","")
	verify_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 40)
