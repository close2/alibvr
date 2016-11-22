#ifndef FLAG_TO_SILENCE_COMPILER_WARNING // no guards!
// do not define this flag!

#pragma push_macro("MODBUS_DATA_LIST_NAME_GEN")
#pragma push_macro("_MODBUS_DATA_LIST_NAME_GEN")
#define _MODBUS_DATA_LIST_NAME_GEN(newTask) ModbusDataList##newTask
#define MODBUS_DATA_LIST_NAME_GEN(newTask) _MODBUS_DATA_LIST_NAME_GEN(newTask)

namespace _modbus_data {
  typedef typename _task_list::concat<MODBUS_DATA_LIST, NEW_MODBUS_DATA>::task MODBUS_DATA_LIST_NAME_GEN(NEW_MODBUS_DATA);
}

#pragma push_macro("TL")
/*
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
   echo "defined _MODBUS_DATA_LIST_COUNTER_$j"
   echo "#  define _MODBUS_DATA_LIST_COUNTER_$counter"
   echo "#  undef MODBUS_DATA_LIST"
   echo "#  define TL ModbusDataList$counter"
   echo "#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList$counter"
   counter=$counter-1
 done

 echo "#else"
 echo "#  define _MODBUS_DATA_LIST_COUNTER_1"
 echo "#  undef MODBUS_DATA_LIST"
 echo "#  define TL ModbusDataList1"
 echo "#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList1"
 echo "#endif"

 */

#if defined _MODBUS_DATA_LIST_COUNTER_19
#  define _MODBUS_DATA_LIST_COUNTER_20
#  undef MODBUS_DATA_LIST
#  define TL ModbusDataList20
#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList20
#elif defined _MODBUS_DATA_LIST_COUNTER_18
#  define _MODBUS_DATA_LIST_COUNTER_19
#  undef MODBUS_DATA_LIST
#  define TL ModbusDataList19
#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList19
#elif defined _MODBUS_DATA_LIST_COUNTER_17
#  define _MODBUS_DATA_LIST_COUNTER_18
#  undef MODBUS_DATA_LIST
#  define TL ModbusDataList18
#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList18
#elif defined _MODBUS_DATA_LIST_COUNTER_16
#  define _MODBUS_DATA_LIST_COUNTER_17
#  undef MODBUS_DATA_LIST
#  define TL ModbusDataList17
#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList17
#elif defined _MODBUS_DATA_LIST_COUNTER_15
#  define _MODBUS_DATA_LIST_COUNTER_16
#  undef MODBUS_DATA_LIST
#  define TL ModbusDataList16
#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList16
#elif defined _MODBUS_DATA_LIST_COUNTER_14
#  define _MODBUS_DATA_LIST_COUNTER_15
#  undef MODBUS_DATA_LIST
#  define TL ModbusDataList15
#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList15
#elif defined _MODBUS_DATA_LIST_COUNTER_13
#  define _MODBUS_DATA_LIST_COUNTER_14
#  undef MODBUS_DATA_LIST
#  define TL ModbusDataList14
#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList14
#elif defined _MODBUS_DATA_LIST_COUNTER_12
#  define _MODBUS_DATA_LIST_COUNTER_13
#  undef MODBUS_DATA_LIST
#  define TL ModbusDataList13
#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList13
#elif defined _MODBUS_DATA_LIST_COUNTER_11
#  define _MODBUS_DATA_LIST_COUNTER_12
#  undef MODBUS_DATA_LIST
#  define TL ModbusDataList12
#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList12
#elif defined _MODBUS_DATA_LIST_COUNTER_10
#  define _MODBUS_DATA_LIST_COUNTER_11
#  undef MODBUS_DATA_LIST
#  define TL ModbusDataList11
#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList11
#elif defined _MODBUS_DATA_LIST_COUNTER_9
#  define _MODBUS_DATA_LIST_COUNTER_10
#  undef MODBUS_DATA_LIST
#  define TL ModbusDataList10
#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList10
#elif defined _MODBUS_DATA_LIST_COUNTER_8
#  define _MODBUS_DATA_LIST_COUNTER_9
#  undef MODBUS_DATA_LIST
#  define TL ModbusDataList9
#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList9
#elif defined _MODBUS_DATA_LIST_COUNTER_7
#  define _MODBUS_DATA_LIST_COUNTER_8
#  undef MODBUS_DATA_LIST
#  define TL ModbusDataList8
#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList8
#elif defined _MODBUS_DATA_LIST_COUNTER_6
#  define _MODBUS_DATA_LIST_COUNTER_7
#  undef MODBUS_DATA_LIST
#  define TL ModbusDataList7
#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList7
#elif defined _MODBUS_DATA_LIST_COUNTER_5
#  define _MODBUS_DATA_LIST_COUNTER_6
#  undef MODBUS_DATA_LIST
#  define TL ModbusDataList6
#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList6
#elif defined _MODBUS_DATA_LIST_COUNTER_4
#  define _MODBUS_DATA_LIST_COUNTER_5
#  undef MODBUS_DATA_LIST
#  define TL ModbusDataList5
#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList5
#elif defined _MODBUS_DATA_LIST_COUNTER_3
#  define _MODBUS_DATA_LIST_COUNTER_4
#  undef MODBUS_DATA_LIST
#  define TL ModbusDataList4
#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList4
#elif defined _MODBUS_DATA_LIST_COUNTER_2
#  define _MODBUS_DATA_LIST_COUNTER_3
#  undef MODBUS_DATA_LIST
#  define TL ModbusDataList3
#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList3
#elif defined _MODBUS_DATA_LIST_COUNTER_1
#  define _MODBUS_DATA_LIST_COUNTER_2
#  undef MODBUS_DATA_LIST
#  define TL ModbusDataList2
#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList2
#else
#  define _MODBUS_DATA_LIST_COUNTER_1
#  undef MODBUS_DATA_LIST
#  define TL ModbusDataList1
#  define MODBUS_DATA_LIST _modbus_data::ModbusDataList1
#endif

namespace _modbus_data {
  typedef MODBUS_DATA_LIST_NAME_GEN(NEW_MODBUS_DATA) TL;
}

#undef NEW_MODBUS_DATA

#pragma pop_macro("MODBUS_DATA_LIST_NAME_GEN")
#pragma pop_macro("_MODBUS_DATA_LIST_NAME_GEN")
#pragma pop_macro("TL")

#endif
