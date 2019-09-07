#!/bin/sh
#echo "running ampl.sh"
SOLVER_PATH=$1
rows=$2
OUT=$3

cp $SOLVER_PATH/amplCommands $SOLVER_PATH/amplCommandsCompiled
# replace instances of the string 'SOLVER_PATH' in file amplCommand
# with the actual $SOLVER_PATH
#echo "compiling amplCommands"
sed -i -e  "s@SOLVER_PATH@$SOLVER_PATH@g" $SOLVER_PATH/amplCommandsCompiled

# $SOLVER_PATH/ampl < $SOLVER_PATH/amplCommandsCompiled | tail -$rows > $OUT;
$SOLVER_PATH/ampl < $SOLVER_PATH/amplCommandsCompiled > $OUT;

# rm $SOLVER_PATH/amplCommandsCompiled
