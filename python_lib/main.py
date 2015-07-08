import fuzzy

fuzzy.init()

with open('../wordlist/list.txt', 'r') as f:
    for line in f:
        fuzzy.build(line)

result = fuzzy.search('eucation')

for ret in result:
    print ret

