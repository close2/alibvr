#avr-gdb --eval-command="target remote localhost:1234" --eval-command="file adapter.obj" --eval-command="load adapter.obj" --interpreter=mi2 -quiet
avr-gdb -cd=/home/cl/projects/alibvr/examples/src/ $@
