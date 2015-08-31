import fuzzy

engine = fuzzy.init()

engine.load('../wordlist/list.txt')

result = engine.search('kkir')

for ret in result:
    print ret

