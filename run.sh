# In order to generate proper output, run this script like shown here:
# (./run.sh 1 parallel) |& tee parallel
# (./run.sh 2 serial) |& tee serial

export TIMEFORMAT=%R
for i in {1..31}; do 
    X=2^$i
    P=$(echo $X | bc)
    echo "Test For $P"
    (time ./lab3 $P $1)
    echo "<<<<DONE>>>>\n"
done