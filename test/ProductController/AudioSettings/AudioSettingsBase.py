import json
import pytest
import time

@pytest.mark.usefixtures("frontDoor_conn")
def setSource(frontDoor_conn, source, sourceAccount):
	message = {"source" : source,"sourceAccount" : sourceAccount}
	jsonMessage = json.dumps(message, indent=4)
	source_dict = frontDoor_conn.sendPlaybackRequest(jsonMessage)
	time.sleep(5)
	assert source_dict['body']['container']['contentItem']['source'] == source
	assert source_dict['body']['container']['contentItem']['sourceAccount'] == sourceAccount

########## bass

@pytest.mark.usefixtures("frontDoor_conn")
def set_bass(frontDoor_conn, persistence, value):
	message = {"persistence" : persistence,"value" : value}
	jsonMessage = json.dumps(message, indent=4)
	bass_dict = frontDoor_conn.setBassLevel(jsonMessage)

@pytest.mark.usefixtures("frontDoor_conn")
def verify_bass(frontDoor_conn, persistence, value):
	bass_dict = frontDoor_conn.getBassLevel()
	time.sleep(1)
	assert bass_dict['body']['persistence'] == persistence
	assert bass_dict['body']['value'] == value

@pytest.mark.usefixtures("frontDoor_conn")
def set_bass_error(frontDoor_conn, persistence, value, error_message):
	message = {"persistence" : persistence,"value" : value}
	jsonMessage = json.dumps(message, indent=4)
	bass_dict = frontDoor_conn.setBassLevel(jsonMessage)
	time.sleep(1)
	assert bass_dict['error']['message'] == error_message

########## treble

@pytest.mark.usefixtures("frontDoor_conn")
def set_treble(frontDoor_conn, persistence, value):
	message = {"persistence" : persistence,"value" : value}
	jsonMessage = json.dumps(message, indent=4)
	treble_dict = frontDoor_conn.setTreble(jsonMessage)

@pytest.mark.usefixtures("frontDoor_conn")
def verify_treble(frontDoor_conn, persistence, value):
	treble_dict = frontDoor_conn.getTreble()
	time.sleep(1)
	assert treble_dict['body']['persistence'] == persistence
	assert treble_dict['body']['value'] == value

@pytest.mark.usefixtures("frontDoor_conn")
def set_treble_error(frontDoor_conn, persistence, value, error_message):
	message = {"persistence" : persistence,"value" : value}
	jsonMessage = json.dumps(message, indent=4)
	treble_dict = frontDoor_conn.setTreble(jsonMessage)
	time.sleep(1)
	assert treble_dict['error']['message'] == error_message

########## center

@pytest.mark.usefixtures("frontDoor_conn")
def set_center(frontDoor_conn, persistence, value):
	message = {"persistence" : persistence,"value" : value}
	jsonMessage = json.dumps(message, indent=4)
	center_dict = frontDoor_conn.setCenter(jsonMessage)

@pytest.mark.usefixtures("frontDoor_conn")
def verify_center(frontDoor_conn, persistence, value):
	center_dict = frontDoor_conn.getCenter()
	time.sleep(1)
	assert center_dict['body']['persistence'] == persistence
	assert center_dict['body']['value'] == value

@pytest.mark.usefixtures("frontDoor_conn")
def set_center_error(frontDoor_conn, persistence, value, error_message):
	message = {"persistence" : persistence,"value" : value}
	jsonMessage = json.dumps(message, indent=4)
	center_dict = frontDoor_conn.setCenter(jsonMessage)
	time.sleep(1)
	assert center_dict['error']['message'] == error_message

########## surround

@pytest.mark.usefixtures("frontDoor_conn")
def set_surround(frontDoor_conn, persistence, value):
	message = {"persistence" : persistence,"value" : value}
	jsonMessage = json.dumps(message, indent=4)
	surround_dict = frontDoor_conn.setSurround(jsonMessage)

@pytest.mark.usefixtures("frontDoor_conn")
def verify_surround(frontDoor_conn, persistence, value):
	surround_dict = frontDoor_conn.getSurround()
	time.sleep(1)
	assert surround_dict['body']['persistence'] == persistence
	assert surround_dict['body']['value'] == value

@pytest.mark.usefixtures("frontDoor_conn")
def set_surround_error(frontDoor_conn, persistence, value, error_message):
	message = {"persistence" : persistence,"value" : value}
	jsonMessage = json.dumps(message, indent=4)
	surround_dict = frontDoor_conn.setSurround(jsonMessage)
	time.sleep(1)
	assert surround_dict['error']['message'] == error_message

########## avSync

@pytest.mark.usefixtures("frontDoor_conn")
def set_avsync(frontDoor_conn, persistence, value):
	message = {"persistence" : persistence,"value" : value}
	jsonMessage = json.dumps(message, indent=4)
	avsync_dict = frontDoor_conn.setAVsync(jsonMessage)

@pytest.mark.usefixtures("frontDoor_conn")
def verify_avsync(frontDoor_conn, persistence, value):
	avsync_dict = frontDoor_conn.getAVsync()
	time.sleep(1)
	assert avsync_dict['body']['persistence'] == persistence
	assert avsync_dict['body']['value'] == value

@pytest.mark.usefixtures("frontDoor_conn")
def set_avsync_error(frontDoor_conn, persistence, value, error_message):
	message = {"persistence" : persistence,"value" : value}
	jsonMessage = json.dumps(message, indent=4)
	avsync_dict = frontDoor_conn.setAVsync(jsonMessage)
	time.sleep(1)
	assert avsync_dict['error']['message'] == error_message

########## gainOffset

@pytest.mark.usefixtures("frontDoor_conn")
def set_gainOffset(frontDoor_conn, persistence, value):
	message = {"persistence" : persistence,"value" : value}
	jsonMessage = json.dumps(message, indent=4)
	gainOffset_dict = frontDoor_conn.setGainOffset(jsonMessage)


@pytest.mark.usefixtures("frontDoor_conn")
def verify_gainOffset(frontDoor_conn, persistence, value):
	gainOffset_dict = frontDoor_conn.getGainOffset()
	time.sleep(1)
	assert gainOffset_dict['body']['persistence'] == persistence
	assert gainOffset_dict['body']['value'] == value

@pytest.mark.usefixtures("frontDoor_conn")
def set_gainOffset_error(frontDoor_conn, persistence, value, error_message):
	message = {"persistence" : persistence,"value" : value}
	jsonMessage = json.dumps(message, indent=4)
	gainOffset_dict = frontDoor_conn.setGainOffset(jsonMessage)
	time.sleep(1)
	assert gainOffset_dict['error']['message'] == error_message

########## subwooferGain

@pytest.mark.usefixtures("frontDoor_conn")
def set_subwooferGain(frontDoor_conn, persistence, value):
	message = {"persistence" : persistence,"value" : value}
	jsonMessage = json.dumps(message, indent=4)
	subwooferGain_dict = frontDoor_conn.setSubwooferGain(jsonMessage)

@pytest.mark.usefixtures("frontDoor_conn")
def verify_subwooferGain(frontDoor_conn, persistence, value):
	subwooferGain_dict = frontDoor_conn.getSubwooferGain()
	time.sleep(1)
	assert subwooferGain_dict['body']['persistence'] == persistence
	assert subwooferGain_dict['body']['value'] == value

@pytest.mark.usefixtures("frontDoor_conn")
def set_subwooferGain_error(frontDoor_conn, persistence, value, error_message):
	message = {"persistence" : persistence,"value" : value}
	jsonMessage = json.dumps(message, indent=4)
	subwooferGain_dict = frontDoor_conn.setSubwooferGain(jsonMessage)
	time.sleep(1)
	assert subwooferGain_dict['error']['message'] == error_message

########## surroundDelay

@pytest.mark.usefixtures("frontDoor_conn")
def set_surroundDelay(frontDoor_conn, persistence, value):
	message = {"persistence" : persistence,"value" : value}
	jsonMessage = json.dumps(message, indent=4)
	surroundDelay_dict = frontDoor_conn.setSurroundDelay(jsonMessage)

@pytest.mark.usefixtures("frontDoor_conn")
def verify_surroundDelay(frontDoor_conn, persistence, value):
	surroundDelay_dict = frontDoor_conn.getSurroundDelay()
	time.sleep(1)
	assert surroundDelay_dict['body']['persistence'] == persistence
	assert surroundDelay_dict['body']['value'] == value

@pytest.mark.usefixtures("frontDoor_conn")
def set_surroundDelay_error(frontDoor_conn, persistence, value, error_message):
	message = {"persistence" : persistence,"value" : value}
	jsonMessage = json.dumps(message, indent=4)
	surroundDelay_dict = frontDoor_conn.setSurroundDelay(jsonMessage)
	time.sleep(1)
	assert surroundDelay_dict['error']['message'] == error_message