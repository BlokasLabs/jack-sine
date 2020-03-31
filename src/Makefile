#        Copyright (C) 2020 Blokas Labs, based on midisine by Ian Esten

#        This program is free software; you can redistribute it and/or modify
#        it under the terms of the GNU General Public License as published by
#        the Free Software Foundation; either version 2 of the License, or
#        (at your option) any later version.

#        This program is distributed in the hope that it will be useful,
#        but WITHOUT ANY WARRANTY; without even the implied warranty of
#        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#        GNU General Public License for more details.

#        You should have received a copy of the GNU General Public License
#        along with this program; if not, write to the Free Software
#        Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

PREFIX = /usr/local

jack_sine: jack_sine.c
	gcc jack_sine.c -o jack_sine -ljack -lm

install: jack_sine
	@install -d $(DESTDIR)$(PREFIX)/bin
	@install jack_sine $(DESTDIR)$(PREFIX)/bin/

clean:
	rm -f jack_sine

.PHONY: clean

all: jack_sine
