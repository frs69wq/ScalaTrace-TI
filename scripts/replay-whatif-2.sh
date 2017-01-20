#!/bin/bash

for processes in 16 32
do
    for class in C
    do
	for cabinets in 0 1 2 4
	do
	    for run in I II III IV V
	    do
		echo -n "Replaying $class-$processes-$run for $cabinets cabinets... "

		(time smpirun --cfg="smpi/simulate_computation:no" \
		    --cfg="surf/precision:1e-6" \
		    --cfg="smpi/privatize_global_variables:yes" \
		    --cfg="smpi/running_power:4.18e9" \
		    -platform scripts/graphene_pmbs13.xml \
		    -np $processes \
		    -hostfile traces-new/td/lu/$class/$processes/simulation-hostfile.$cabinets-cabinets \
		    replay/replay traces-new/td/lu/$class/$processes/$run/0 > /tmp/file1) 1> /dev/null 2> /tmp/file2 
		
		realTime=$(awk '/Time in seconds/ {print $5}' traces-new/td/lu/$class/$processes/details.$run)
		simTime=$(awk '/Execution time/ {print $3}' /tmp/file1)
		runTime=$(awk '/real/ {print $2}' /tmp/file2)
		
		case $run in
		    "I") run=1;;
		    "II") run=2;;
		    "III") run=3;;
		    "IV") run=4;;
		    "V") run=5;;
		    *) ;;
		esac

		echo "$class, $processes, $cabinets, $realTime, RealTime, $run" >> datasets/simulationsTD-whatif-2.csv
		echo "$class, $processes, $cabinets, $simTime, SimTime, $run" >> datasets/simulationsTD-whatif-2.csv
		echo "$class, $processes, $cabinets, $runTime, RunTime, $run" >> datasets/simulationsTD-whatif-2.csv

		echo "done"
	    done
	done
    done
done
