grep "newNode =" logDifferentiator.txt | awk '{print $NF}' > allocated.txt
grep "free:" logDifferentiator.txt | awk '{print $NF}' > freed.txt
comm -23 <(sort allocated.txt) <(sort freed.txt) > not_freed.txt
comm -12 <(sort allocated.txt) <(sort freed.txt) > freed_properly.txt
