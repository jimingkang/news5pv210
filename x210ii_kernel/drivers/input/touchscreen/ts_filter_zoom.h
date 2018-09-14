#ifndef __TS_FILTER_ZOOM_H__
#define __TS_FILTER_ZOOM_H__

#include "ts_filter.h"
#include <linux/kobject.h>

/*
 * Touchscreen zoom filter.
 *
 * Copyright (C) 2008,2009 by Openmoko, Inc.
 * Author: Nelson Castillo <arhuaco@freaks-unidos.net>
 *
 */

#define TS_FILTER_ZOOM_NCONSTANTS	1

struct ts_filter_zoom_configuration {
	/* Calibration constants. */
	int constants[TS_FILTER_ZOOM_NCONSTANTS];
	/* First coordinate. */
	int coord0;
	/* Second coordinate. */
	int coord1;

	/* Generic filter configuration. */
	struct ts_filter_configuration config;
};

extern const struct ts_filter_api ts_filter_zoom_api;

#endif
