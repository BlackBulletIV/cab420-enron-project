all: split select

split:
	gcc -o preprocessors/split_data preprocessors/split_data.c

select:
	gcc -o preprocessors/select_subset preprocessors/select_subset.c

lines:
	wc -l preprocessors/identify_main_users.rb preprocessors/preprocess.rb preprocessors/*.c *.py
