#!/bin/bash
function get_pid() 
{ 
        ps -ef | grep -v grep | grep multip| awk '{print $2}' 
} 

proc_pid=''
while [ -z "$proc_pid" ]
do
proc_pid=$(get_pid)
done;    /usr/local/bin/gdb -p $proc_pid -q -x scripts/watch_ref.py
