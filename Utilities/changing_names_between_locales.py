""""
This script contains functions which help in creating txt file which contains
proper nouns from tsv, merging proper nouns txt files, and to rename names in TSVs
"""
import csv
from nltk.tokenize import word_tokenize
from nltk.tag import pos_tag
import nltk
import io


def get_proper_nouns(fin_fname, fout_fname):
    """
    This function get all the possible proper nouns from the TSV file and list
    them in the the output file.
    fin_fname: TSV file for the proper nouns
    fout_fname: output file for the listing proper nouns(not in append mode)
                [*.txt file]
    Example:
        get_proper_nouns('eggquizliteracy_levels_en.tsv','propernouns3.txt')
    """
    nltk.download('punkt')
    nltk.download('averaged_perceptron_tagger')
    proper_nouns = set()
    # Read the sentences from the TSV and use NLTK Tagger to tag each of the word
    with open(fin_fname, encoding="utf8") as tsvfile:
        reader = csv.reader(tsvfile, delimiter='\t')
        for row in reader:
            for cell in row:
                    sentence = cell.replace('^', ' ').replace('_', '')
                    token = word_tokenize(sentence)
                    # tagged_sent = pos_tag(sentence.split())
                    tagged_sent = pos_tag(token)
                    for word, pos in tagged_sent:
                        if pos == 'NNP':
                            word = word.replace("'s", '')
                            if word.isalpha():
                                propernouns.add(word)
        # List of proper nouns
        print(propernouns)
        # Creating a new file named fout_fname to store proper nouns
        with open(fout_fname, 'w', encoding="utf8") as f:
            for item in propernouns:
                print(item, file=f)


def merge_proper_nouns_files(fin_files, fout_file):
    """
    This function get all the  proper nouns and generate fout_file file which
    will contains all the unique proper nouns
    fin_files: List of the files which contains proper nouns
    fout_file: Output file which will finally contains proper nouns(not opened
    in append mode)[*.txt file]
    Example:
        merge_proper_nouns_files(["propernouns.txt", "propernouns2.txt"], 'merged_names2.txt')
    """
    proper_nouns = set()
    # Reading each of the proper nouns from the each of the txt file
    for file in fin_files:
        f = open(file, "r", encoding="utf8").readlines()
        for line in f:
            word = line.strip()
            propernouns.add(word)
    # Creating a new file which stores all the unique proper nouns
    print(propernouns)
    with open(fout_file, 'w', encoding="utf8") as f:
        for item in propernouns:
            print(item, file=f)


def replace_proper_nouns(tsv_in, tsv_out, map_fname):
    """
    This function will change the names in TSV and generate a new file which
    will contains new names
    tsv_in: The TSV file where the names are to be changed
    tsv_out: The TSV file that will be generated which will contains the
            changed name
    map_fname: This file will contains tab separated two values as the mapping
            from the old name to new name
    Example:
        replace_proper_nouns('wordwindow_level_en.tsv', 'wordwindow_level_en_change.tsv', 'mapping.txt')
    """
    # Reading each line from the mapping file and creating a map for the old name and new name
    f_indian = io.open(map_fname, 'r', encoding='utf-8').readlines()
    names_dict = dict()
    for line in f_indian:
        src_word, dst_word = line.split('\t')
        src_word = src_word.strip()
        dst_word = dst_word.strip()
        names_dict[src_word] = dst_word

    # Reading the rows from the SRC TSV file and updating the cells and writing the new cells into DEST TSV file
    with open(tsv_in, 'r',  newline='', encoding='utf-8-sig') as fin, open(tsv_out, 'w', newline='', encoding='utf-8') as fout:
        reader = csv.reader(fin, delimiter='\t', quoting=csv.QUOTE_NONE)
        writer = csv.writer(fout, delimiter='\t', quotechar='', quoting=csv.QUOTE_NONE)
        for line in reader:
            for i in range(len(line)):
                for src_word in names_dict:
                    start_chars = [u' ', u'.', u'^', u'']
                    last_chars = [u' ', u'.', u',', u'?', u"'s"]
                    cell = line[i]
                    for start_char in start_chars:
                        for last_char in last_chars:
                            cell = cell.replace(start_char+src_word+last_char, start_char+names_dict[src_word]+last_char)
                    line[i] = cell
            writer.writerow(line)


mapping_file = 'mapping.txt'
locale_list = ['en', 'hi']
for locale in locale_list:
    replace_proper_nouns('original/eggquizliteracy_levels_' + locale + '.tsv', 'changed/eggquizliteracy_levels_' + locale + '.tsv', mapping_file)
    replace_proper_nouns('original/eggquizmath_levels_' + locale + '.tsv', 'changed/eggquizmath_levels_' + locale + '.tsv', mapping_file)
    replace_proper_nouns('original/wordwindow_level_' + locale + '.tsv', 'changed/wordwindow_level_' + locale + '.tsv', mapping_file)
