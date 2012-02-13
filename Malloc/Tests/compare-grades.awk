BEGIN {
    outcome = "passed";
}


/heapsize was/ {
    measured = $10;
#    print "Measured heap size "measured;
}

/HeapSize/ {
    reported = $2;
#    print "Reported heap size "reported
}

/>>>/ {
    if ( ($2 ~ /stress/) || ($2 ~ /test1/) || ($2 ~ /test2/) || ($2 ~ /test3/)  ){
	print "test "$2
	if ( ((measured - 16384) < reported) && ( reported <= (measured + 16384) ) ) {
	    print "OK";
	} else {
	    print "PROBLEM";
	    outcome = "failed";
	}
    } else 
    measured = 0;
    reported = 0;
}

END {
    print ">>>  accurate heap size: "outcome
}










