import numpy as np 
import time 
import MyHMM
from MyHMM import viterbi, seg_sentence

#测试集路径
testSetPath = "./test.txt"
#模型版本号
modelVer = "2020-09-20-11_23_01"
#labels = MyHMM.labels

def import_models():
    d = np.load("./Models/" + modelVer + "-Dic.npy", allow_pickle=True).item()
    w = np.load("./Models/" + modelVer + "-WS.npy", allow_pickle=True).item()

    return d, w

def fmm(Dic, windowSize, sentence):
    sentenceLength = len(sentence)
    idx = 0
    segSentenceList = []
    
    while idx < sentenceLength:
        match = False
        if windowSize > sentenceLength - idx:
            windowSize = sentenceLength - idx
        for i in range(windowSize, 0, -1):
            subStr = sentence[idx: idx + i]
            if subStr in Dic:
                segSentenceList.append(subStr)
                match = True
                idx += i
                break
        if not match: #可以改进的地方1
            segSentenceList.append(sentence[idx])
            idx += 1
    return segSentenceList

def bmm(Dic, windowSize, sentence):
    sentenceLength = len(sentence)
    idx = sentenceLength
    segSentenceList = []

    while idx > 0:
        match = False
        if windowSize > idx - 0:
            windowSize = idx - 0
        for i in range(windowSize, 0, -1):
            subStr = sentence[idx - i: idx]
            if subStr in Dic:
                segSentenceList.append(subStr)
                match = True
                idx -= i
                break
        if not match:
            segSentenceList.append(sentence[idx - 1])
            idx -= 1
    segSentenceList.reverse()
    return segSentenceList

def bimm_enhance(Dic, windowSize, A, B, Pi, sentence):
    fSegSentenceList = fmm(Dic, windowSize, sentence)
    bSegSentenceList = bmm(Dic, windowSize, sentence)
    sentencePreLabels = viterbi(sentence, A, B, Pi)
    hSegSentenceList = seg_sentence(sentence, sentencePreLabels)

    #print("Forward: %s\nBackward: %s"%(fSegSentenceList, bSegSentenceList))

    #if fSegSentenceList == bSegSentenceList:
    #    return fSegSentenceList
    
    fWordCnt = len(fSegSentenceList)
    bWordCnt = len(bSegSentenceList)
    hWordCnt = len(hSegSentenceList)

    fSingleCnt = bSingleCnt = hSingleCnt = 0
    fNonDicCnt = bNonDicCnt = hNonDicCnt = 0


    for word in fSegSentenceList:
        if len(word) == 1:
            fSingleCnt += 1
        if word not in Dic:
            fNonDicCnt += 1

    for word in bSegSentenceList:
        if len(word) == 1:
            bSingleCnt += 1
        if word not in Dic:
            bNonDicCnt += 1
    
    for word in hSegSentenceList:
        if len(word) == 1:
            hSingleCnt += 1
        if word not in Dic:
            hNonDicCnt += 1
    
    wcWeight = 0.8
    scWeight = 0.1
    ndWeight = 0.1
    fPenScore = wcWeight * fWordCnt + scWeight * fSingleCnt + ndWeight * fNonDicCnt
    bPenScore = wcWeight * bWordCnt + scWeight * bSingleCnt + ndWeight * bNonDicCnt
    hPenScore = wcWeight * hWordCnt + scWeight + hSingleCnt + ndWeight * hNonDicCnt

    scoreList = [fPenScore, bPenScore, hPenScore]
    if hPenScore == min(scoreList):
        if bPenScore == hPenScore:
            return bSegSentenceList
        return hSegSentenceList
    elif fPenScore == min(scoreList):
        if bPenScore == fPenScore:
            return bSegSentenceList
        return fSegSentenceList
    elif bPenScore == min(scoreList):
        return bSegSentenceList

def unit_test():
    sentence = "《经济日报》产经新闻部主任崔书文：负面清单管理制度的推出和修订，提高了开放度和透明度。"
    Dic, windowSize = import_models()
    A, B, Pi = MyHMM.import_models()
    fSegSentenceList = fmm(Dic, windowSize, sentence)
    bSegSentenceList = bmm(Dic, windowSize, sentence)

    sentencePreLabels = viterbi(sentence, A, B, Pi)
    hSegSentenceList = seg_sentence(sentence, sentencePreLabels)
    print("Forward: %s\nBackward: %s\nLabels: %s\nHMM: %s"%(fSegSentenceList, bSegSentenceList, sentencePreLabels,hSegSentenceList))
    

def apply_all():
    Dic, windowSize = import_models()
    A, B, Pi = MyHMM.import_models()
    testSet = open(testSetPath, encoding="utf-8")
    sentence = testSet.readline().strip()
    strTime = time.strftime("%Y-%m-%d-%H_%M_%S", time.localtime(int(time.time())))
    outputFile = open("./Results/181220039-bi-" + strTime +".txt", "a", encoding="utf-8")
    while sentence:
        segSentenceList = bimm_enhance(Dic, windowSize, A, B, Pi, sentence)
        segSentenceStr = ''
        for idx in range(len(segSentenceList) - 1):
            segSentenceStr = segSentenceStr + segSentenceList[idx] + ' '
        segSentenceStr = segSentenceStr + segSentenceList[-1] + '\n'
        outputFile.write(segSentenceStr)

        #break
        sentence = testSet.readline().strip()
    testSet.close()
    outputFile.close()
    #print(windowSize)

if __name__ == "__main__":
    apply_all()
    #unit_test()

