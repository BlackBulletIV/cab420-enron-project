#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <string.h>

#define TRAIN_SAMPLES 100
#define TEST_SAMPLES 20

const char* path = "processed_data/filtered_users/";
const char* line_check = ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,False";

char* make_path(char* name)
{
  char* p = malloc(sizeof(char) * 164);
  memcpy(p, path, (size_t) 31);
  strcat(p, name);
  return p;
}

int count_lines(FILE* f)
{
  int lines = 0;
  char c;

  while ((c = getc(f)) != EOF)
  {
    if (c == '\n') lines++;
  }

  fseek(f, 0, SEEK_SET);
  return lines;
}

void generate_lines(int lines, int* train_lines, int* test_lines)
{
  int i;
  int max_step;
  int sum = 0;
  int train_i = 0;
  int test_i = 0;

  for (i = 0; i < TRAIN_SAMPLES + TEST_SAMPLES; ++i)
  {
    max_step = (lines - sum) / (TRAIN_SAMPLES + TEST_SAMPLES - i);
    sum += 1 + rand() / (RAND_MAX / max_step + 1);

    if (i % (TRAIN_SAMPLES / TEST_SAMPLES + 1) == 0)
    {
      test_lines[test_i] = sum;
      test_i++;
    }
    else
    {
      train_lines[train_i] = sum;
      train_i++;
    }
  }
}

void write_lines(FILE* f, char* name, int* lines, int n)
{
  int i;
  fprintf(f, ":%s:\n", name);
  for (i = 0; i < n; ++i) fprintf(f, "%i,", lines[i]);
  fseek(f, -1, SEEK_CUR);
  fprintf(f, "\n");
}

char check_line_end(char* line, ssize_t line_len)
{
  int i;
  int line_i = line_len - 43;

  for (i = 0; i < 42; ++i)
  {
    if (line_check[i] != line[line_i]) return 0;
    line_i++;
  }

  return 1;
}

void print_int_array(int* A, int n)
{
  int i;
  printf("[");

  for (i = 0; i < n; ++i)
  {
    printf(i == n - 1 ? "%i" : "%i,", A[i]);
  }

  printf("]\n");
}

int main(int argc, char** argv)
{
  char* file_path;
  int lines;
  int cur_line, cur_rand_line, cur_test_line;
  int* train_lines = malloc(sizeof(int) * TRAIN_SAMPLES);
  int* test_lines = malloc(sizeof(int) * TEST_SAMPLES);
  int users = 0;
  int output_lines = 0;
  int user_output = 0;
  int rejected = 0;

  ssize_t line_len;
  size_t buffer_size = 4096;
  char* buffer = malloc(sizeof(char) * buffer_size);

  FILE* f;
  FILE* main_file;
  FILE* test_file;
  FILE* train_line_file;
  FILE* test_line_file;

  struct dirent* entry;
  DIR* d = opendir(path);

  srand(time(NULL));

  if (d != NULL)
  {
    main_file = fopen("processed_data/raw_dataset.csv", "w");
    test_file = fopen("processed_data/raw_test_dataset.csv", "w");
    train_line_file = fopen("processed_data/raw_train_lines.txt", "w");
    test_line_file = fopen("processed_data/raw_test_lines.txt", "w");

    while (entry = readdir(d))
    {
      if (entry->d_name == "." || entry->d_name == "..") continue;
      file_path = make_path(entry->d_name);
      f = fopen(file_path, "r");
      lines = count_lines(f);

      if (lines >= TRAIN_SAMPLES + TEST_SAMPLES)
      {
        generate_lines(lines, train_lines, test_lines);
        write_lines(train_line_file, entry->d_name, train_lines, TRAIN_SAMPLES);
        write_lines(test_line_file, entry->d_name, test_lines, TEST_SAMPLES);
        cur_line = 0;
        cur_rand_line = 0;
        cur_test_line = 0;
        user_output = 0;
        users++;

        printf("%s: %i", file_path, lines);

        while ((line_len = getline(&buffer, &buffer_size, f)) != -1)
        {
          if (cur_line == train_lines[cur_rand_line])
          {
            if (check_line_end(buffer, line_len))
            {
              fprintf(main_file, "%s,", entry->d_name);
              fwrite(buffer, sizeof(char), line_len, main_file);
              output_lines++;
              user_output++;
            }
            else
            {
              rejected++;
            }

            cur_rand_line++;
          }
          else if (cur_line == test_lines[cur_test_line])
          {
            if (check_line_end(buffer, line_len))
            {
              fprintf(test_file, "%s,", entry->d_name);
              fwrite(buffer, sizeof(char), line_len, test_file);
              output_lines++;
              user_output++;
            }
            else
            {
              rejected++;
            }

            cur_test_line++;
          }

          cur_line++;
        }

        printf("\t%i\n", user_output);
      }

      fclose(f);
      free(file_path);
    }

    printf("%i total users\n", users);
    printf("%i lines written\n", output_lines);
    printf("%i lines rejected\n", rejected);

    fclose(main_file);
    fclose(test_file);
    fclose(train_line_file);
    fclose(test_line_file);
    closedir(d);
  }
  else
  {
    perror("Couldn't open directory");
  }

  free(buffer);
  free(train_lines);
  free(test_lines);
  return 0;
}
