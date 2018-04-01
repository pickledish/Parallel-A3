for name in histogram histo_private histo_lockfree histo_lock1 histo_lock2
do
	./$name inputs/earth.ppm outFile_$name 1
done
for name in histo_private histo_lockfree histo_lock1 histo_lock2
do
	echo "Diff file for normal correct histogram and $name: "
        diff outFile_histogram outFile_$name
        echo ""
done

