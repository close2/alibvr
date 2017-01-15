#ifndef FLAG_TO_SILENCE_COMPILER_WARNING // no guards!
// do not define this flag!

#include "macros.h"

#include "macro_counter.h"

// no guards!
#pragma push_macro("IRQ_TASKLIST_NAME")
#pragma push_macro("IRQ_TASK_F")
#pragma push_macro("REMINDER_NAME")

#pragma push_macro("ITL")

#define IRQ_TASKLIST_NAME CAT(IrqTaskListType, _COUNTER)
#define IRQ_TASK_F CAT(irq_task_f, _COUNTER)
#define REMINDER_NAME CAT(reminder, _COUNTER)


namespace _irqs {
  void IRQ_TASK_F(Irq _i) { NEW_IRQ_TASK(_i); }
  
  typedef typename _task_list::concat<IRQ_TASK_LIST, IrqTaskWrapper<IRQ_TASK_F, IRQ_NAME> >::task IRQ_TASKLIST_NAME;
  
  inline uint8_t you_must_include_register_irqs_h();
  static const uint8_t __attribute__ ((unused)) REMINDER_NAME = you_must_include_register_irqs_h();
}

#pragma push_macro("ITL")

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
   echo "defined _IRQ_TASK_LIST_COUNTER_$j"
   echo "#  define _IRQ_TASK_LIST_COUNTER_$counter"
   echo "#  undef IRQ_TASK_LIST"
   echo "#  define ITL IrqTaskList$counter"
   echo "#  define IRQ_TASK_LIST _irqs::IRQ::IrqTaskList$counter"
   counter=$counter-1
 done
 
 echo "#else"
 echo "#  define _IRQ_TASK_LIST_COUNTER_1"
 echo "#  undef IRQ_TASK_LIST"
 echo "#  define ITL IrqTaskList1"
 echo "#  define IRQ_TASK_LIST _irqs::IRQ::IrqTaskList1"
 echo "#endif"
 
 */

#if defined _IRQ_TASK_LIST_COUNTER_99
#  define _IRQ_TASK_LIST_COUNTER_100
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList100
#  define IRQ_TASK_LIST _irqs::IrqTaskList100
#elif defined _IRQ_TASK_LIST_COUNTER_98
#  define _IRQ_TASK_LIST_COUNTER_99
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList99
#  define IRQ_TASK_LIST _irqs::IrqTaskList99
#elif defined _IRQ_TASK_LIST_COUNTER_97
#  define _IRQ_TASK_LIST_COUNTER_98
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList98
#  define IRQ_TASK_LIST _irqs::IrqTaskList98
#elif defined _IRQ_TASK_LIST_COUNTER_96
#  define _IRQ_TASK_LIST_COUNTER_97
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList97
#  define IRQ_TASK_LIST _irqs::IrqTaskList97
#elif defined _IRQ_TASK_LIST_COUNTER_95
#  define _IRQ_TASK_LIST_COUNTER_96
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList96
#  define IRQ_TASK_LIST _irqs::IrqTaskList96
#elif defined _IRQ_TASK_LIST_COUNTER_94
#  define _IRQ_TASK_LIST_COUNTER_95
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList95
#  define IRQ_TASK_LIST _irqs::IrqTaskList95
#elif defined _IRQ_TASK_LIST_COUNTER_93
#  define _IRQ_TASK_LIST_COUNTER_94
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList94
#  define IRQ_TASK_LIST _irqs::IrqTaskList94
#elif defined _IRQ_TASK_LIST_COUNTER_92
#  define _IRQ_TASK_LIST_COUNTER_93
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList93
#  define IRQ_TASK_LIST _irqs::IrqTaskList93
#elif defined _IRQ_TASK_LIST_COUNTER_91
#  define _IRQ_TASK_LIST_COUNTER_92
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList92
#  define IRQ_TASK_LIST _irqs::IrqTaskList92
#elif defined _IRQ_TASK_LIST_COUNTER_90
#  define _IRQ_TASK_LIST_COUNTER_91
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList91
#  define IRQ_TASK_LIST _irqs::IrqTaskList91
#elif defined _IRQ_TASK_LIST_COUNTER_89
#  define _IRQ_TASK_LIST_COUNTER_90
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList90
#  define IRQ_TASK_LIST _irqs::IrqTaskList90
#elif defined _IRQ_TASK_LIST_COUNTER_88
#  define _IRQ_TASK_LIST_COUNTER_89
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList89
#  define IRQ_TASK_LIST _irqs::IrqTaskList89
#elif defined _IRQ_TASK_LIST_COUNTER_87
#  define _IRQ_TASK_LIST_COUNTER_88
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList88
#  define IRQ_TASK_LIST _irqs::IrqTaskList88
#elif defined _IRQ_TASK_LIST_COUNTER_86
#  define _IRQ_TASK_LIST_COUNTER_87
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList87
#  define IRQ_TASK_LIST _irqs::IrqTaskList87
#elif defined _IRQ_TASK_LIST_COUNTER_85
#  define _IRQ_TASK_LIST_COUNTER_86
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList86
#  define IRQ_TASK_LIST _irqs::IrqTaskList86
#elif defined _IRQ_TASK_LIST_COUNTER_84
#  define _IRQ_TASK_LIST_COUNTER_85
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList85
#  define IRQ_TASK_LIST _irqs::IrqTaskList85
#elif defined _IRQ_TASK_LIST_COUNTER_83
#  define _IRQ_TASK_LIST_COUNTER_84
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList84
#  define IRQ_TASK_LIST _irqs::IrqTaskList84
#elif defined _IRQ_TASK_LIST_COUNTER_82
#  define _IRQ_TASK_LIST_COUNTER_83
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList83
#  define IRQ_TASK_LIST _irqs::IrqTaskList83
#elif defined _IRQ_TASK_LIST_COUNTER_81
#  define _IRQ_TASK_LIST_COUNTER_82
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList82
#  define IRQ_TASK_LIST _irqs::IrqTaskList82
#elif defined _IRQ_TASK_LIST_COUNTER_80
#  define _IRQ_TASK_LIST_COUNTER_81
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList81
#  define IRQ_TASK_LIST _irqs::IrqTaskList81
#elif defined _IRQ_TASK_LIST_COUNTER_79
#  define _IRQ_TASK_LIST_COUNTER_80
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList80
#  define IRQ_TASK_LIST _irqs::IrqTaskList80
#elif defined _IRQ_TASK_LIST_COUNTER_78
#  define _IRQ_TASK_LIST_COUNTER_79
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList79
#  define IRQ_TASK_LIST _irqs::IrqTaskList79
#elif defined _IRQ_TASK_LIST_COUNTER_77
#  define _IRQ_TASK_LIST_COUNTER_78
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList78
#  define IRQ_TASK_LIST _irqs::IrqTaskList78
#elif defined _IRQ_TASK_LIST_COUNTER_76
#  define _IRQ_TASK_LIST_COUNTER_77
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList77
#  define IRQ_TASK_LIST _irqs::IrqTaskList77
#elif defined _IRQ_TASK_LIST_COUNTER_75
#  define _IRQ_TASK_LIST_COUNTER_76
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList76
#  define IRQ_TASK_LIST _irqs::IrqTaskList76
#elif defined _IRQ_TASK_LIST_COUNTER_74
#  define _IRQ_TASK_LIST_COUNTER_75
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList75
#  define IRQ_TASK_LIST _irqs::IrqTaskList75
#elif defined _IRQ_TASK_LIST_COUNTER_73
#  define _IRQ_TASK_LIST_COUNTER_74
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList74
#  define IRQ_TASK_LIST _irqs::IrqTaskList74
#elif defined _IRQ_TASK_LIST_COUNTER_72
#  define _IRQ_TASK_LIST_COUNTER_73
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList73
#  define IRQ_TASK_LIST _irqs::IrqTaskList73
#elif defined _IRQ_TASK_LIST_COUNTER_71
#  define _IRQ_TASK_LIST_COUNTER_72
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList72
#  define IRQ_TASK_LIST _irqs::IrqTaskList72
#elif defined _IRQ_TASK_LIST_COUNTER_70
#  define _IRQ_TASK_LIST_COUNTER_71
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList71
#  define IRQ_TASK_LIST _irqs::IrqTaskList71
#elif defined _IRQ_TASK_LIST_COUNTER_69
#  define _IRQ_TASK_LIST_COUNTER_70
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList70
#  define IRQ_TASK_LIST _irqs::IrqTaskList70
#elif defined _IRQ_TASK_LIST_COUNTER_68
#  define _IRQ_TASK_LIST_COUNTER_69
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList69
#  define IRQ_TASK_LIST _irqs::IrqTaskList69
#elif defined _IRQ_TASK_LIST_COUNTER_67
#  define _IRQ_TASK_LIST_COUNTER_68
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList68
#  define IRQ_TASK_LIST _irqs::IrqTaskList68
#elif defined _IRQ_TASK_LIST_COUNTER_66
#  define _IRQ_TASK_LIST_COUNTER_67
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList67
#  define IRQ_TASK_LIST _irqs::IrqTaskList67
#elif defined _IRQ_TASK_LIST_COUNTER_65
#  define _IRQ_TASK_LIST_COUNTER_66
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList66
#  define IRQ_TASK_LIST _irqs::IrqTaskList66
#elif defined _IRQ_TASK_LIST_COUNTER_64
#  define _IRQ_TASK_LIST_COUNTER_65
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList65
#  define IRQ_TASK_LIST _irqs::IrqTaskList65
#elif defined _IRQ_TASK_LIST_COUNTER_63
#  define _IRQ_TASK_LIST_COUNTER_64
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList64
#  define IRQ_TASK_LIST _irqs::IrqTaskList64
#elif defined _IRQ_TASK_LIST_COUNTER_62
#  define _IRQ_TASK_LIST_COUNTER_63
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList63
#  define IRQ_TASK_LIST _irqs::IrqTaskList63
#elif defined _IRQ_TASK_LIST_COUNTER_61
#  define _IRQ_TASK_LIST_COUNTER_62
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList62
#  define IRQ_TASK_LIST _irqs::IrqTaskList62
#elif defined _IRQ_TASK_LIST_COUNTER_60
#  define _IRQ_TASK_LIST_COUNTER_61
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList61
#  define IRQ_TASK_LIST _irqs::IrqTaskList61
#elif defined _IRQ_TASK_LIST_COUNTER_59
#  define _IRQ_TASK_LIST_COUNTER_60
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList60
#  define IRQ_TASK_LIST _irqs::IrqTaskList60
#elif defined _IRQ_TASK_LIST_COUNTER_58
#  define _IRQ_TASK_LIST_COUNTER_59
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList59
#  define IRQ_TASK_LIST _irqs::IrqTaskList59
#elif defined _IRQ_TASK_LIST_COUNTER_57
#  define _IRQ_TASK_LIST_COUNTER_58
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList58
#  define IRQ_TASK_LIST _irqs::IrqTaskList58
#elif defined _IRQ_TASK_LIST_COUNTER_56
#  define _IRQ_TASK_LIST_COUNTER_57
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList57
#  define IRQ_TASK_LIST _irqs::IrqTaskList57
#elif defined _IRQ_TASK_LIST_COUNTER_55
#  define _IRQ_TASK_LIST_COUNTER_56
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList56
#  define IRQ_TASK_LIST _irqs::IrqTaskList56
#elif defined _IRQ_TASK_LIST_COUNTER_54
#  define _IRQ_TASK_LIST_COUNTER_55
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList55
#  define IRQ_TASK_LIST _irqs::IrqTaskList55
#elif defined _IRQ_TASK_LIST_COUNTER_53
#  define _IRQ_TASK_LIST_COUNTER_54
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList54
#  define IRQ_TASK_LIST _irqs::IrqTaskList54
#elif defined _IRQ_TASK_LIST_COUNTER_52
#  define _IRQ_TASK_LIST_COUNTER_53
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList53
#  define IRQ_TASK_LIST _irqs::IrqTaskList53
#elif defined _IRQ_TASK_LIST_COUNTER_51
#  define _IRQ_TASK_LIST_COUNTER_52
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList52
#  define IRQ_TASK_LIST _irqs::IrqTaskList52
#elif defined _IRQ_TASK_LIST_COUNTER_50
#  define _IRQ_TASK_LIST_COUNTER_51
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList51
#  define IRQ_TASK_LIST _irqs::IrqTaskList51
#elif defined _IRQ_TASK_LIST_COUNTER_49
#  define _IRQ_TASK_LIST_COUNTER_50
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList50
#  define IRQ_TASK_LIST _irqs::IrqTaskList50
#elif defined _IRQ_TASK_LIST_COUNTER_48
#  define _IRQ_TASK_LIST_COUNTER_49
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList49
#  define IRQ_TASK_LIST _irqs::IrqTaskList49
#elif defined _IRQ_TASK_LIST_COUNTER_47
#  define _IRQ_TASK_LIST_COUNTER_48
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList48
#  define IRQ_TASK_LIST _irqs::IrqTaskList48
#elif defined _IRQ_TASK_LIST_COUNTER_46
#  define _IRQ_TASK_LIST_COUNTER_47
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList47
#  define IRQ_TASK_LIST _irqs::IrqTaskList47
#elif defined _IRQ_TASK_LIST_COUNTER_45
#  define _IRQ_TASK_LIST_COUNTER_46
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList46
#  define IRQ_TASK_LIST _irqs::IrqTaskList46
#elif defined _IRQ_TASK_LIST_COUNTER_44
#  define _IRQ_TASK_LIST_COUNTER_45
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList45
#  define IRQ_TASK_LIST _irqs::IrqTaskList45
#elif defined _IRQ_TASK_LIST_COUNTER_43
#  define _IRQ_TASK_LIST_COUNTER_44
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList44
#  define IRQ_TASK_LIST _irqs::IrqTaskList44
#elif defined _IRQ_TASK_LIST_COUNTER_42
#  define _IRQ_TASK_LIST_COUNTER_43
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList43
#  define IRQ_TASK_LIST _irqs::IrqTaskList43
#elif defined _IRQ_TASK_LIST_COUNTER_41
#  define _IRQ_TASK_LIST_COUNTER_42
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList42
#  define IRQ_TASK_LIST _irqs::IrqTaskList42
#elif defined _IRQ_TASK_LIST_COUNTER_40
#  define _IRQ_TASK_LIST_COUNTER_41
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList41
#  define IRQ_TASK_LIST _irqs::IrqTaskList41
#elif defined _IRQ_TASK_LIST_COUNTER_39
#  define _IRQ_TASK_LIST_COUNTER_40
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList40
#  define IRQ_TASK_LIST _irqs::IrqTaskList40
#elif defined _IRQ_TASK_LIST_COUNTER_38
#  define _IRQ_TASK_LIST_COUNTER_39
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList39
#  define IRQ_TASK_LIST _irqs::IrqTaskList39
#elif defined _IRQ_TASK_LIST_COUNTER_37
#  define _IRQ_TASK_LIST_COUNTER_38
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList38
#  define IRQ_TASK_LIST _irqs::IrqTaskList38
#elif defined _IRQ_TASK_LIST_COUNTER_36
#  define _IRQ_TASK_LIST_COUNTER_37
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList37
#  define IRQ_TASK_LIST _irqs::IrqTaskList37
#elif defined _IRQ_TASK_LIST_COUNTER_35
#  define _IRQ_TASK_LIST_COUNTER_36
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList36
#  define IRQ_TASK_LIST _irqs::IrqTaskList36
#elif defined _IRQ_TASK_LIST_COUNTER_34
#  define _IRQ_TASK_LIST_COUNTER_35
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList35
#  define IRQ_TASK_LIST _irqs::IrqTaskList35
#elif defined _IRQ_TASK_LIST_COUNTER_33
#  define _IRQ_TASK_LIST_COUNTER_34
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList34
#  define IRQ_TASK_LIST _irqs::IrqTaskList34
#elif defined _IRQ_TASK_LIST_COUNTER_32
#  define _IRQ_TASK_LIST_COUNTER_33
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList33
#  define IRQ_TASK_LIST _irqs::IrqTaskList33
#elif defined _IRQ_TASK_LIST_COUNTER_31
#  define _IRQ_TASK_LIST_COUNTER_32
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList32
#  define IRQ_TASK_LIST _irqs::IrqTaskList32
#elif defined _IRQ_TASK_LIST_COUNTER_30
#  define _IRQ_TASK_LIST_COUNTER_31
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList31
#  define IRQ_TASK_LIST _irqs::IrqTaskList31
#elif defined _IRQ_TASK_LIST_COUNTER_29
#  define _IRQ_TASK_LIST_COUNTER_30
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList30
#  define IRQ_TASK_LIST _irqs::IrqTaskList30
#elif defined _IRQ_TASK_LIST_COUNTER_28
#  define _IRQ_TASK_LIST_COUNTER_29
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList29
#  define IRQ_TASK_LIST _irqs::IrqTaskList29
#elif defined _IRQ_TASK_LIST_COUNTER_27
#  define _IRQ_TASK_LIST_COUNTER_28
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList28
#  define IRQ_TASK_LIST _irqs::IrqTaskList28
#elif defined _IRQ_TASK_LIST_COUNTER_26
#  define _IRQ_TASK_LIST_COUNTER_27
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList27
#  define IRQ_TASK_LIST _irqs::IrqTaskList27
#elif defined _IRQ_TASK_LIST_COUNTER_25
#  define _IRQ_TASK_LIST_COUNTER_26
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList26
#  define IRQ_TASK_LIST _irqs::IrqTaskList26
#elif defined _IRQ_TASK_LIST_COUNTER_24
#  define _IRQ_TASK_LIST_COUNTER_25
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList25
#  define IRQ_TASK_LIST _irqs::IrqTaskList25
#elif defined _IRQ_TASK_LIST_COUNTER_23
#  define _IRQ_TASK_LIST_COUNTER_24
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList24
#  define IRQ_TASK_LIST _irqs::IrqTaskList24
#elif defined _IRQ_TASK_LIST_COUNTER_22
#  define _IRQ_TASK_LIST_COUNTER_23
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList23
#  define IRQ_TASK_LIST _irqs::IrqTaskList23
#elif defined _IRQ_TASK_LIST_COUNTER_21
#  define _IRQ_TASK_LIST_COUNTER_22
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList22
#  define IRQ_TASK_LIST _irqs::IrqTaskList22
#elif defined _IRQ_TASK_LIST_COUNTER_20
#  define _IRQ_TASK_LIST_COUNTER_21
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList21
#  define IRQ_TASK_LIST _irqs::IrqTaskList21
#elif defined _IRQ_TASK_LIST_COUNTER_19
#  define _IRQ_TASK_LIST_COUNTER_20
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList20
#  define IRQ_TASK_LIST _irqs::IrqTaskList20
#elif defined _IRQ_TASK_LIST_COUNTER_18
#  define _IRQ_TASK_LIST_COUNTER_19
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList19
#  define IRQ_TASK_LIST _irqs::IrqTaskList19
#elif defined _IRQ_TASK_LIST_COUNTER_17
#  define _IRQ_TASK_LIST_COUNTER_18
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList18
#  define IRQ_TASK_LIST _irqs::IrqTaskList18
#elif defined _IRQ_TASK_LIST_COUNTER_16
#  define _IRQ_TASK_LIST_COUNTER_17
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList17
#  define IRQ_TASK_LIST _irqs::IrqTaskList17
#elif defined _IRQ_TASK_LIST_COUNTER_15
#  define _IRQ_TASK_LIST_COUNTER_16
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList16
#  define IRQ_TASK_LIST _irqs::IrqTaskList16
#elif defined _IRQ_TASK_LIST_COUNTER_14
#  define _IRQ_TASK_LIST_COUNTER_15
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList15
#  define IRQ_TASK_LIST _irqs::IrqTaskList15
#elif defined _IRQ_TASK_LIST_COUNTER_13
#  define _IRQ_TASK_LIST_COUNTER_14
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList14
#  define IRQ_TASK_LIST _irqs::IrqTaskList14
#elif defined _IRQ_TASK_LIST_COUNTER_12
#  define _IRQ_TASK_LIST_COUNTER_13
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList13
#  define IRQ_TASK_LIST _irqs::IrqTaskList13
#elif defined _IRQ_TASK_LIST_COUNTER_11
#  define _IRQ_TASK_LIST_COUNTER_12
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList12
#  define IRQ_TASK_LIST _irqs::IrqTaskList12
#elif defined _IRQ_TASK_LIST_COUNTER_10
#  define _IRQ_TASK_LIST_COUNTER_11
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList11
#  define IRQ_TASK_LIST _irqs::IrqTaskList11
#elif defined _IRQ_TASK_LIST_COUNTER_9
#  define _IRQ_TASK_LIST_COUNTER_10
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList10
#  define IRQ_TASK_LIST _irqs::IrqTaskList10
#elif defined _IRQ_TASK_LIST_COUNTER_8
#  define _IRQ_TASK_LIST_COUNTER_9
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList9
#  define IRQ_TASK_LIST _irqs::IrqTaskList9
#elif defined _IRQ_TASK_LIST_COUNTER_7
#  define _IRQ_TASK_LIST_COUNTER_8
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList8
#  define IRQ_TASK_LIST _irqs::IrqTaskList8
#elif defined _IRQ_TASK_LIST_COUNTER_6
#  define _IRQ_TASK_LIST_COUNTER_7
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList7
#  define IRQ_TASK_LIST _irqs::IrqTaskList7
#elif defined _IRQ_TASK_LIST_COUNTER_5
#  define _IRQ_TASK_LIST_COUNTER_6
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList6
#  define IRQ_TASK_LIST _irqs::IrqTaskList6
#elif defined _IRQ_TASK_LIST_COUNTER_4
#  define _IRQ_TASK_LIST_COUNTER_5
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList5
#  define IRQ_TASK_LIST _irqs::IrqTaskList5
#elif defined _IRQ_TASK_LIST_COUNTER_3
#  define _IRQ_TASK_LIST_COUNTER_4
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList4
#  define IRQ_TASK_LIST _irqs::IrqTaskList4
#elif defined _IRQ_TASK_LIST_COUNTER_2
#  define _IRQ_TASK_LIST_COUNTER_3
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList3
#  define IRQ_TASK_LIST _irqs::IrqTaskList3
#elif defined _IRQ_TASK_LIST_COUNTER_1
#  define _IRQ_TASK_LIST_COUNTER_2
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList2
#  define IRQ_TASK_LIST _irqs::IrqTaskList2
#else
#  define _IRQ_TASK_LIST_COUNTER_1
#  undef IRQ_TASK_LIST
#  define ITL IrqTaskList1
#  define IRQ_TASK_LIST _irqs::IrqTaskList1
#endif

namespace _irqs {
  typedef IRQ_TASKLIST_NAME ITL;
}

#undef NEW_IRQ_TASK

#pragma pop_macro("ITL")
#pragma pop_macro("IRQ_TASKLIST_NAME")
#pragma pop_macro("IRQ_TASK_F")
#pragma pop_macro("REMINDER_NAME")

#endif
