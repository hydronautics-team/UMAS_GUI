#!/bin/bash
set -e

WORKDIR="/UMAS_GUI/build"

cd $WORKDIR

# Проброс DISPLAY
export DISPLAY=$DISPLAY

# Запуск приложения
./UMAS_GUI
