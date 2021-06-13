/*
About this program:
- This program counts words.
- The specific words that will be counted are passed in as command-line
  arguments.
- The program reads words (one word per line) from standard input until EOF or
  an input line starting with a dot '.'
- The program prints out a summary of the number of times each word has
  appeared.
- Various command-line options alter the behavior of the program.

E.g., count the number of times 'cat', 'nap' or 'dog' appears.
> ./main cat nap dog
Given input:
 cat
 .
Expected output:
 Looking for 3 words
 Result:
 cat:1
 nap:0
 dog:0
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "smp0_tests.h"


/* Structures */
typedef struct {
  char *word;
  int counter;
} WordCountEntry;


int process_stream(WordCountEntry entries[], int entry_count)
{
  short line_count = 0;
  char buffer[30];
  char *word;
/*Part 3: Question 4*/
  while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    if (*buffer == '.')
      break;
/*Part 3: Question 5*/
  word = strtok(buffer, " \n");
  while (word != NULL){

    /* Compare against each entry */
    int i = 0;
    while (i < entry_count) {
      if (!strcmp(entries[i].word, buffer))
        entries[i].counter++;
      i++;
    }
    word = strtok(NULL, " \n");
  }
    line_count++;
  }
  return line_count;
}


void print_result(WordCountEntry entries[], int entry_count, FILE *stream)
{
  int i = 0;
  fprintf(stream, "Result:\n");
  while (i < entry_count) {
    fprintf(stream, "%s:%d\n", entries[i].word, entries[i].counter);
    i++;
  }
}


void printHelp(const char *name)
{
  fprintf(stderr, "usage: %s [-h] <word1> ... <wordN>\n", name);
}


int main(int argc, char **argv)
{
  const char *prog_name = *argv;
/*Part 3: Question 3*/
  WordCountEntry *entries;
  int entryCount = 0;
/*Part 3: Question 2*/
  FILE *fp = stdout;

  /* Entry point for the testrunner program */
  if (argc > 1 && !strcmp(argv[1], "-test")) {
    run_smp0_tests(argc - 1, argv + 1);
    return EXIT_SUCCESS;
  }

/*Part 3: Question 3*/
  if ((entries = malloc(sizeof(WordCountEntry) * (argc - 1))) == NULL){
	fprintf(stderr, "malloc failed, exiting\n");
	return EXIT_FAILURE;
  }

  argv++; 
  while (*argv != NULL) {
    if (**argv == '-') {

      switch ((*argv)[1]) {
        case 'h':
          printHelp(prog_name);
	  //return EXIT_SUCCESS;
	break; 
	case 'f':
	  if ((fp = fopen(*argv + 2, "w")) == NULL){
		fprintf(stderr, "Failed to open file \%s\".\n", *argv + 2);
			free(entries);
		return EXIT_FAILURE;
	}
	break;
        default:
          fprintf(stderr, "%s: Invalid option %s. Use -h for help.\n",
                 prog_name, *argv);
      }
    } 
    else {
/*Part 3: Question 3*/
      if (entryCount < argc - 1) {
        entries[entryCount].word = *argv;
        entries[entryCount++].counter = 0;
      }
    } 
    argv++;
  }
  if (entryCount == 0) {
    fprintf(stderr, "%s: Please supply at least one word. Use -h for help.\n",
           prog_name);
	free(entries);
    return EXIT_FAILURE;
  }
  if (entryCount == 1) {
    fprintf(fp, "Looking for a single word\n");
  } else {
    fprintf(fp, "Looking for %d words\n", entryCount);
  }

  process_stream(entries, entryCount);
  print_result(entries, entryCount, fp);

  free(entries);
  if (fp != stdout){
	fclose(fp);
  }

  return EXIT_SUCCESS;
}
