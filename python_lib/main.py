import fuzzy

fuzzy.load('../wordlist/list.txt')

result = fuzzy.search('other')

for ret in result:
    print ret

