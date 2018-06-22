# musicplayer.py
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
Music Player that utilizes configuration in services.py
"""
import argparse
import time
from logging import WARNING, DEBUG
from random import sample, randint
from CastleTestUtils.CAPSUtils.TransportUtils.commonBehaviorHandler import CommonBehaviorHandler
from CastleTestUtils.CAPSUtils.TransportUtils.messageCreator import MessageCreator
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from ..services import SERVICES, MusicService

MAX_RETRIES = 20

class MusicPlayerException(Exception):
    pass


def music_player_factory(ip_address, music_service, front_door, log_level=WARNING):
    """
    Music Playing Service factory.

    :param ip_address: The IP Address of the device that will play music.
    :param music_service: The music service name that will be used.
        Can, alternately, be set to Random.

    :return: A MusicPlayer object.
    """
    log = get_logger(__name__, level=log_level)
    log.debug("Attempting to play %s on %s", music_service, ip_address)
    if music_service.lower() in ['amazon', 'spotify', 'tunein', 'pandora']:
        return SingleServiceMusicPlayer(ip_address=ip_address, music_service=music_service.upper(), front_door=front_door, log_level=log_level)
    elif music_service.lower() == 'random':
        return RandomMusicPlayer(ip_address=ip_address, front_door=front_door, log_level=log_level)
    else:
        raise MusicPlayerException("Service, {}, is not available.".format(music_service))

class MusicPlayer(object):
    """
    Base class for operating with a Music Player.
    """
    content_pointer = 0

    def __init__(self, ip_address, music_service, front_door):
        self.ip_address = ip_address
        self.music_service = music_service
        self.front_door = front_door

    def play_music(self, music_service, playback_content):
        """
        Will play the Music Service's content and verify its playback.

        :param music_service: The name of the music source that will be played
        :param playback_content: The content from the service that will be
            played
        :return: The dictionary response from the playback request
        """
        if music_service.lower() in ['amazon', 'spotify', 'tunein', 'pandora']:
            # We need the account information to be used
            playback_account = SERVICES[music_service]['account']
            self.logger.debug('Playback Account: %s', playback_account)

            # We need the content to be played
            self.logger.debug('Playback Content: %s', playback_content)

            # We need to generate a message
            message_creator = MessageCreator(music_service, self.logger)
            # Special Case for TuneIn Radio
            if music_service.lower() in ['tunein']:
                playback_message = message_creator.playback_msg(playback_account['name'],
                                                                playback_content['preset_location'],
                                                                playback_content['preset_name'],
                                                                playback_content['playback_location'],
                                                                playback_content['preset_type'],
                                                                playback_content['playback_type'])
            else:
                playback_message = message_creator.playback_msg(playback_account['name'],
                                                                playback_content['container_location'],
                                                                playback_content['container_name'],
                                                                playback_content['track_location'])
            self.logger.debug('Playback Message: %s', playback_message)

            # We need a response handler
            # We need a common behavior handler
            common_behavior_handler = CommonBehaviorHandler(self.front_door, message_creator, music_service, playback_account['name'],
                                                            self.logger)

            # We need to check playback
            now_playing_response = common_behavior_handler.playContentItemAndVerifyPlayStatus(playback_message)
            self.logger.debug('Playback Response: %s', now_playing_response)
            return now_playing_response

        else:
            # We can't support PANDORA (or others) at this time.
            self.logger.warning("%s is not a valid source right now.", music_service)
            raise MusicPlayerException("{} is not a valid source right now.".format(music_service))

    def stop_playback(self):
        """
        Attempts to stop the music playback through a FrontDoor request

        :return: FrontDoor.stopPlaybackRequest response
        """
        return self.front_door.stopPlaybackRequest()

    def play_next_station(self):
        raise NotImplementedError

    def _increment_content(self):
        raise NotImplementedError

    def __del__(self):
        """
        On destruction of the object, close outstanding connections.

        :return: None
        """
        if self.front_door:
            self.front_door.close()


class SingleServiceMusicPlayer(MusicPlayer):
    """
    MusicPlayer for a Single Music Service
    Playing a new station will cycle through the service's content
    """
    def __init__(self, ip_address, music_service, front_door, random_content=True, log_level=DEBUG):
        self.logger = get_logger(self.__class__.__name__, level=log_level)
        MusicPlayer.__init__(self, ip_address, music_service, front_door)
        self.logger.debug("Music Service: %s", self.music_service)
        if random_content:
            self.content_pointer = randint(0, len(SERVICES[self.music_service]['content']) - 1)

    def play_next_station(self):
        """
        Actively plays the next station in the list of content for the Music Service

        :return: The dictionary response from the playback request
        """
        # Figure out how to get the next content
        self.current_content = SERVICES[self.music_service]['content'][self.content_pointer]
        if self.music_service == MusicService.TUNEIN:
            self.logger.debug("Attempting to play: %s", self.current_content['preset_name'])
        else:
            self.logger.debug("Attempting to play: %s", self.current_content['container_name'])
        self._increment_content()
        return self.play_music(self.music_service, self.current_content)

    def _increment_content(self):
        """
        Updates the content pointer location for the current music service.
        Will recycle the list if pointer reaches the end.

        :return: None
        """
        self.content_pointer += 1
        number_contents = len(SERVICES[self.music_service]['content'])
        if self.content_pointer >= number_contents:
            self.content_pointer = 0


class RandomMusicPlayer(MusicPlayer):
    """
    MusicPlayer for a Random Music Service.
    Playing a new station will pick a random music service.
    """
    service_pointer = 0
    service_list = []
    failure_count = 0

    def __init__(self, ip_address, front_door, log_level=DEBUG):
        self.logger = get_logger(self.__class__.__name__, level=log_level)
        MusicPlayer.__init__(self, ip_address, MusicService.RANDOM, front_door)
        self.logger.debug("Music Service: %s", self.music_service)
        self.shuffle()

    def shuffle(self):
        """
        Will shuffle all of the music services that are available and
        reset the pointer to current music service.

        It will not start playing music. This can be called as many times as necessary.

        :return: None
        """
        # Get all of the services available
        if not len(self.service_list):
            self.service_list = []
            # Stick all of the items into an easily randomizable list
            for service_number, service in enumerate(SERVICES):
                number_contents = len(SERVICES[service]['content'])
                self.service_list.extend([(service, content_number) for content_number in range(0, number_contents, 1)])
            assert self.service_list, "Service List generation issue: {}".format(self.service_list)

        # Randomize the Services
        self.service_list = sample(self.service_list, len(self.service_list))

    def play_next_station(self):
        """
        Will attempt to play the next station in the list and yield out
        information about that station

        :return: The dictionary response from the playback request
        """
        self.music_service, music_content_number = self.service_list[self.content_pointer]
        self.current_content = SERVICES[self.music_service]['content'][music_content_number]
        self._increment_content()
        try:
            playback_response = self.play_music(self.music_service, self.current_content)
            self.failure_count = 0
        except MusicPlayerException as exception:
            self.logger.warning("Retrying up to {} more times.".format(MAX_RETRIES - self.failure_count))
            self.failure_count += 1

            # MAX_RETRIES should be removed, but all music services are not working properly.
            if self.failure_count >= MAX_RETRIES:
                raise exception
            self.shuffle()
            playback_response = self.play_next_station()

        return playback_response

    def _increment_content(self):
        """
        Updates the content pointer location for the current music service.
        Will recycle the list if pointer reaches the end.

        :return: None
        """
        self.content_pointer += 1
        number_contents = len(self.service_list)
        if self.content_pointer >= number_contents:
            self.shuffle()
            self.content_pointer = 0


def main(arguments):
    """
    Simple check that the FrontDoor API will get the languages on the system.

    :param arguments: ArgParser based arguments
    :return: None
    """
    music_player = music_player_factory(arguments.ip_address, arguments.service)
    music_player.play_next_station()
    if arguments.play_time:
        time.sleep(arguments.play_time)
        music_player.stop_playback()


def parse_arguments():
    """"
    Parses command line arguments to the __main__ function.

    :return: Namespace parsed command line arguments.
    """
    parser = argparse.ArgumentParser(
        description="Uses the FrontDoor API against the target IP Address to get the languages.")
    parser.add_argument('--ip-address', action='store', type=str, required=True,
                        help="The IP Address of the target.")
    parser.add_argument('--service', action='store', type=str, required=False, default=MusicService.RANDOM,
                        help="Music service that you would like to play. Defaults to a Random mix.")
    parser.add_argument('--play-time', action='store', type=int, default=None,
                        help="How long you would like the music to play in seconds. "
                             "If <=0 or not given, will play continuously. Abort with ^c.")

    return parser.parse_args()


if __name__ == "__main__":
    main(parse_arguments())
