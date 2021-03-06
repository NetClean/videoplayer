/*
 * SSG VideoPlayer
 *  Multi process video player for windows.
 *
 * Copyright (c) 2010-2015 Safer Society Group Sweden AB
 * All Rights Reserved.
 *
 * This file is part of SSG VideoPlayer.
 *
 * SSG VideoPlayer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * SSG VideoPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SSG VideoPlayer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TIMEHANDLER_H
#define TIMEHANDLER_H

#include <memory>

#include "IAudioDevice.h"

typedef std::shared_ptr<class TimeHandler> TimeHandlerPtr;

class TimeHandler
{
	public:
	virtual double GetTime() = 0;
	virtual double GetTimeWarp() = 0;
	virtual void Pause() = 0;
	virtual void Play() = 0;
	virtual bool GetPaused() = 0;
	virtual void SetTimeWarp(double tps) = 0;
	virtual void SetTime(double t) = 0;
	virtual void AddTime(double t) = 0;

	static TimeHandlerPtr Create(IAudioDevicePtr audioDevice);
};

#endif
