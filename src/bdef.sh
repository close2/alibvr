 # !/bin/bash  *
 
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
   echo "defined _IRQ_TASK_LIST_COUNTER_$j"
   echo "#  define _IRQ_TASK_LIST_COUNTER_$counter"
   echo "#  undef IRQ_TASK_LIST"
   echo "#  define ITL IrqTaskList$counter"
   echo "#  define IRQ_TASK_LIST _irqs::IrqTaskList$counter"
   counter=$counter-1
 done
 
 echo "#else"
 echo "#  define _IRQ_TASK_LIST_COUNTER_1"
 echo "#  undef IRQ_TASK_LIST"
 echo "#  define ITL IrqTaskList1"
 echo "#  define IRQ_TASK_LIST _irqs::TaskList1"
 echo "#endif"

