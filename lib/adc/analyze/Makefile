default:
	@echo skip adc entropy by default

ENTROPY_FILE=entropy.txt

results: clean
	mkdir -p out
	cat $(ENTROPY_FILE).bz2 | bunzip2 > out/$(ENTROPY_FILE)
	cat out/$(ENTROPY_FILE) | python3 create_bin_files.py
	cat out/$(ENTROPY_FILE) | python3 collect_raw_data.py > out/raw_data.txt
	cat out/raw_data.txt | python raw_data_to_csv.py > out/results.csv
	echo "--- CREATED out/results.csv ---"

clean:
	rm -rf out
