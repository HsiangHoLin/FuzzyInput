import fuzzy

fuzzy.init()

with open('wordlist/list.txt', 'r') as f:
    for line in f:
        fuzzy.build(line)

result = fuzzy.search('success')

for ret in result:
    print ret

'''
class test:
    def __init__(self):
        d = []
        self.num = 1

t = test()
print t.num

'''
