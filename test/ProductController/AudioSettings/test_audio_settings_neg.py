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
    This is the test script file which contains audio settings tests,
    specifically testing negative test cases.
"""

import json
import pytest
import time

from AudioSettingsBase import *

###################################################################################################
# TESTS BY PERSISTANCE TYPE
###################################################################################################

# global bounds
@pytest.mark.usefixtures("frontDoor_conn")
def test_audio_global_bounds(frontDoor_conn):
	"""
	Test Objctive:

		Verify that out of bounds values do not go through
		1. Set source to PRODUCT
		2. Set to GLOBAL with value of 10
		2. Set to GLOBAL with value of 220
		3. Change source to TV
		4. Verify, should be GLOBAL with value of 10
	"""
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_bass(frontDoor_conn, "GLOBAL", 10)
	set_treble(frontDoor_conn, "GLOBAL", 10)
	set_center(frontDoor_conn, "GLOBAL", 10)
	set_surround(frontDoor_conn, "GLOBAL", 10)
	set_avsync(frontDoor_conn, "GLOBAL", 10)
	set_gainOffset(frontDoor_conn, "GLOBAL", 10)
	set_subwooferGain(frontDoor_conn, "GLOBAL", 10)
	set_surroundDelay(frontDoor_conn, "GLOBAL", 10)
	set_bass(frontDoor_conn, "GLOBAL", 220)
	set_treble(frontDoor_conn, "GLOBAL", 220)
	set_center(frontDoor_conn, "GLOBAL", 220)
	set_surround(frontDoor_conn, "GLOBAL", 220)
	set_avsync(frontDoor_conn, "GLOBAL", 220)
	set_gainOffset(frontDoor_conn, "GLOBAL", 220)
	set_subwooferGain(frontDoor_conn, "GLOBAL", 220)
	set_surroundDelay(frontDoor_conn, "GLOBAL", 220)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_bass(frontDoor_conn, "GLOBAL", 10)
	verify_treble(frontDoor_conn, "GLOBAL", 10)
	verify_center(frontDoor_conn, "GLOBAL", 10)
	verify_surround(frontDoor_conn, "GLOBAL", 10)
	verify_avsync(frontDoor_conn, "GLOBAL", 10)
	verify_gainOffset(frontDoor_conn, "GLOBAL", 10)
	verify_subwooferGain(frontDoor_conn, "GLOBAL", 10)
	verify_surroundDelay(frontDoor_conn, "GLOBAL", 10)

# global step
@pytest.mark.usefixtures("frontDoor_conn")
def test_audio_global_step(frontDoor_conn):
	"""
	Test Objctive:

		Verify that values outside of step range do not go through
		1. Set source to PRODUCT
		2. Set to GLOBAL with value of 10
		3. Set to GLOBAL with value of 12
		4. Change source to TV
		5. Verify, should be GLOBAL with value of 10
	"""
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_bass(frontDoor_conn, "GLOBAL", 10)
	set_treble(frontDoor_conn, "GLOBAL", 10)
	set_center(frontDoor_conn, "GLOBAL", 10)
	set_surround(frontDoor_conn, "GLOBAL", 10)
	set_avsync(frontDoor_conn, "GLOBAL", 10)
	set_gainOffset(frontDoor_conn, "GLOBAL", 10)
	set_subwooferGain(frontDoor_conn, "GLOBAL", 10)
	set_surroundDelay(frontDoor_conn, "GLOBAL", 10)
	set_bass(frontDoor_conn, "GLOBAL", 12)
	set_treble(frontDoor_conn, "GLOBAL", 12)
	set_center(frontDoor_conn, "GLOBAL", 12)
	set_surround(frontDoor_conn, "GLOBAL", 12)
	set_avsync(frontDoor_conn, "GLOBAL", 12)
	set_gainOffset(frontDoor_conn, "GLOBAL", 12)
	set_subwooferGain(frontDoor_conn, "GLOBAL", 12)
	set_surroundDelay(frontDoor_conn, "GLOBAL", 12)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_bass(frontDoor_conn, "GLOBAL", 10)
	verify_treble(frontDoor_conn, "GLOBAL", 10)
	verify_center(frontDoor_conn, "GLOBAL", 10)
	verify_surround(frontDoor_conn, "GLOBAL", 10)
	verify_avsync(frontDoor_conn, "GLOBAL", 10)
	verify_gainOffset(frontDoor_conn, "GLOBAL", 10)
	verify_subwooferGain(frontDoor_conn, "GLOBAL", 10)
	verify_surroundDelay(frontDoor_conn, "GLOBAL", 10)

# session bounds
@pytest.mark.usefixtures("frontDoor_conn")
def test_audio_session_bounds(frontDoor_conn):
	"""
	Test Objctive:

		Verify that out of bounds values do not go through
		1. Set source to PRODUCT
		2. Change to SESSION with value of 20
		3. Change to SESSION with value of 300
		4. Verify, should be SESSION with value of 20
	"""

	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_bass(frontDoor_conn, "SESSION", 20)
	set_treble(frontDoor_conn, "SESSION", 20)
	set_center(frontDoor_conn, "SESSION", 20)
	set_surround(frontDoor_conn, "SESSION", 20)
	set_avsync(frontDoor_conn, "SESSION", 20)
	set_gainOffset(frontDoor_conn, "SESSION", 20)
	set_subwooferGain(frontDoor_conn, "SESSION", 20)
	set_surroundDelay(frontDoor_conn, "SESSION", 20)
	set_bass(frontDoor_conn, "SESSION", 300)
	set_treble(frontDoor_conn, "SESSION", 300)
	set_center(frontDoor_conn, "SESSION", 300)
	set_surround(frontDoor_conn, "SESSION", 300)
	set_avsync(frontDoor_conn, "SESSION", 300)
	set_gainOffset(frontDoor_conn, "SESSION", 300)
	set_subwooferGain(frontDoor_conn, "SESSION", 300)
	set_surroundDelay(frontDoor_conn, "SESSION", 300)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	verify_bass(frontDoor_conn, "SESSION", 20)
	verify_treble(frontDoor_conn, "SESSION", 20)
	verify_center(frontDoor_conn, "SESSION", 20)
	verify_surround(frontDoor_conn, "SESSION", 20)
	verify_avsync(frontDoor_conn, "SESSION", 20)
	verify_gainOffset(frontDoor_conn, "SESSION", 20)
	verify_subwooferGain(frontDoor_conn, "SESSION", 20)
	verify_surroundDelay(frontDoor_conn, "SESSION", 20)

# session step
@pytest.mark.usefixtures("frontDoor_conn")
def test_audio_session_step(frontDoor_conn):
	"""
	Test Objctive:

		Verify that values outside of step range do not go through
		1. Set source to PRODUCT
		2. Change to SESSION with value of 20
		3. Change to SESSION with value of 13
		4. Verify, should be SESSION with value of 20
	"""

	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_bass(frontDoor_conn, "SESSION", 20)
	set_treble(frontDoor_conn, "SESSION", 20)
	set_center(frontDoor_conn, "SESSION", 20)
	set_surround(frontDoor_conn, "SESSION", 20)
	set_avsync(frontDoor_conn, "SESSION", 20)
	set_gainOffset(frontDoor_conn, "SESSION", 20)
	set_subwooferGain(frontDoor_conn, "SESSION", 20)
	set_surroundDelay(frontDoor_conn, "SESSION", 20)
	set_bass(frontDoor_conn, "SESSION", 13)
	set_treble(frontDoor_conn, "SESSION", 13)
	set_center(frontDoor_conn, "SESSION", 13)
	set_surround(frontDoor_conn, "SESSION", 13)
	set_avsync(frontDoor_conn, "SESSION", 13)
	set_gainOffset(frontDoor_conn, "SESSION", 13)
	set_subwooferGain(frontDoor_conn, "SESSION", 13)
	set_surroundDelay(frontDoor_conn, "SESSION", 13)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	verify_bass(frontDoor_conn, "SESSION", 20)
	verify_treble(frontDoor_conn, "SESSION", 20)
	verify_center(frontDoor_conn, "SESSION", 20)
	verify_surround(frontDoor_conn, "SESSION", 20)
	verify_avsync(frontDoor_conn, "SESSION", 20)
	verify_gainOffset(frontDoor_conn, "SESSION", 20)
	verify_subwooferGain(frontDoor_conn, "SESSION", 20)
	verify_surroundDelay(frontDoor_conn, "SESSION", 20)

# contentitem bounds
@pytest.mark.usefixtures("frontDoor_conn")
def test_audio_contentitem_bounds(frontDoor_conn):
	"""
	Test Objctive:

		Verify that out of bounds values do not go through
		1. Set source to TV
		2. Set to CONTENT_ITEM with a value of 20 (TV)
		3. Set to CONTENT_ITEM with a value of 400 (TV)
		4. Change source to PRODUCT
		5. Set to CONTENT_ITEM with a value of 30 (PRODUCT)
		6. Set to CONTENT_ITEM with a value of 500 (PRODUCT)
		7. Change source to TV
		8. Verify, should be CONTENT_ITEM with value of 20 (TV)
		9. Change source to PRODUCT
		10. Verify, should be CONTENT_ITEM with value of 30 (PRODUCT)
	"""

	setSource(frontDoor_conn,"PRODUCT","TV")
	set_bass(frontDoor_conn, "CONTENT_ITEM", 20)
	set_treble(frontDoor_conn, "CONTENT_ITEM", 20)
	set_center(frontDoor_conn, "CONTENT_ITEM", 20)
	set_surround(frontDoor_conn, "CONTENT_ITEM", 20)
	set_avsync(frontDoor_conn, "CONTENT_ITEM", 20)
	set_gainOffset(frontDoor_conn, "CONTENT_ITEM", 20)
	set_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 20)
	set_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 20)
	set_bass(frontDoor_conn, "CONTENT_ITEM", 400)
	set_treble(frontDoor_conn, "CONTENT_ITEM", 400)
	set_center(frontDoor_conn, "CONTENT_ITEM", 400)
	set_surround(frontDoor_conn, "CONTENT_ITEM", 400)
	set_avsync(frontDoor_conn, "CONTENT_ITEM", 400)
	set_gainOffset(frontDoor_conn, "CONTENT_ITEM", 400)
	set_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 400)
	set_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 400)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_bass(frontDoor_conn, "CONTENT_ITEM", 30)
	set_treble(frontDoor_conn, "CONTENT_ITEM", 30)
	set_center(frontDoor_conn, "CONTENT_ITEM", 30)
	set_surround(frontDoor_conn, "CONTENT_ITEM", 30)
	set_avsync(frontDoor_conn, "CONTENT_ITEM", 30)
	set_gainOffset(frontDoor_conn, "CONTENT_ITEM", 30)
	set_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 30)
	set_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 30)
	set_bass(frontDoor_conn, "CONTENT_ITEM", 500)
	set_treble(frontDoor_conn, "CONTENT_ITEM", 500)
	set_center(frontDoor_conn, "CONTENT_ITEM", 500)
	set_surround(frontDoor_conn, "CONTENT_ITEM", 500)
	set_avsync(frontDoor_conn, "CONTENT_ITEM", 500)
	set_gainOffset(frontDoor_conn, "CONTENT_ITEM", 500)
	set_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 500)
	set_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 500)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_bass(frontDoor_conn, "CONTENT_ITEM", 20)
	verify_treble(frontDoor_conn, "CONTENT_ITEM", 20)
	verify_center(frontDoor_conn, "CONTENT_ITEM", 20)
	verify_surround(frontDoor_conn, "CONTENT_ITEM", 20)
	verify_avsync(frontDoor_conn, "CONTENT_ITEM", 20)
	verify_gainOffset(frontDoor_conn, "CONTENT_ITEM", 20)
	verify_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 20)
	verify_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 20)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	verify_bass(frontDoor_conn, "CONTENT_ITEM", 30)
	verify_treble(frontDoor_conn, "CONTENT_ITEM", 30)
	verify_center(frontDoor_conn, "CONTENT_ITEM", 30)
	verify_surround(frontDoor_conn, "CONTENT_ITEM", 30)
	verify_avsync(frontDoor_conn, "CONTENT_ITEM", 30)
	verify_gainOffset(frontDoor_conn, "CONTENT_ITEM", 30)
	verify_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 30)
	verify_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 30)

	# contentitem step
@pytest.mark.usefixtures("frontDoor_conn")
def test_audio_contentitem_step(frontDoor_conn):
	"""
	Test Objctive:

		Verify that values outside of step range do not go through
		1. Set source to TV
		2. Set to CONTENT_ITEM with a value of 20 (TV)
		3. Set to CONTENT_ITEM with a value of 14 (TV)
		4. Change source to PRODUCT
		5. Set to CONTENT_ITEM with a value of 30 (PRODUCT)
		6. Set to CONTENT_ITEM with a value of 24 (PRODUCT)
		7. Change source to TV
		8. Verify, should be CONTENT_ITEM with value of 20 (TV)
		9. Change source to PRODUCT
		10. Verify, should be CONTENT_ITEM with value of 30 (PRODUCT)
	"""

	setSource(frontDoor_conn,"PRODUCT","TV")
	set_bass(frontDoor_conn, "CONTENT_ITEM", 20)
	set_treble(frontDoor_conn, "CONTENT_ITEM", 20)
	set_center(frontDoor_conn, "CONTENT_ITEM", 20)
	set_surround(frontDoor_conn, "CONTENT_ITEM", 20)
	set_avsync(frontDoor_conn, "CONTENT_ITEM", 20)
	set_gainOffset(frontDoor_conn, "CONTENT_ITEM", 20)
	set_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 20)
	set_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 20)
	set_bass(frontDoor_conn, "CONTENT_ITEM", 14)
	set_treble(frontDoor_conn, "CONTENT_ITEM", 14)
	set_center(frontDoor_conn, "CONTENT_ITEM", 14)
	set_surround(frontDoor_conn, "CONTENT_ITEM", 14)
	set_avsync(frontDoor_conn, "CONTENT_ITEM", 14)
	set_gainOffset(frontDoor_conn, "CONTENT_ITEM", 14)
	set_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 14)
	set_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 14)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_bass(frontDoor_conn, "CONTENT_ITEM", 30)
	set_treble(frontDoor_conn, "CONTENT_ITEM", 30)
	set_center(frontDoor_conn, "CONTENT_ITEM", 30)
	set_surround(frontDoor_conn, "CONTENT_ITEM", 30)
	set_avsync(frontDoor_conn, "CONTENT_ITEM", 30)
	set_gainOffset(frontDoor_conn, "CONTENT_ITEM", 30)
	set_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 30)
	set_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 30)
	set_bass(frontDoor_conn, "CONTENT_ITEM", 24)
	set_treble(frontDoor_conn, "CONTENT_ITEM", 24)
	set_center(frontDoor_conn, "CONTENT_ITEM", 24)
	set_surround(frontDoor_conn, "CONTENT_ITEM", 24)
	set_avsync(frontDoor_conn, "CONTENT_ITEM", 24)
	set_gainOffset(frontDoor_conn, "CONTENT_ITEM", 24)
	set_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 24)
	set_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 24)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_bass(frontDoor_conn, "CONTENT_ITEM", 20)
	verify_treble(frontDoor_conn, "CONTENT_ITEM", 20)
	verify_center(frontDoor_conn, "CONTENT_ITEM", 20)
	verify_surround(frontDoor_conn, "CONTENT_ITEM", 20)
	verify_avsync(frontDoor_conn, "CONTENT_ITEM", 20)
	verify_gainOffset(frontDoor_conn, "CONTENT_ITEM", 20)
	verify_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 20)
	verify_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 20)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	verify_bass(frontDoor_conn, "CONTENT_ITEM", 30)
	verify_treble(frontDoor_conn, "CONTENT_ITEM", 30)
	verify_center(frontDoor_conn, "CONTENT_ITEM", 30)
	verify_surround(frontDoor_conn, "CONTENT_ITEM", 30)
	verify_avsync(frontDoor_conn, "CONTENT_ITEM", 30)
	verify_gainOffset(frontDoor_conn, "CONTENT_ITEM", 30)
	verify_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 30)
	verify_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 30)