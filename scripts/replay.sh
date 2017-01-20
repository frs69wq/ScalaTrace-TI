#!/bin/bash

for processes in 8 16 32 64 128
do
    for class in B C
    do
	for run in I II III IV V
	do
	    echo -n "Replaying $class-$processes-$run... "

	    (time smpirun --cfg="smpi/simulate_computation:no" \
		--cfg="surf/precision:1e-6" \
		--cfg="smpi/privatize_global_variables:yes" \
		--cfg="smpi/running_power:4.18e9" \
		-platform scripts/graphene_pmbs13.xml \
		-np $processes \
		-hostfile traces-new/td/lu/$class/$processes/hostfile.$run \
		replay/replay traces-new/td/lu/$class/$processes/$run/0 > /tmp/file1) 1> /dev/null 2> /tmp/file2 
	    
	    realTime=$(grep "Time in seconds" traces-new/td/lu/$class/$processes/details.$run | awk '{print $5}')
	    simTime=$(grep "Execution time" /tmp/file1 | awk '{print $3}')
	    runTime=$(grep "real" /tmp/file2 | awk '{print $2}')
	    
	    case $run in
		"I") run=1;;
		"II") run=2;;
		"III") run=3;;
		"IV") run=4;;
		"V") run=5;;
		*) ;;
	    esac

	    echo "$class, $processes, $realTime, RealTime, $run" >> datasets/simulationsTDruns.csv
	    echo "$class, $processes, $simTime, SimTime, $run" >> datasets/simulationsTDruns.csv
	    echo "$class, $processes, $runTime, RunTime, $run" >> datasets/simulationsTDruns.csv

	    echo "done"
	done
    done
done
