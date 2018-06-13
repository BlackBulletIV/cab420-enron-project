#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

const char* split_path = "processed_data/users/";
const int author_allowance = 1000;
const char* first_cmp = "frozenset({'";
const char* second_cmp = "enron.com";

char* make_path(char* name)
{
  char* p = malloc(sizeof(char) * 164);
  memcpy(p, split_path, (size_t) 22);
  strcat(p, name);
  return p;
}

int main(int argc, char** argv)
{
  size_t buffer_size = 4096;
  size_t name_size = 128;
  char cmp_buffer[64];
  char* buffer = malloc(sizeof(char) * buffer_size);
  char* buffer_shift;
  char* author_name = malloc(sizeof(char) * name_size);
  char author_okay = 0;
  char* path;

  int i;
  int line = 0;
  char* current_author = malloc(sizeof(char) * name_size);
  int total_authors = 0;
  int commas;
  ssize_t line_len;

  FILE* author_file;
  FILE* data_file = fopen("data/enron.csv", "r");

  while ((line_len = getline(&buffer, &buffer_size, data_file)) != -1)
  {
    line++;
    if (line == 1) continue;
    commas = 0;
    author_okay = 0;

    for (buffer_shift = buffer; *buffer_shift != '\0'; buffer_shift++)
    {
      if (commas == 3)
      {
        memcpy(cmp_buffer, buffer_shift, 12);
        cmp_buffer[12] = '\0';

        if (strcmp(cmp_buffer, first_cmp) == 0)
        {
          buffer_shift += 12;

          for (i = 0; *buffer_shift != '@'; i++)
          {
            author_name[i] = *buffer_shift;
            buffer_shift++;
          }

          author_name[i] = '\0';
          buffer_shift++;
          memcpy(cmp_buffer, buffer_shift, 9);
          cmp_buffer[9] = '\0';
          if (strcmp(cmp_buffer, second_cmp) == 0) author_okay = 1;
        }

        break;
      }

      if (*buffer_shift == ',') commas++; 
    }

    if (author_okay)
    {
      if (total_authors == 0)
      {
        total_authors++;
        memcpy(current_author, author_name, name_size);
        path = make_path(author_name);
        author_file = fopen(path, "a");
        free(path);
      }
      else if (strcmp(author_name, current_author) != 0)
      {
        total_authors++;
        memcpy(current_author, author_name, name_size);
        fclose(author_file);
        path = make_path(author_name);
        author_file = fopen(path, "a");
        free(path);
      }

      printf("%i: %s\n", line, author_name);
      fwrite(buffer, sizeof(char), (size_t) line_len, author_file);
    }
  }

  fclose(author_file);
  fclose(data_file);
  free(buffer);
  free(author_name);
  free(current_author);
}
