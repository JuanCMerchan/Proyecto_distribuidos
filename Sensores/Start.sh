#!/bin/bash
gnome-terminal -- ./publisher
gnome-terminal -- ./sensor temperatura 1 temperatura.cfg
gnome-terminal -- ./sensor oxigeno 2 oxigeno.cfg
gnome-terminal -- ./sensor ph 3 ph.cfg
