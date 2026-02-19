# dead peer zombie test.
for i in $(seq 1 5000); do
	nc 127.0.0.1 8080 &
done
