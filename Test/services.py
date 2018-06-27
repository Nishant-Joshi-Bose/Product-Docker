# services.py
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
Music service information used for Preset tests.
"""

SERVICES = {
    'AMAZON': {
        'account': {
            'account_type': 'MSP',
            'provider': 'AMAZON',
            'account_id': 'embedded.test01@gmail.com',
            'name': 'embedded.test01@gmail.com',
            'secret': 'eyJyZWZyZXNoX3Rva2VuIjogIkF0enJ8SXdFQklFTHloUXAzU2xVY1gzbFZvNUwtRkZMbGU1a0JPOUhTZ0xGalo3TWZncGJvOGx2ZkprVHc3c3BfanhuczZUUXR4QmxWRTR4eVRrYlo2cUxUOGlTX2hsQU9NSHVBcFQ5M1dYUWFENVRCeFVpcEZBN2pyaF82WHJKc1labkw2NmtDN2dlYVZ2VE45QUhTWkVSMjZXS0VuZXc0dkRhRDJWV2V4ejBodnZTeDRmRC1zQjYxek1RbDFmbkprUmNSX2hZWmNHOElRTHdUOXVBTzc1S3RvcDZzM0ZGWUp5TkVlNWZqcVJ5bERSR3k5MmJPR0lrMExSbkJ2eFQyUEZ2OWNzRWxWUlVZODJPWDBPTDBmR1ZjdWk2dTdjQjJKZzljTVFreUV2QkdiTlVWUFVOTHFSU3kzX0hYLXVtbHp5TnB3czVkQl8yWlVXQTh6XzkxOTRBREVuaTdLa19Mb2ZOZ2kzY2ZzUHBTSm5TZmZJUU9ubm81STl4R3RmdnItT001cmdJTW8wdU9HTE9ZNUNqOWJpRDVzR1hZbzB5TGZrWmp0MmFHTjd2Mk5JblA3d1M3QjRITnZ1M19zSHFsZ3J4clIxc3ZsQXk5WU0yeGY3dk4wM3pNc2Zka0dmUkVVUzNCQTA4Tm85MG9qRUdZUlZOOGFMcE5JcS1vdmpYVzRBRk1BOVlMN01hcFpiV0VmbHlnMy0zemg3anBFYWd3IiwgInNpdGVfaWQiOiAiNTgxNTM2NjAtMWQyMS00MzdjLWI3YWItYjM1MWNmOWIxMTQwIn0='
        },
        'content': [{
            'container_location': '/v1/playback/type/playable/url/cHJpbWUvc3RhdGlvbnMvQTEwMlVLQzcxSTVEVTgvI3BsYXlhYmxl/trackIndex/0',
            'container_name': 'Classic Hits',
            'track_location': '/v1/playback/type/playable/url/cHJpbWUvc3RhdGlvbnMvQTEwMlVLQzcxSTVEVTgvI3BsYXlhYmxl/trackIndex/0'
        }, {
            'container_location': '/v1/playback/type/playable/url/cHJpbWUvcGxheWxpc3RzL0IwMTVTNzlJOE8vY2h1bms9MC8jcGxheWFibGU=/trackIndex/0',
            'container_name': 'Roadtrip: Classic Rock',
            'track_location': '/v1/playback/type/chunk/url/cHJpbWUvcGxheWxpc3RzL0IwMTVTNzlJOE8vY2h1bms9MC8jY2h1bms=/trackIndex/0'
        }]
    },
    'DEEZER': {
        'account': {
            'account_type': 'MSP',
            'provider': 'DEEZER',
            'account_id': 'embedded.test01@gmail.com',
            'name': 'embedded.test01@gmail.com',
            'secret': 'nypWrm4SNPEeSrhWrb7U8Bc0Mnf2QPP38Qe9U9Fb0MwFrpo7dC'
        },
        'content': [{
            'container_location': '/v1/playback/containerType/album/containerId/7776833',
            'container_name': 'XSCAPE',
            'track_location': '/v1/playback/containerType/album/containerId/7776833/track/78213533'
        }]
    },
    'SPOTIFY': {
        'account': {
            'account_type': 'MSP',
            'provider': 'SPOTIFY',
            'account_id': 'e946bb7e-963c-4cfb-a9ce-7a64a7e0b316',
            'name': 'embedded.test01@gmail.com',
            'secret': 'AQCPahkgWzgBY1ee11lKBsMMcbd37GT7kc68Zex4YsEMhXK-kLt01uqYO51Dbr9-X0EfijNsjJj_3uwfEK6Q6kmN8l70I1YA844MvuGlaM5fyHBd3bGY4oDh4nSQykrpOAQ'
        },
        'content': [{
            'container_location':'/v1/playback/container/c3BvdGlmeTp1c2VyOjEyMTIyODgyNzYwOnBsYXlsaXN0OjdEWkhpb1E1RldaOXZhTHNPY2YwaVU=',
            'container_name': 'Take A Chance On Me',
            'container_type': 'tracklisturl',
            'track_location':'/v1/playback/container/c3BvdGlmeTp1c2VyOjEyMTIyODgyNzYwOnBsYXlsaXN0OjdEWkhpb1E1RldaOXZhTHNPY2YwaVU=/track/2',
            'track_type': 'tracklisturl'
        }, {
            'container_type': 'tracklisturl',
            'container_location':'/v1/playback/container/c3BvdGlmeTphbGJ1bToyY0taZmF6N0dpR3RaRWVRTmoxUnlS',
            'container_name': 'Knowing Me, Knowing You',
            'track_type': 'tracklisturl',
            'track_location':'/v1/playback/container/c3BvdGlmeTphbGJ1bToyY0taZmF6N0dpR3RaRWVRTmoxUnlS'
        }, {
            'container_type': 'tracklisturl',
            'container_location':'/v1/playback/container/c3BvdGlmeTphcnRpc3Q6MExjSkxxYkJtYUdVZnQxZTlNbThIVg==',
            'container_name': 'ABBA',
            'track_type': 'tracklisturl',
            'track_location':'/v1/playback/container/c3BvdGlmeTphcnRpc3Q6MExjSkxxYkJtYUdVZnQxZTlNbThIVg==/track/0'
        }, {
            'container_type': 'tracklisturl',
            'container_location':'/v1/playback/container/c3BvdGlmeTp1c2VyOnNwb3RpZnk6cGxheWxpc3Q6MzdpOWRRWkYxRFdXd2F4UmVhMUxXUw==',
            'container_name': 'Photograph',
            'track_type': 'tracklisturl',
            'track_location':'/v1/playback/container/c3BvdGlmeTp1c2VyOnNwb3RpZnk6cGxheWxpc3Q6MzdpOWRRWkYxRFdXd2F4UmVhMUxXUw==/track/0'
        }]
    },
    'TUNEIN': {
        'account': {
            'account_type': 'MSP',
            'provider': 'TUNEIN',
            'account_id': '',
            'name': '',
            'secret': 'eyJzZXJpYWwiOiAiZjQzNjRhMjUtMWE3ZC00ODRmLTgyNjUtYjc4YWU1ZjliMThmIn0='
        },
        'content': [{
            'preset_type': 'stationurl',
            'preset_location': '/playback/station/s28589',
            'preset_name': "WERS 88.9 FM is Boston's home for commercial-free modern rock",
            'playback_type': 'stationurl',
            'playback_location': '/playback/station/s28589'
        }, {
            'preset_type': 'stationurl',
            'preset_location': '/playback/station/s13606',
            'preset_name': "Radio Paradise",
            'playback_type': 'stationurl',
            'playback_location': '/playback/station/s13606'
        }, {
            'preset_type': 'stationurl',
            'preset_location': '/playback/station/s28957',
            'preset_name': "WGBH 89.7 FM Boston Public Radio",
            'playback_type': 'stationurl',
            'playback_location': '/playback/station/s28957'
        }, {
            'preset_type': 'stationurl',
            'preset_location': '/playback/station/s27567',
            'preset_name': "WUMB 91.9 FM UMass Boston Radio",
            'playback_type': 'stationurl',
            'playback_location': '/playback/station/s27567'
        }, {
            'preset_type': 'stationurl',
            'preset_location': '/playback/station/s24062',
            'preset_name': "WXRV 92.5 FM the River Boston's Independent Radio",
            'playback_type': 'stationurl',
            'playback_location': '/playback/station/s24062'
        }, {
            'preset_type': 'stationurl',
            'preset_location': '/playback/station/s30913',
            'preset_name': "WMBR 88.1 FM at MIT",
            'playback_type': 'stationurl',
            'playback_location': '/playback/station/s30913'
        }, {
            'preset_type': 'stationurl',
            'preset_location': '/playback/station/s27654',
            'preset_name': "WBUR 90.9 FM Boston's NPR News Station",
            'playback_type': 'stationurl',
            'playback_location': '/playback/station/s27654'
        }]
    },
    'PANDORA': {
        'account': {
            'account_type': 'MSP',
            'provider': 'PANDORA',
            'account_id': 'embedded.test01@gmail.com',
            'name': 'embedded.test01@gmail.com',
            'secret': 'eyJzaGFyZWREZXZpY2VJZCI6ICIxYTBiYzZhOS04NmYyLTQ0MTItOTEyMi1mNjg3OWU0MmU3MmQiLCAiZGV2aWNlSWQiOiAiIn0=',
        },
        'content': [{
            'container_location': '/v1/playback/token/3828911929411831692',
            'container_name': 'The Beatles Radio',
            'container_type': 'tracklisturl',
            'track_location': '/v1/playback/token/3828911929411831692',
            'track_type': 'tracklisturl'
        }]
    }
}


class MusicService(object):
    """
    Enum-like class for storing Music service names.
    """
    AMAZON = "AMAZON"
    DEEZER = "DEEZER"
    PANDORA = "PANDORA"
    SPOTIFY = "SPOTIFY"
    TUNEIN = "TUNEIN"
    RANDOM = "RANDOM"
