import fuzzy

fuzzy.load('../wordlist/list.txt')

result = fuzzy.search('eucation')

for ret in result:
    print ret

