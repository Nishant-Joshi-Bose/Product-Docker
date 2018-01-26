"""
This file implements the FrontDoorQueue class for adding a queue interface to
front door notifications.
"""

from Queue import Queue
import json
from CastleTestUtils.FrontDoorAPI.FrontDoorAPI import FrontDoorAPI

# pylint: disable=C0103

class FrontDoorQueue(FrontDoorAPI):
    """Subclass FrontDoorAPI to add a queue for notifications"""

    _q = Queue()

    def queueEmpty(self):
        """Return True if queue is empty"""
        return self._q.empty()

    def getNotification(self):
        """Return next notification in queue or None"""
        if not self._q.empty():
            return self._q.get()
        return None

    def printNotifications(self):
        """Print all notifications in queue"""
        while not self._q.empty():
            print self._q.get()

    def setQueueRemaining(self, numRemaining):
        """Consume events on the queue until only numRemaining are left"""
        while self._q.qsize() > numRemaining:
            self.getNotification()

    def _onMessage(self, ws, message):
        """
        This function detects if the message is a notification and adds it to the last_notificaiton
        """

        if '"method":"NOTIFY"' in message:
            self._last_notification = json.loads(message)
            self._q.put(self._last_notification)
        else:
            self._jsonIn.append(message)
            if self._msgCb(message, len(self._jsonIn) - 1):
                self.jsonInevent.set()
