for name in histogram histo_private histo_lockfree histo_lock1 histo_lock2
do
	./$name moon-small.ppm outFile_$name 1
done
