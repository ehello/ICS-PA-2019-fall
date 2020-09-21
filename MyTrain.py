import time
import numpy as np
from math import log

trainSetPath = "./train.txt"
labels = ['B', 'E', 'S', 'M'] #开始, 结束, 单字, 中间
labelCnt = {'B':0, 'E': 0, 'S': 0, 'M': 0}
lineCnt = 0
wordCnt = 0

#HMM的模型
#转移概率矩阵 {'B':{{'B':{}, 'E':{}, 'S':{}, 'M':{}}}, 'E':{...}, 'S':{...}, 'M':{...}}
A = {} 
#发射概率矩阵 {'B':{'我': ..., '爱': ..., '你': ...}, 'E':{...}, 'S':{...}, 'M':{...}}
B = {} 
#初始状态分布 {'B': 0.0, 'E': 0.0, 'S': 0.0, 'M': 0.0}
Pi = {} 

#BMM需要的模型
Dic = {}
windowSize = 0
maxLenWord = ''

def init_matrix():
    for lb0 in labels:
        A[lb0] = {}
        B[lb0] = {}
        Pi[lb0] = 0.0
        for lb1 in labels:
            A[lb0][lb1] = 0.0

def tag_word(word):
    tags = []

    if (len(word) == 1):
        tags.append('S')

    elif (len(word) == 2):
        tags.append('B')
        tags.append('E')
    else:
        tags.append('B')
        tags.extend(['M'] * (len(word) - 2))
        tags.append('E')
    return tags

def add_char(word):
    chars = []
    chars.extend(list(word))
    return chars

def adjust_prob(labelCnt, lineCnt):
    #取对数可以使得后续的概率相乘转换成相加
    for key in Pi:
        if Pi[key] == 0.0:
            Pi[key] = float("-Inf")
        else:
            Pi[key] = log(Pi[key] / lineCnt)

    for key0 in A:
        for key1 in A[key0]:
            if A[key0][key1] == 0.0:
                A[key0][key1] = float("-Inf")
            else:
                A[key0][key1] = log(A[key0][key1] / labelCnt[key0])
    
    # 取对数对于小频字符能保证精度 
    for key0 in B: #每个标签
        for key1 in B[key0]: #标签下的每个字符
            if B[key0][key1] == 0.0:
                B[key0][key1] = float("-Inf")
            else:
                B[key0][key1] = log(B[key0][key1] / labelCnt[key0])
    
    for key in Dic:
        Dic[key] = log(Dic[key] / wordCnt)

def save_models():
    strTime = time.strftime("%Y-%m-%d-%H_%M_%S", time.localtime(int(time.time())))

    fA = open("./Models/" + strTime + "-A", "w+", encoding="utf-8")
    fA.write(str(A))
    fA.close

    fB = open("./Models/" + strTime + "-B", "w+", encoding="utf-8")
    fB.write(str(B))
    fB.close

    fP = open("./Models/" + strTime + "-Pi", "w+", encoding="utf-8")
    fP.write(str(Pi))
    fP.close

    fD = open("./Models/" + strTime + "-Dic", "w+", encoding="utf-8")
    fD.write(str(Dic))
    fD.close

    fW = open("./Models/" + strTime + "-WS", "w+", encoding="utf-8")
    fW.write(str(windowSize))
    fW.close

    np.save("./Models/" + strTime + "-A.npy", A)
    np.save("./Models/" + strTime + "-B.npy", B)
    np.save("./Models/" + strTime + "-Pi.npy", Pi)
    np.save("./Models/" + strTime + "-Dic.npy", Dic)
    np.save("./Models/" + strTime + "-WS.npy", windowSize)

def train_models(dataPath):
    trainSet = open(dataPath, encoding='utf-8')
    #print(lineCnt)
    line = trainSet.readline().strip() #Read the first line and cut off the '' at beginning and '\n'
    #lim = 20
    while line:
        global lineCnt, wordCnt, windowSize, maxLenWord 
        lineCnt += 1
        line = line.split()
        #print(line)

        #这两个列表的长度一致
        lineChars = [] # 当前行包含的所有字符
        lineLabels = [] #当前行每个字符的标签
            
        for word in line:
            Dic[word] = Dic.get(word, 0) + 1
            wordCnt += 1
            if len(word) > windowSize:
                windowSize = len(word)
                maxLenWord = word
            
            lineLabels.extend(tag_word(word))
            lineChars.extend(add_char(word))

        #print("%s\n%s"%(lineChars,lineLabels))
        assert(len(lineChars) == len(lineLabels))
        lineLength = len(lineChars)

        Pi[lineLabels[0]] += 1 #初始状态分布概率矩阵中, 对应的状态的数量加上1
        
        #print("%s\n%s"%(lineChars,lineLabels))

        for idx in range(lineLength):
            labelCnt[lineLabels[idx]] += 1

            if (idx < lineLength - 1): #更新转移矩阵时, 只需关注前(lineLength - 1)个字符的情况
                #更新转移矩阵中, 对应转移情况的值
                A[lineLabels[idx]][lineLabels[idx + 1]] =  A[lineLabels[idx]].get(lineLabels[idx + 1], 0)+ 1 
            
            for label in labels: # 如果当前字符在发射概率矩阵中的某一行不存在, 则添加并初始化为0
                if lineChars[idx] not in B[label]:
                    B[label][lineChars[idx]] = B[label].get(lineChars[idx], 0)
            B[lineLabels[idx]][lineChars[idx]] = B[lineLabels[idx]].get(lineChars[idx], 0) + 1
        
        line = trainSet.readline().strip() #Read next line
    trainSet.close()

def train():
    #trainSet_1 = open(trainSetPath, encoding='utf-8')
    init_matrix()
    train_models(trainSetPath)
    train_models("./dev.txt")
    #learn_from_data("./pku_training.utf8")
    #learn_from_data("./msr_training.utf8")
    
    print("Total number of each label: %s"%(labelCnt))
    print("Total number of lines for train: %d"%(lineCnt))
    print("Total number of words for train: %d"%(wordCnt))
    print("Window size: %d, word: %s"%(windowSize, maxLenWord))
    
    adjust_prob(labelCnt, lineCnt)
    save_models()
    

if __name__ == "__main__":
    train()
    