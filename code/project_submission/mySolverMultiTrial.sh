#!/bin/bash
GAME="game.txt"
SOLN="solvedGame.txt"
UTIL_TEMP="utility.temp"
UTIL_TEMP2="utility.temp2"
RANGE_GRANULARITY=10

MAX_TARGETS=$1
MAX_RESOURCES=$2
MAX_RANGE_ITER=$3
max_range=$(echo "scale=1; $MAX_RANGE_ITER/$RANGE_GRANULARITY" | bc)
#MAX_RANGE=$3
#MAX_RANGE_ITER=expr $MAX_RANGE*$RANGE_GRANULARITY
#MAX_RANGE_ITER=$(echo $MAX_RANGE*$RANGE_GRANULARITY | bc)
#printf "MAX_RANGE = " printf $MAX_RANGE
#printf "MAX_RANGE_ITER = " printf $MAX_RANGE_ITER
NUM_TRIALS=$4

make
UTILS="utilities_$1,$2,$max_range,$4.csv"
# empty contents of file
>$UTILS;

printf "Model,# Targets,# Resources,Range,Trials...\n" >> $UTILS
for (( targets=1; targets <= $MAX_TARGETS; targets++ ))
do
  true_max_resources=$(echo $(( MAX_RESOURCES < targets ?
           MAX_RESOURCES : targets)))
  for (( resources=1; resources <= $true_max_resources; resources++ ))
  do
    for (( rangeIter=0; rangeIter <= $MAX_RANGE_ITER; rangeIter++ ))
    do
      range=$(echo "scale=1; $rangeIter/$RANGE_GRANULARITY" | bc)
      for model in Planar Non-planar
      do
        isPlanar=0
        planarStr="Planar"
        if [ "$model" = "$planarStr" ]
        then
          isPlanar=1
        fi
        >$UTIL_TEMP
        >$UTIL_TEMP2
        printf "$model,$targets,$resources,$range," >> $UTILS
        for (( trials=1; trials <= $NUM_TRIALS; trials++ ))
        do
          #echo "model=$model, #targets=$targets, #resources=$resources, range=$range, trial #$trials"
          ./spe -p $isPlanar -b $targets -d $resources -r $range -f $GAME
          ./myZeroSumSolver/runzerosum.sh "./myZeroSumSolver" $GAME $SOLN
          # append utility from solution file to csv file
          sed -n -e '/utility: /w '$UTIL_TEMP'' $SOLN
          sed -i -e 's/^.*[^0-9]\([0-9]*\.[0-9]*\).*$/\1/' $UTIL_TEMP
          #sed -i -e 's/^.*utility: \+.*$/\1/' $UTIL_TEMP
          tr -d "\n" < $UTIL_TEMP > $UTIL_TEMP2
          cat $UTIL_TEMP2 >> $UTILS
          printf "," >> "$UTILS"
        done
        printf "\n" >> "$UTILS"
      done
    done
  done
done
rm $UTIL_TEMP
rm $UTIL_TEMP2
