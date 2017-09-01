# -*- coding: utf-8 -*-

# Copyright (C) 2007-2014 Clement Lefebvre <root@linuxmint.com>
# Copyright (C) 2015 Martin Wimpress <code@ubuntu-ukui.org>
# Copyright (C) 2016,Tianjin KYLIN Information Technology Co., Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the
# Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.

import gi
import threading
gi.require_version("Gtk", "3.0")

from Xlib.display import Display
from Xlib import X, error
from gi.repository import Gtk, Gdk, GObject, GLib

class PointerMonitor(GObject.GObject, threading.Thread):
    __gsignals__ = {
        'activate': (GObject.SignalFlags.RUN_LAST, None, ()),
    }

    def __init__(self):
        try:
            GObject.GObject.__init__ (self)
            threading.Thread.__init__ (self)
            self.setDaemon (True)
            self.display = Display()
            self.root = self.display.screen().root
            self.windows = []
        except Exception, cause:
            print "init pointerMonitor error :\n", str(cause)
            self.display = None
            return None

    # Receives GDK windows
    def addWindowToMonitor(self, window):
        if self.display == None:
            return
        self.windows.append(window)

    def grabPointer(self):
        if self.display == None:
            return
        self.root.grab_button(X.AnyButton, X.AnyModifier, True, X.ButtonPressMask, X.GrabModeSync, X.GrabModeAsync, 0, 0)
        self.display.flush()

    def ungrabPointer(self):
        if self.display == None:
            return
        self.root.ungrab_button(X.AnyButton, X.AnyModifier)
        self.display.flush()

    def idle(self):
        self.emit("activate")
        return False

    def activate(self):
        GLib.idle_add(self.run)

    def run(self):
        if self.display == None:
            return
        self.running = True
        while self.running:
            event = self.display.next_event()
            try:
                if event.type == X.ButtonPress:
                    # Check if pointer is inside monitored windows
                    for w in self.windows:
                        if Gtk.check_version (3, 20, 0) is None:
                            pdevice = Gdk.Display.get_default().get_default_seat().get_pointer()
                        else:
                            pdevice = Gdk.Display.get_default().get_device_manager().get_client_pointer()
                        p = self.get_window().get_device_position(pdevice)
                        g = self.get_size()
                        if p.x >= 0 and p.y >= 0 and p.x <= g.width and p.y <= g.height:
                            break
                    else:
                        # Is outside, so activate
                        GLib.idle_add(self.idle)
                    self.display.allow_events(X.ReplayPointer, event.time)
                else:
                    self.display.allow_events(X.ReplayPointer, X.CurrentTime)
            except Exception as e:
                print "Unexpected error: " + str(e)

    def stop(self):
        if self.display == None:
            return
        self.running = False
        self.root.ungrab_button(X.AnyButton, X.AnyModifier)
        self.display.close()
