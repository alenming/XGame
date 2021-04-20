#!/bin/sh
android update project -p .

if [ $? != 0 ]
then exit
fi


ant debug

if [ $? != 0 ]
then
  exit
fi
