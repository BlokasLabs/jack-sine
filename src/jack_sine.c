/*
	Copyright (C) 2020 Blokas Labs, based on midisine by Ian Esten

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <inttypes.h>

#include <jack/jack.h>

jack_port_t *output_port;
jack_default_audio_sample_t ramp = 0.0;
jack_default_audio_sample_t amplitude = 0.3f;
jack_default_audio_sample_t frequency = 440.0f;
jack_nframes_t samplingRate = 0;

jack_client_t *client;

static void signal_handler(int sig)
{
	jack_client_close(client);
	fprintf(stderr, "signal received, exiting ...\n");
	exit(0);
}

#if 0
static void calc_note_frqs(jack_default_audio_sample_t srate)
{
	int i;
	for (i = 0; i < 128; i++)
	{
		note_frqs[i] = (2.0 * 440.0 / 32.0) * pow(2, (((jack_default_audio_sample_t)i - 9.0) / 12.0)) / srate;
	}
}
#endif

static int process(jack_nframes_t nframes, void *arg)
{
	if (!samplingRate)
		return -EINVAL;

	int i;
	jack_default_audio_sample_t *out = (jack_default_audio_sample_t *)jack_port_get_buffer(output_port, nframes);
	for (i = 0; i < nframes; i++)
	{
		ramp += frequency / samplingRate;
		ramp = (ramp > 1.0) ? ramp - 2.0 : ramp;
		out[i] = amplitude * sin(2 * M_PI*ramp);
	}
	return 0;
}

static int srate(jack_nframes_t nframes, void *arg)
{
	printf("the sample rate is now %" PRIu32 "/sec\n", nframes);
	samplingRate = nframes;
	return 0;
}

static void jack_shutdown(void *arg)
{
	fprintf(stderr, "JACK shut down, exiting ...\n");
	exit(1);
}

int main(int argc, char **argv)
{
	if (argc != 3 && argc != 4)
	{
		printf(
			"Usage: jack_sine frequency amplitude [-n]\n"
			"\n"
			"\tfrequency - a value between 0.0 - 20000.0\n"
			"\tamplitude - a value between 0.0 - 1.0\n"
			"\t-n        - don't autoconnect to playback ports (optional)\n"
			"\n"
			"The order of the arguments is important!\n"
			"\n"
			"Version: 1.0.0\n"
		);
		return 0;
	}

	bool autoConnect = true;
	if (argc == 4)
	{
		if (strcmp(argv[3], "-n") == 0)
		{
			autoConnect = false;
		}
		else
		{
			fprintf(stderr, "Unknown 3rd argument!\n");
			return 1;
		}
	}

	if (sscanf(argv[1], "%f", &frequency) != 1 || sscanf(argv[2], "%f", &amplitude) != 1)
	{
		fprintf(stderr, "Failed parsing the arguments!\n");
		return 1;
	}

	if (frequency < 0.0f || frequency > 20000.0f)
	{
		fprintf(stderr, "Frequency outside range!\n");
		return 1;
	}

	if (amplitude < 0.0f || amplitude > 1.0f)
	{
		fprintf(stderr, "Amplitude outside range!\n");
		return 1;
	}

	if ((client = jack_client_open("jack_sine", JackNullOption, NULL)) == 0)
	{
		fprintf(stderr, "JACK server not running?\n");
		return 1;
	}

	jack_set_process_callback(client, process, 0);

	jack_set_sample_rate_callback(client, srate, 0);

	jack_on_shutdown(client, jack_shutdown, 0);

	output_port = jack_port_register(client, "audio_out", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

	if (jack_activate(client))
	{
		fprintf(stderr, "cannot activate client");
		return 1;
	}

	if (autoConnect)
	{
		jack_connect(client, jack_port_name(output_port), "system:playback_1");
		jack_connect(client, jack_port_name(output_port), "system:playback_2");
	}

	/* install a signal handler to properly quits jack client */
#ifndef WIN32
	signal(SIGQUIT, signal_handler);
	signal(SIGHUP, signal_handler);
#endif
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);

	/* run until interrupted */
	while (1) {
#ifdef WIN32
		Sleep(1 * 1000);
#else
		sleep(1);
#endif
	}
	jack_client_close(client);
	exit(0);
}
