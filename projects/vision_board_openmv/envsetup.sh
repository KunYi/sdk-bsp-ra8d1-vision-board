#!/bin/bash

export RTT_ROOT=${PWD}/rt-thread
export ENV_ROOT=${HOME}/.env
export BSP_ROOT=${PWD}
export RTT_CC='gcc'
#export RTT_EXEC_PATH='/usr/bin'
export RTT_EXEC_PATH='/home/kunyi/renesas/ra/e2studio_v2024-01.1_fsp_v5.2.0/toolchains/gcc_arm/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi/bin'

rtt_dir='../../rt-thread'
lib_dir='../../libraries'

if [ ! -L "rt-thread" ]; then
	if [ -d $rtt_dir ]; then
		ln -s $rtt_dir ./rt-thread
	fi
fi

if [ ! -L "libraries" ]; then
	if [ -d $lib_dir ]; then
		ln -s $lib_dir ./libraries
	fi
fi

pyocd_flash() {
	pyocd load --target=r7fa8d1bh --frequency=10M rtthread.elf
}
