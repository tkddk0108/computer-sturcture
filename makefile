amh_bigint: main.c amh_bigint.c
	gcc main.c amh_bigint.c -O -o amh_bigint
	
clean:
	rm amh_bigint
