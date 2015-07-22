import re

class TrieNode:
    def __init__(self):
        self.child_char = []
        self.child_node = {}
        self.word = None
        self.pri = 0

root = TrieNode()
counter = 0
key_map = {
            'a':['s'],
            'b':['v','n'],
            'c':['x','v'],
            'd':['s','f'],
            'e':['w','r'],
            'f':['d','g'],
            'g':['h','f'],
            'h':['g','j'],
            'i':['u','o'],
            'j':['h','k'],
            'k':['j','l'],
            'l':['k'],
            'm':['n'],
            'n':['b','m'],
            'o':['i','p'],
            'p':['o'],
            'q':['w','a'],
            'r':['e','t'],
            's':['a','d'],
            't':['r','y'],
            'u':['y','i'],
            'v':['c','b'],
            'w':['q','e'],
            'x':['z','c'],
            'y':['t','u'],
            'z':['x'],
    }

def build_helper(word):
    '''
        Build the trie with word
    '''
    global root
    node = root
    for ch in word:
        next_node = None
        if ch in node.child_char:
            next_node = node.child_node[ch]
        else:
            next_node = TrieNode()
            node.child_char.extend(ch)
            node.child_node[ch] = next_node
        node = next_node
    node.word = word
    global counter
    node.pri = counter

def build(word):
    '''
        Load each word in the dictionary and build the trie
    '''
    global counter
    counter = counter + 1
    pattern = '^[a-zA-Z]+$'
    ret = re.search(pattern, word)
    if ret and len(word) > 0:
        build_helper(word.rstrip())

def load(file_path):
    with open(file_path, 'r') as f:
        for line in f:
            build(line)

def search(spell):
    '''
        Search for the results of a spelling
    '''
    u_results = {}
    global root

    spell = spell.lower()
    spell = re.sub('[^a-z]', ' ', spell)
    spell = spell.split();

    if len(spell):
        spell = spell[0]
        if len(spell) > 2:
            dfs(root, 0, spell, u_results, 1, 1, 2)
            results = u_results.items()
            results = [a[0] for a in sorted(results, key=lambda results: results[1])]
        else:
            results = [spell]
    else:
        results = []

    return results

def dfs(node, pos, spell, results, insert, delete, fuzz):
    '''
        Traverse every node and put results into results array
        1. Try to match as many char in the spelling as possible
        2. When the spelling has ended, for every downstream node, we try to auto complete by
            a. Checking if the current node is a keyword
            b. Try to go down from the first child, and then repeat step a.
    '''
    if pos < len(spell):
        if spell[pos] in node.child_char:
            dfs(node.child_node[spell[pos]], pos + 1, spell, results, insert, delete, fuzz)

        if insert > 0:
            dfs(node, pos + 1, spell, results, insert - 1, delete, fuzz)

        if delete > 0:
            for ch in node.child_node.values():
                dfs(ch, pos, spell, results, insert, delete-1, fuzz)

        if fuzz > 0:
            for fuz_char in key_map[spell[pos]]:
                if fuz_char in node.child_char:
                    dfs(node.child_node[fuz_char], pos + 1, spell, results, insert, delete, fuzz-1)

    else:
        if node.word and node.word not in results:
            results[node.word] = node.pri * (1 - pow(0.95, len(results) + 1))

        length = min(len(node.child_char), 3)

        for i in range(length):
            dfs(node.child_node[node.child_char[i]], pos, spell, results, 0, 0, 0)





