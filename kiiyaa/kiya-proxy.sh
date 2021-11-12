#!/bin/sh
name=${0##*/}
if [ $name == "kiya-proxy.sh" ]; then
	if [ $1 ]; then
		kiya /usr/local/kiiyaa/source $1 :main ${@:2}
	else
		echo "$0 <kiya> ..."
	fi
else
	kiya /usr/local/kiiyaa/source $name :main $*
fi