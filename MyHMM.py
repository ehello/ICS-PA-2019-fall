import numpy as np 
import time
from math import log

#测试集路径
testSetPath = "./test.txt"
#模型版本号
modelVer = "2020-09-20-10_16_55"

labels = ['B', 'E', 'S', 'M']

def import_models():
    a = np.load("./Models/" + modelVer + "-A.npy", allow_pickle=True).item()
    #print(a)
    b = np.load("./Models/" + modelVer + "-B.npy", allow_pickle=True).item()
    p = np.load("./Models/" + modelVer + "-Pi.npy", allow_pickle=True).item()
    
    return a, b, p
def get_median(aList):
    aList.sort()
    half = len(aList) // 2
    return (aList[half] + aList[~half]) / 2

def viterbi(sentence, A, B, Pi):
    sentenceLength = len(sentence)
    dpTab = [{}] #动态规划表
    seqAll = {} #记录到当前字符位置为止的标签顺序, 最终只用在4个可能标签中取最佳
                #键是4种标签, 值是当前位置之前的标签顺序
    
    if sentence[0] not in B['B']: #当句子第一个字符没在训练集出现过, 那么不在B['B']中, 也就不在B中
        for state in labels:
            if state == 'S': #认为它是一个单字, 并设置发射概率
                B[state][sentence[0]] = log(0.5)
            else:
                B[state][sentence[0]] = float("-Inf")
    for label in labels:
        dpTab[0][label] = dpTab[0].get(label, 0.0) + (Pi[label] + B[label][sentence[0]])
        seqAll[label] = [label]

    

    for idx in range(1, sentenceLength):
        dpTab.append({}) #增加一个当前位置字符的各标签概率表
        seqNow = {}    
        for curLabel in labels: #idx位置字符可能的状态值
            choices = [] #记录前一字符到当前字符各种标签顺序的概率值
            for prevLabel in labels: #(idx - 1)位置字符的状态值
                if sentence[idx] not in B[curLabel]: #当前位置字符不在发射概率矩阵中时
                    emitProb = get_median(list(B[curLabel].values()))

                    #print("Median of emitProb of %s: %f"%(curLabel, emitProb))

                    B[curLabel][sentence[idx]] = emitProb
                prob = dpTab[idx - 1][prevLabel] + A[prevLabel][curLabel] + B[curLabel][sentence[idx]]
                choices.append((prob, prevLabel))
            bestChoice = max(choices)
            dpTab[idx][curLabel] = bestChoice[0]
            seqNow[curLabel] = seqAll[bestChoice[1]] + [curLabel]
        seqAll = seqNow

    #print(dpTab)    
    #maxProb, label = max([(dpTab[-1][label], label) for label in labels])
    #path = [max(item, key = item.get) for item in dpTab]
    bestLastLabel = max(dpTab[-1], key=dpTab[-1].get)
    #print(path)
    return seqAll[bestLastLabel]

def seg_sentence(sentence, tags): #待完善
    segSentence = []
    assert(len(sentence) == len(tags))
    if tags[-1] != 'S' and tags[-1] != 'E':
        if tags[-2] == 'B' or tags[-2] == 'M':
            tags[-1] = 'E'
        else:
            tags[-1] = 'S'

    sentenceLength = len(sentence)
    idx = 0
    while idx < sentenceLength:
        if tags[idx] == 'B':
            cursor = idx + 1
            while tags[cursor] != 'E' and cursor < sentenceLength:
                cursor += 1
            if cursor != sentenceLength:
                segSentence.append(sentence[idx: cursor + 1])
                idx = cursor + 1
            else:
                segSentence.append(sentence[idx: sentenceLength])
                break 
        else:
            segSentence.append(sentence[idx])
            idx += 1

    return segSentence

def apply_all():
    A, B, Pi = import_models()
    
    #print(A)

    testSet = open(testSetPath, encoding="utf-8")
    sentence = testSet.readline().strip()
    strTime = time.strftime("%Y-%m-%d-%H_%M_%S", time.localtime(int(time.time())))
    outputFile = open("./Results/181220039-H-" + strTime +".txt", "a", encoding="utf-8")
    while sentence:
        #print(sentence)
        sentencePreLabels = viterbi(sentence, A, B, Pi)
        #print(sentencePreLabels)
        segSentenceList = seg_sentence(sentence, sentencePreLabels)
        segSentenceStr = ''
        for idx in range(len(segSentenceList) - 1):
            segSentenceStr = segSentenceStr + segSentenceList[idx] + ' '
        segSentenceStr = segSentenceStr + segSentenceList[-1] + '\n'
        #print(segSentenceStr)
        outputFile.write(segSentenceStr)

        sentence = testSet.readline().strip()
        #break
    testSet.close()
    outputFile.close()
    
if __name__ == "__main__":
    apply_all()