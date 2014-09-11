/*
* initial test for a modification of wavemon
* The intent is to read frequency data from the wifi connector
* in order to implement an essentially COTS wifi-based gesture
* recognition engine. A freqmon, if you will.
*
* Copyright (c) 2014-2015 Jeremy Tinker
* freqmon is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation; either version 2, or (at your option) any later
* version.
*
* freqmon is distributed in the hope that it will be useful, but WITHOUT A$
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License along
* with wavemon; see the file COPYING.  If not, write to the Free Software
* Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/*
* NEEDED (from looking at wavemon code):
* socket connection to wlan0
*	more if there's a wlan1...n (something for the future?)
* thread to report information as it is acquired
*	timer to handle sample reading
*	method to report samples
*	Eventually, load it to a GCTP (using TCP/UDP?) and connect to server
* thread(s) to handle sampling (eventually three threads to sample from each connection)
* 	timer to handle sampling timing
* 	method to do sampling
* main thread to handle real-time reporting
*	should also scan interfaces to connect to the right interface
*	handle all the setup stuff. . .
*
* TODO:
*	setup stuff:
*		create and initialize a timer
*		create a buffer for the readings
*		check that wlan0 is up, start it if not
*		initialize connection to server(s) (eventually)
*		create a sampler instance
*		initialize the sampler
*			check that wlan0 is up, start it if not
*		connect the sampler to the sample buffer
*		connect the sampler to the timer
*		start the scanner and the sampler running
*			with a cut-off time
*	run stuff:
*		sample thread:
*			while the cut-off time has not been reached,
*				and wlan0 is still up
*			read frequency and power from the interface 
*			add frequency and power datum to the sample buffer
*		report thread:
*			while the cut-off time has not been reached,
*				and wlan0 is still up
*			read frequency and power from the sample buffer
*			report frequency and power data to stdout
*/

