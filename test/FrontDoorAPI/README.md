
PyMadrid 
===============

Python utility for FrontDoorAPI's
This is the command-line utility to send FD requests and tries to closely simulate the Madrid Application.
More improvements to this tool are work in progress.

TYPICAL SCENARIO's

Method 1:
```
$ cd /scratch
$ git clone git@github.com:BoseCorp/CastleTestUtils.git
$ sudo pip2.7 install -r CastleTestUtils/requirements.txt
$ python2.7 -m CastleTestUtils.FrontDoorAPI.madrid 
```

Method 2:
```
$ cd /scratch
$ cd /scratch/
$ git clone git@github.com:BoseCorp/CastleTools.git
$ git clone git@github.com:BoseCorp/CastleEddie.git
$ export PATH=$PATH:/scratch/CastleTools/bin/

$ cd Eddie/
$ sudo pip2.7 install -r requirements.txt
$ make 

$ cd builds/
$ python2.7 -m CastleTestUtils.FrontDoorAPI.madrid 
```

### Quick Steps to get Music Service playing on the Riviera Board.  
TODO: Change the IP-Address in madrid.py
1. Create a Bose-PersonID (Passport Account)
Note: This bosepersonID is vital; hence cache it somewhere
```
$ python2.7 -m CastleTestUtils.FrontDoorAPI.madrid create_boseperson
```

2. Link Product (Riviera-Board) and Bose-PersonID
```
$ python2.7 -m CastleTestUtils.FrontDoorAPI.madrid add_product
```

3. Check if Product and Passport Accounts have linked
```
$ python2.7 -m CastleTestUtils.FrontDoorAPI.madrid get_productsInfo
```

4. Add  Music Service Provider (MSP's) to Bose-PersonID  
Note: If you want to add a different account - Please make changes to config file.  
After this step; reboot the system; if cloudSync does not happen
```
$ python2.7 -m CastleTestUtils.FrontDoorAPI.madrid add_MSPAccounts
$ python2.7 -m CastleTestUtils.FrontDoorAPI.madrid cloudSync
```

5. Check if MSP's have been linked to Bose-PersonID
```
$ python2.7 -m CastleTestUtils.FrontDoorAPI.madrid get_MSPDetails
```

6. Send playbackRequest for 'source' of interest 
```
$ python2.7 -m CastleTestUtils.FrontDoorAPI.madrid content_playbackRequest
```

7. Send nowPlaying request and verify playStatus
```
$ python2.7 -m CastleTestUtils.FrontDoorAPI.madrid content_nowPlaying
```
