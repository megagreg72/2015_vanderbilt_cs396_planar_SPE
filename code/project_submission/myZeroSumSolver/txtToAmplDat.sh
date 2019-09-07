#!/bin/bash
#echo "running txtToAmplDat.sh"
SOLVER_PATH=$1
TXT="./$2"
TEMP="$1/$2.temp"
TEMP2="$1/$2.temp2"
DAT="$1/$3";
rm $DAT;

# need bash for this line to work, not sh
firstLine=$(head -n 1 $TXT);
tokens=( $firstLine );
rows=${tokens[0]};
columns=${tokens[1]};

cp $TXT $TEMP;

# delete first 2 lines
sed -i -e '1,2d' $TEMP
# delete everything after first matrix
# (nash solver uses 2 matrices)
rowsPlusOne=$((rows+1))
sed -i -e ''$rowsPlusOne',$d' $TEMP

# label the rows (using line number)
nl $TEMP > $TEMP2
# awk '{printf "%d\t%s\n", NR, $0}' $TEMP > $TEMP;

echo "data;" >> $DAT;
printf "param Rows := " >> $DAT; printf $rows >> $DAT; printf ";\n" >> $DAT;
printf "param Columns := " >> $DAT; printf $columns >> $DAT; printf ";\n" >> $DAT;
echo "param P2_Rewards:" >> $DAT;

# label the columns
for i in $(seq $columns)
do
  printf "$i " >> $DAT
done


printf ":=\n" >> $DAT;

cat "$TEMP2" >> "$DAT";

sed -i -e 's///g' $DAT;
# add semicolon at end of last line
sed -i -e '${s/$/;/}' $DAT;

rm $TEMP;
