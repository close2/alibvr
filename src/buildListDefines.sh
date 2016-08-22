#!/bin/bash

# This script has been used to create part of internal/tasks.h
# It is no longer needed and could be removed.
 
declare -i counter
counter=$1

FIRST=true

declare -i i
declare -i j
declare -i k

while [ $counter -gt 1 ]
do
	j=$counter-1
	
	if [ $FIRST = "true" ]
	then
		printf '#if '
		FIRST=false
	else
		printf '#elif '
	fi
	echo "defined _TASK_LIST_COUNTER_$j"
	echo "#  define _TASK_LIST_COUNTER_$counter"
	echo "#  undef TASK_LIST"
	echo "#  define TL TaskList$counter"
	echo "#  define TASK_LIST _tasks::TaskList$counter"
	counter=$counter-1
done

echo "#else"
echo "#  define _TASK_LIST_COUNTER_1"
echo "#  undef TASK_LIST"
echo "#  define TL TaskList1"
echo "#  define TASK_LIST _tasks::TaskList1"
echo "#endif"

