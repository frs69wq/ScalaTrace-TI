#!/bin/bash

for processes in 4 8 16 32 64 128
do
    for class in A B C
    do
	echo -n "Replaying $class-$processes... "
	(time smpirun --cfg="smpi/simulate_computation:no" \
	    --cfg="surf/precision:1e-6" \
	    --cfg="smpi/privatize_global_variables:yes" \
	    --cfg="smpi/running_power:4.18e9" \
	    -platform scripts/graphene_pmbs13.xml \
	    -np $processes \
	    -hostfile traces-new/td/lu/$class/$processes/hostfile.I \
	    replay/replay traces-new/td/lu/$class/$processes/I/0 > /tmp/file1) 1> /dev/null 2> /tmp/file2 
	
	realTime=$(grep "Time in seconds" traces-new/td/lu/$class/$processes/details.I | awk '{print $5}')
	simTime=$(grep "Execution time" /tmp/file1 | awk '{print $3}')
	runTime=$(grep "real" /tmp/file2 | awk '{print $2}')
	
	echo  "$class, $processes, $realTime, $simTime, $runTime" >> datasets/simulationsTD.csv
	echo "done"
    done
done
