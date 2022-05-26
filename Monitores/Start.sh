#!/bin/bash
gnome-terminal -- ./monitor temperatura temperatura.cfg 2051
gnome-terminal -- ./monitor oxigeno oxigeno.cfg 2055
gnome-terminal -- ./monitor ph ph.cfg 2059
sleep 2
#gnome-terminal -- ./checker temperatura temperatura.cfg 2051
#gnome-terminal -- ./checker oxigeno oxigeno.cfg 2055
#gnome-terminal -- ./checker ph ph.cfg 2059
