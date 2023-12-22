import csv
import sys


def main():

    # Check for command-line usage
    if len(sys.argv) != 3:
        print('Usage: python dna.py data.csv sequence.txt')
        sys.exit(1)
    else:

        # Read database file into a variable
        database = open(sys.argv[1], 'r')
        database_reader = csv.DictReader(database)

        # Read DNA sequence file into a variable
        sequence = open(sys.argv[2], 'r')
        sequence_reader = sequence.read()

        # Extract STRs to search for from database
        # Credit Кирилл Морозов (https://grabthiscode.com/python/how-to-extract-keys-from-dictreader-python)
        headings = database_reader.fieldnames
        strs = headings[1:]

        # Find longest match of each STR in DNA sequence
        str_count = {}
        for s in strs:
            str_count[s] = str(longest_match(sequence_reader, s))

        # Check database for matching profiles
        for person in database_reader:

            # Credit Martijn Pieters (https://stackoverflow.com/questions/30818694/test-if-dict-contained-in-dict)
            if str_count.items() <= person.items():
                print(person['name'])
                sys.exit(0)

        # No match found
        print('No match')
        sys.exit(0)


def longest_match(sequence, subsequence):
    """Returns length of longest run of subsequence in sequence."""

    # Initialize variables
    longest_run = 0
    subsequence_length = len(subsequence)
    sequence_length = len(sequence)

    # Check each character in sequence for most consecutive runs of subsequence
    for i in range(sequence_length):

        # Initialize count of consecutive runs
        count = 0

        # Check for a subsequence match in a "substring" (a subset of characters) within sequence
        # If a match, move substring to next potential match in sequence
        # Continue moving substring and checking for matches until out of consecutive matches
        while True:

            # Adjust substring start and end
            start = i + count * subsequence_length
            end = start + subsequence_length

            # If there is a match in the substring
            if sequence[start:end] == subsequence:
                count += 1

            # If there is no match in the substring
            else:
                break

        # Update most consecutive matches found
        longest_run = max(longest_run, count)

    # After checking for runs at each character in seqeuence, return longest run found
    return longest_run


main()
