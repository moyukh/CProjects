#!/usr/local/bin/bash

sixteenk=16384


total=0

# test 1\
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
export LD_LIBRARY_PATH
./test1 2>&1 | tee out
passed=$?
heapsize_raw=`cat out | grep heap | awk '{print $2}'`
heapsize_inK=`echo $heapsize_raw | sed 's/K//' `
heapsize=$(expr $heapsize_inK \* 1024)

if [ "$heapsize" = "" ]; then heapsize=0; fi;
echo "expected: 0 < heapsize <= $sixteenk ; heapsize was $heapsize" 
if [ $passed -eq 0 -a "$heapsize" -a $heapsize -gt 0 -a $heapsize -le $sixteenk ];
then
  echo ">>> test1 passed";
  total=`expr $total + 10`;
else
  echo ">>> test1 failed";
fi


# test 2
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
export LD_LIBRARY_PATH
./test2 2>&1 | tee out
passed=$?
heapsize_raw=`cat out | grep heap | awk '{print $2}'`
heapsize_inK=`echo $heapsize_raw | sed 's/K//' `
heapsize=$(expr $heapsize_inK \* 1024)

if [ "$heapsize" = "" ]; then heapsize=0; fi;

sixpages=$(expr 6 \*  16 \* 1024)
sevenpages=$(expr 7 \* 16 \* 1024)

echo "expected: $sixpages <= heapsize <= $sevenpages ; heapsize was $heapsize" 
if [ $passed -eq 0 -a "$heapsize" -a $heapsize -ge $sixpages -a $heapsize -le $sevenpages ];
then
  echo ">>> test2 passed";
  total=`expr $total + 10`;
else
  echo ">>> test2 failed";
fi


# test 3
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
export LD_LIBRARY_PATH
./test3 2>&1 | tee out
passed=$?
heapsize_raw=`cat out | grep heap | awk '{print $2}'`
heapsize_inK=`echo $heapsize_raw | sed 's/K//' `
heapsize=$(expr $heapsize_inK \* 1024)

if [ "$heapsize" = "" ]; then heapsize=0; fi;

echo "expected: 0 < heapsize <= $sixteenk ; heapsize was $heapsize" 
if [ $passed -eq 0 -a "$heapsize" -a $heapsize -gt 0 -a $heapsize -le $sixteenk ];
then
  echo ">>> test3 passed";
  total=`expr $total + 10`;
else
  echo ">>> test3 failed";
fi






# test for large allocations
echo running maxheap
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
export LD_LIBRARY_PATH

./maxheap 2>&1 | tee out
passed=$?
heapsize_raw=`cat out | grep heap | awk '{print $2}'`
heapsize_inK=`echo $heapsize_raw | sed 's/K//' `
heapsize=$(expr $heapsize_inK \* 1024)

echo heapsize \= $heapsize

if [ "$heapsize" = "" ]; then heapsize=0; fi;

twogigs=$(expr 2 \* 1024 \* 1024 \* 1024 )


echo "expected: 0 < heapsize <= $twogigs ; heapsize was $heapsize" 
if [ $passed -eq 0 -a "$heapsize" -a $heapsize -gt 0 -a $heapsize -le $twogigs ];
then
  echo ">>> \"maxheap\" passed";
  total=`expr $total + 10`;
else
  echo heapsize over by $(expr $heapsize - $twogigs)
  echo ">>> \"maxheap\" failed";
fi




# test brk
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
export LD_LIBRARY_PATH
./brk 2>&1 | tee out
passed=$?
heapsize_raw=`cat out | grep heap | awk '{print $2}'`
heapsize_inK=`echo $heapsize_raw | sed 's/K//' `
heapsize=$(expr $heapsize_inK \* 1024)

if [ "$heapsize" = "" ]; then heapsize=0; fi;

fourpages=$(expr 4 \*  16 \* 1024 )

echo "expected: $fourpages <= heapsize <= $fourpages ; heapsize was $heapsize" 
if [ $passed -eq 0 -a "$heapsize" -a $heapsize -ge $fourpages -a $heapsize -le $fourpages ];
then
  echo ">>> brk passed";
  total=`expr $total + 10`;
else
  echo ">>> brk failed";
fi




# test stress
echo -----Running stress
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
export LD_LIBRARY_PATH
./stress 64 2>&1 | tee out
passed=$?
heapsize_raw=`cat out | grep heap | awk '{print $2}'`
heapsize_inK=`echo $heapsize_raw | sed 's/K//' `
heapsize=$(expr $heapsize_inK \* 1024)

if [ "$heapsize" = "" ]; then heapsize=0; fi;

onepage=$(expr 1 \*  16 \* 1024 )

echo "expected: $onepage <= heapsize <= $onepage ; heapsize was $heapsize" 
if [ $passed -eq 0 -a "$heapsize" -a $heapsize -ge $onepage -a $heapsize -le $onepage ];
then
  echo ">>> stress passed";
  total=`expr $total + 20`;
else
  echo ">>> stress failed";
fi





# test testcalloc
echo -----Running testcalloc
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
export LD_LIBRARY_PATH
./testcalloc 2>&1 | tee out
passed=$?
if [ $passed -eq 0 ];
then
  echo ">>> testcalloc passed";
  total=`expr $total + 10`;
else
  echo ">>> testcalloc failed";
fi



# test testrealloc
echo -----Running testrealloc
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
export LD_LIBRARY_PATH
./testrealloc 2>&1 | tee out
passed=$?
if [ $passed -eq 0 ];
then
  echo ">>> testrealloc passed";
  total=`expr $total + 10`;
else
  echo ">>> testrealloc failed";
fi




# test testfree
echo -----Running testrealloc
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
export LD_LIBRARY_PATH
./testfree 2>&1 | tee out
passed=$?
if [ $passed -eq 0 ];
then
  echo ">>> testfree passed";
  total=`expr $total + 10`;
else
  echo ">>> testfree failed";
fi




echo Total: $total / 100




