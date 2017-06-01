/*
 *  This small demo sends a simple sinusoidal wave to your speakers.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <errno.h>
#include <getopt.h>
#include "../include/alsa/asoundlib.h"
#include <sys/time.h>
#include <math.h>

extern "C" int playAlert (char *);
extern "C" int stopAlert ();
