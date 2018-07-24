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

###################################################################################################
# TESTS BY PERSISTANCE TYPE
###################################################################################################

# global
@pytest.mark.usefixtures("frontDoor_conn")
def test_audio_global(frontDoor_conn):
	"""
	Test Objctive:

		Verify that GLOBAL remains in place when source is changed
		1. Set source to PRODUCT
		2. Set to GLOBAL with value of 10
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
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_bass(frontDoor_conn, "GLOBAL", 10)
	verify_treble(frontDoor_conn, "GLOBAL", 10)
	verify_center(frontDoor_conn, "GLOBAL", 10)
	verify_surround(frontDoor_conn, "GLOBAL", 10)
	verify_avsync(frontDoor_conn, "GLOBAL", 10)
	verify_gainOffset(frontDoor_conn, "GLOBAL", 10)
	verify_subwooferGain(frontDoor_conn, "GLOBAL", 10)
	verify_surroundDelay(frontDoor_conn, "GLOBAL", 10)

# session
@pytest.mark.usefixtures("frontDoor_conn")
def test_audio_session(frontDoor_conn):
	"""
	Test Objctive:

		Verify that if in SESSION, previous persistance is set after source change
		1. Set source to PRODUCT
		2. Set to GLOBAL with value of 10
		3. Change source to TV
		4. Change to SESSION with value of 20
		5. Change source to PRODUCT
		6. Verify, should be GLOBAL with value of 10
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
	setSource(frontDoor_conn,"PRODUCT","TV")
	set_bass(frontDoor_conn, "SESSION", 20)
	set_treble(frontDoor_conn, "SESSION", 20)
	set_center(frontDoor_conn, "SESSION", 20)
	set_surround(frontDoor_conn, "SESSION", 20)
	set_avsync(frontDoor_conn, "SESSION", 20)
	set_gainOffset(frontDoor_conn, "SESSION", 20)
	set_subwooferGain(frontDoor_conn, "SESSION", 20)
	set_surroundDelay(frontDoor_conn, "SESSION", 20)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	verify_bass(frontDoor_conn, "GLOBAL", 10)
	verify_treble(frontDoor_conn, "GLOBAL", 10)
	verify_center(frontDoor_conn, "GLOBAL", 10)
	verify_surround(frontDoor_conn, "GLOBAL", 10)
	verify_avsync(frontDoor_conn, "GLOBAL", 10)
	verify_gainOffset(frontDoor_conn, "GLOBAL", 10)
	verify_subwooferGain(frontDoor_conn, "GLOBAL", 10)
	verify_surroundDelay(frontDoor_conn, "GLOBAL", 10)

# contentitem
@pytest.mark.usefixtures("frontDoor_conn")
def test_audio_contentitem(frontDoor_conn):
	"""
	Test Objctive:

		Verify that CONTENT_ITEM settings remain after source change
		1. Set source to TV
		2. Set to CONTENT_ITEM with a value of 30 (PRODUCT_TV)
		3. Change source to PRODUCT
		4. Set to CONTENT_ITEM with a value of 40 (PRODUCT_)
		5. Change source to TV
		6. Verify, should be CONTENT_ITEM with value of 30 (PRODUCT_TV)
		7. Change source to PRODUCT
		8. Verify, should be CONTENT_ITEM with value of 40 (PRODUCT_)
	"""

	setSource(frontDoor_conn,"PRODUCT","TV")
	set_bass(frontDoor_conn, "CONTENT_ITEM", 30)
	set_treble(frontDoor_conn, "CONTENT_ITEM", 30)
	set_center(frontDoor_conn, "CONTENT_ITEM", 30)
	set_surround(frontDoor_conn, "CONTENT_ITEM", 30)
	set_avsync(frontDoor_conn, "CONTENT_ITEM", 30)
	set_gainOffset(frontDoor_conn, "CONTENT_ITEM", 30)
	set_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 30)
	set_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_bass(frontDoor_conn, "CONTENT_ITEM", 40)
	set_treble(frontDoor_conn, "CONTENT_ITEM", 40)
	set_center(frontDoor_conn, "CONTENT_ITEM", 40)
	set_surround(frontDoor_conn, "CONTENT_ITEM", 40)
	set_avsync(frontDoor_conn, "CONTENT_ITEM", 40)
	set_gainOffset(frontDoor_conn, "CONTENT_ITEM", 40)
	set_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 40)
	set_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 40)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_bass(frontDoor_conn, "CONTENT_ITEM", 30)
	verify_treble(frontDoor_conn, "CONTENT_ITEM", 30)
	verify_center(frontDoor_conn, "CONTENT_ITEM", 30)
	verify_surround(frontDoor_conn, "CONTENT_ITEM", 30)
	verify_avsync(frontDoor_conn, "CONTENT_ITEM", 30)
	verify_gainOffset(frontDoor_conn, "CONTENT_ITEM", 30)
	verify_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 30)
	verify_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	verify_bass(frontDoor_conn, "CONTENT_ITEM", 40)
	verify_treble(frontDoor_conn, "CONTENT_ITEM", 40)
	verify_center(frontDoor_conn, "CONTENT_ITEM", 40)
	verify_surround(frontDoor_conn, "CONTENT_ITEM", 40)
	verify_avsync(frontDoor_conn, "CONTENT_ITEM", 40)
	verify_gainOffset(frontDoor_conn, "CONTENT_ITEM", 40)
	verify_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 40)
	verify_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 40)

###################################################################################################
# TESTS BY ENDPOINT
###################################################################################################
"""
	Test Structure:

		TEST 1: Verify that GLOBAL remains in place when source is changed
		1. Set source to PRODUCT
		2. Set to GLOBAL with value of 10
		3. Change source to TV
		4. Verify, should be GLOBAL with value of 10

		TEST 2: Verify that if in SESSION, previous persistance is set after source change
		5. Set to SESSION with value of 20
		6. Change source to PRODUCT
		7. Verify, should be GLOBAL with value of 10

		TEST 3: Verify that CONTENT_ITEM settings remain after source change
		8. Change source to TV
		9. Set to CONTENT_ITEM with a value of 30 (PRODUCT_TV)
		10. Change source to PRODUCT
		11. Set to CONTENT_ITEM with a value of 40 (PRODUCT_)
		12. Change source to TV
		13. Verify, should be CONTENT_ITEM with value of 30 (PRODUCT_TV)
		14. Change source to PRODUCT
		15. Verify, should be CONTENT_ITEM with value of 40 (PRODUCT_)
"""
###################################################################################################
'''
# bass
@pytest.mark.usefixtures("frontDoor_conn")
def test_audio_bass(frontDoor_conn):
	"""
	Test Objective:
		Verify that the bass behaves properly for persistence and source changes
	"""

	######################### TEST 1 #########################
	# Verify that GLOBAL remains in place when source is changed

	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_bass(frontDoor_conn, "GLOBAL", 10)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_bass(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 2 #########################
	# Verify that if in SESSION, previous persistance is set after source change

	set_bass(frontDoor_conn, "SESSION", 20)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	verify_bass(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 3 #########################
	# Verify that CONTENT_ITEM settings remain after source change

	setSource(frontDoor_conn,"PRODUCT","TV")
	set_bass(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_bass(frontDoor_conn, "CONTENT_ITEM", 40)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_bass(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
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

	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_treble(frontDoor_conn, "GLOBAL", 10)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_treble(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 2 #########################
	# Verify that if in SESSION, previous persistance is set after source change

	set_treble(frontDoor_conn, "SESSION", 20)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	verify_treble(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 3 #########################
	# Verify that CONTENT_ITEM settings remain after source change

	setSource(frontDoor_conn,"PRODUCT","TV")
	set_treble(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_treble(frontDoor_conn, "CONTENT_ITEM", 40)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_treble(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
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

	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_center(frontDoor_conn, "GLOBAL", 10)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_center(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 2 #########################
	# Verify that if in SESSION, previous persistance is set after source change

	set_center(frontDoor_conn, "SESSION", 20)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	verify_center(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 3 #########################
	# Verify that CONTENT_ITEM settings remain after source change

	setSource(frontDoor_conn,"PRODUCT","TV")
	set_center(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_center(frontDoor_conn, "CONTENT_ITEM", 40)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_center(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
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

	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_surround(frontDoor_conn, "GLOBAL", 10)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_surround(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 2 #########################
	# Verify that if in SESSION, previous persistance is set after source change

	set_surround(frontDoor_conn, "SESSION", 20)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	verify_surround(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 3 #########################
	# Verify that CONTENT_ITEM settings remain after source change

	setSource(frontDoor_conn,"PRODUCT","TV")
	set_surround(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_surround(frontDoor_conn, "CONTENT_ITEM", 40)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_surround(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
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

	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_avsync(frontDoor_conn, "GLOBAL", 10)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_avsync(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 2 #########################
	# Verify that if in SESSION, previous persistance is set after source change

	set_avsync(frontDoor_conn, "SESSION", 20)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	verify_avsync(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 3 #########################
	# Verify that CONTENT_ITEM settings remain after source change

	setSource(frontDoor_conn,"PRODUCT","TV")
	set_avsync(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_avsync(frontDoor_conn, "CONTENT_ITEM", 40)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_avsync(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
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

	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_gainOffset(frontDoor_conn, "GLOBAL", 10)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_gainOffset(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 2 #########################
	# Verify that if in SESSION, previous persistance is set after source change

	set_gainOffset(frontDoor_conn, "SESSION", 20)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	verify_gainOffset(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 3 #########################
	# Verify that CONTENT_ITEM settings remain after source change

	setSource(frontDoor_conn,"PRODUCT","TV")
	set_gainOffset(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_gainOffset(frontDoor_conn, "CONTENT_ITEM", 40)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_gainOffset(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
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

	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_subwooferGain(frontDoor_conn, "GLOBAL", 10)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_subwooferGain(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 2 #########################
	# Verify that if in SESSION, previous persistance is set after source change

	set_subwooferGain(frontDoor_conn, "SESSION", 20)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	verify_subwooferGain(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 3 #########################
	# Verify that CONTENT_ITEM settings remain after source change

	setSource(frontDoor_conn,"PRODUCT","TV")
	set_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 40)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_subwooferGain(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
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

	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_surroundDelay(frontDoor_conn, "GLOBAL", 10)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_surroundDelay(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 2 #########################
	# Verify that if in SESSION, previous persistance is set after source change

	set_surroundDelay(frontDoor_conn, "SESSION", 20)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	verify_surroundDelay(frontDoor_conn, "GLOBAL", 10)

	######################### TEST 3 #########################
	# Verify that CONTENT_ITEM settings remain after source change

	setSource(frontDoor_conn,"PRODUCT","TV")
	set_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	set_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 40)
	setSource(frontDoor_conn,"PRODUCT","TV")
	verify_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 30)
	setSource(frontDoor_conn,"PRODUCT","SLOT_0")
	verify_surroundDelay(frontDoor_conn, "CONTENT_ITEM", 40)
'''