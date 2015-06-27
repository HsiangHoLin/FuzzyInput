import re

class TrieNode:
    def __init__(self):
        self.child_char = []
        self.child_node = {}
        self.word = None
        self.pri = 0

root = None
counter = 0
key_map = {
            'a':['s'],
            'b':['v','h','n'],
            'c':['x','f','v'],
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

def init():
    '''
        Initialize the root node
    '''
    global root
    root = TrieNode()

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

def search(spell):
    '''
        Search for the results of a spelling
    '''
    results = []
    global root
    dfs(root, 0, spell, results, 0, 0, 0)
    return sorted(results, key=lambda results: results[1])

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
        else:
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
        if node.word:
            results.append((node.word, node.pri))
        length = min(len(node.child_char), 3)
        for i in range(length):
            dfs(node.child_node[node.child_char[i]], pos, spell, results, 0, 0, 0)





