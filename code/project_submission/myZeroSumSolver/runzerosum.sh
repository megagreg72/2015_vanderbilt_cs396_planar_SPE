#!/bin/bash
#echo "running runzerosum.sh"
SOLVER_PATH=$1;
IN=$2;
OUT=$3;

# must match with .dat in amplCommands
DAT="./zeroSum.ampl.dat";

# need bash for this line, not sh
firstLine=$(head -n 1 $IN);

tokens=($firstLine);
rows=${tokens[0]};
columns=${tokens[1]};

$SOLVER_PATH/txtToAmplDat.sh $SOLVER_PATH $IN $DAT
$SOLVER_PATH/ampl.sh $SOLVER_PATH $rows $OUT
