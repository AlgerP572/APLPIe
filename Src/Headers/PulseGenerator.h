/*
 * PulseGenerator.h:
 *	Another Peripheral Library for the raspberry PI.
 *	Copyright (c) 2019 Alger Pike
 ***********************************************************************
 * This file is part of APLPIe:
 *	https://github.com/AlgerP572/APLPIe
 *
 *    APLPIe is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    APLPIe is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with APLPIe.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */
#pragma once

#include "Device.h"
#include "Clock.h"
#include "Gpio.h"
#include "Dma.h"
#include "DmaMemory.h"
#include "Pwm.h"

#include <vector>

struct Pulse
{
	PinState State;
	uint32_t Duration; // Duration is defined by the clocks current clock frequency
};

struct PulseTrain
{
	uint32_t Pin; // Pin or pins to output the pulse train on.
	std::vector<Pulse> Timing;

	PulseTrain(uint32_t pin) :
		Pin(pin)
	{
	}

	void Add(PinState pinState, uint32_t duration)
	{
		Pulse segment;
		segment.State = pinState;
		segment.Duration = duration;
		Add(segment);
	}
	
	void Add(Pulse& pulse)
	{
		Timing.emplace_back(pulse);
	}
};


class PulseGenerator : public Device
{
private:
	Gpio& _gpio;
	Dma& _dma;
	Pwm& _pwm;
	Clock& _clock;
	uint32_t _bufferSyncPin;
	uint8_t _numTransferPages;
	uint32_t _numTransferFramesPerPage;
	uint32_t _numControlBlocksPerPage;
	uint32_t _clockCycle;

	std::vector<PulseTrain> _pulseTracks;
	std::vector<DmaMem_t*> _buffer0Pages;
	std::vector<DmaMem_t*> _buffer1Pages;

	std::vector<DmaMem_t*> _controlBlock0Pages;
	std::vector<DmaMem_t*> _controlBlock1Pages;

	static void SyncPinIsr(void* arg);

	uint32_t ConfigureBuffer0(uint32_t startingClock);
	uint32_t ConfigureBuffer1(uint32_t startingClock);

	void ConfigureControlBlocks0(uint32_t numControlBlocks);
	void ConfigureControlBlocks1(uint32_t numControlBlocks);

public:
	PulseGenerator(Gpio& gpio, Dma& dma, Pwm& pwm, Clock& clock, uint32_t bufferSyncPin, uint8_t numBufferPages);
	void virtual SysInit(void);
	void virtual SysUninit(void);
	void Add(PulseTrain& pulseTrain);
	void Start();
	void WriteSyncPinState(PinState state);
};