import torch
import torch.nn as nn
from torch.nn.utils.rnn import pad_sequence
import numpy as np
import time, math
import pickle

charsVocab = ['s', 'i', 'd', 'n', 'y', ' ', 'o', 'p', 'e', 'r', 'a', 'h', 'u', 't', 'k', 'j', 'l', 'b', 'm', 'c', 'v', ',', '.', 'g', 'w', 'z', 'ൻ', 'ൽ', 'ർ', 'ൺ', 'ൾ', 'q', '|', 'f', 'x', "'", '(', '1', '2', ')', '?', '9', '8', '4', ';', '3', '"', '‘', '’', '0', '!', '\u200b', '5', '%', '6', ':', '7', 'ʼ', '/', '°']
PAD_NULL = '-'
PAD_START = '*'
PAD_END = '^'

vocab = [PAD_NULL, PAD_START, PAD_END]+[i[0] for i in charsVocab]

IDX_PAD_NULL = vocab.index(PAD_NULL)

len(vocab), IDX_PAD_NULL
DEVICE = torch.device('cpu')#torch.device('cuda' if torch.cuda.is_available() else 'cpu')



def time_since(since):
    s = time.time() - since
    m = math.floor(s / 60)
    s -= m * 60
    return '%dm %ds' % (m, s)

def accuracy(model, data_loader):
    # Set the model to evaluation mode
    model.eval()

    correct = 0
    total = 0
    
    # Disable gradient computation during inference
    for (sequences, labels, _) in data_loader: # test_loader
        sequences = sequences.squeeze(0).to(DEVICE)
        labels = labels.squeeze(0).to(DEVICE).argmax(dim=1)
        # Forward pass
        predicted = model(sequences).argmax(dim=1)
            
        # Count total number of labels
        total += labels.size(0)
        
        # Count number of correct predictions
        correct += (predicted == labels).sum().item()
    model.train()
    # Calculate accuracy
    return 100 * correct / total
    # print('Accuracy: {:.2f}%'.format(accuracy))

def add_extraToken(texts, startToken=True, endToken=True):
    if startToken and endToken: return [PAD_START+text+PAD_END for text in texts]
    elif startToken: return [PAD_START+text for text in texts]
    elif endToken: return [text+PAD_END for text in texts]
    else: return texts

def remove_extraToken(texts:list[str])->list[str]:
    return [text.lower().replace('–','').replace('$','').replace('&','').replace('[','').replace(']',''
                                            ).replace('“','').replace('”','').replace('=','').replace('৷','').replace('`','').replace('ؑ', '').replace('}',''
                                            ).replace(PAD_START, '').replace(PAD_END, '').replace(PAD_NULL, '')
            for text in texts]

def preprocesser(texts: list[str], prePadding=False, startToken=True, endToken=True, batch_first=False):
    texts = add_extraToken(remove_extraToken(texts), startToken, endToken)
    text_ints = [[vocab.index(c) for c in text if c in vocab] for text in texts]
    # Apply pre-padding to each sequence
    if prePadding:
        max_length = max(len(seq) for seq in text_ints)
        padded_seqs = pad_sequence([torch.cat([torch.tensor([IDX_PAD_NULL]*(max_length - len(seq)), dtype=torch.int64), torch.LongTensor(seq)]) for seq in text_ints], batch_first=True)
    else:
        padded_seqs = pad_sequence([torch.LongTensor(seq) for seq in text_ints], batch_first=True, padding_value=IDX_PAD_NULL)
    
    return padded_seqs if batch_first else padded_seqs.T


class Encoder(nn.Module):
    def __init__(self, embedding_dim, hidden_size, num_layers, vocab_size, p=0, num_classes=11):
        super(Encoder, self).__init__()
        self.dropout = nn.Dropout(p)
        self.embedding = nn.Embedding(vocab_size, embedding_dim)
        self.lstm = nn.LSTM(embedding_dim, hidden_size, num_layers, dropout=p, bidirectional=False) 
        # self.fc1 = nn.Linear(hidden_size, hidden_size*2)
        # self.fc2 = nn.Linear(hidden_size*2, num_classes)
        self.fc = nn.Linear(hidden_size, num_classes)
        self.num_layers = num_layers
        self.hidden_size = hidden_size

    def forward(self, x):
        # (sequencen x batch_size)
        x = self.dropout(self.embedding(x)) # (sequencen x batch_size x embedding_dim)
        outputs, (hidden, cell) = self.lstm(x) # (sequencen x batch_size x hidden_size), ((num_layers x batch_size x hidden_size), (num_layers x batch_size x hidden_size))
        return self.fc(outputs[-1])
        # x = F.relu(self.fc1(outputs[-1]))
        # return self.fc2(x)
        
def model_predict(X):
    model.eval()
    with torch.no_grad():
        outputs =  model(preprocesser(X).to(DEVICE)).cpu().numpy()
    model.train()
    return outputs

# NORMALIZE and MAKE between 0 and 1
def prob(arr:np.ndarray, gap_adjuster:int=3)->np.ndarray:
    if len(arr.shape) == 1:
        arr = (arr-arr.min())/(arr.max()-arr.min())
        arr = arr**gap_adjuster
        return arr/arr.sum()
    else:
        arr = (arr-arr.min(axis=1).reshape(-1, 1))/(arr.max(axis=1)-arr.min(axis=1)).reshape(-1, 1)
        arr = arr**gap_adjuster
        return arr/arr.sum(axis=1).reshape(-1, 1)

EMBEDDING_SIZE = 50
HIDDEN_SIZE = 128
NUM_LAYERS = 2
P = 0.5

model = Encoder(EMBEDDING_SIZE, HIDDEN_SIZE, NUM_LAYERS, vocab_size=len(vocab), p=P, num_classes=11).to(device=DEVICE)
# Load the model
model.load_state_dict(torch.load('models_icdc\\gru.model.pth'))

# load the vectors
with open('models_icdc\\vectorizer.states.pkl','rb') as f: 
    vectorizer = pickle.load(f)

# load LogisticRegression
with open('models_icdc\\lr.model.pkl', 'rb') as f:
    lr_classifier = pickle.load(f)
    
# load naive_bayes
with open('models_icdc\\nb.model.pkl', 'rb') as f:
    nb_classifier = pickle.load(f)
    
# load RandomForestClassifier
with open('models_icdc\\rf.model.pkl', 'rb') as f:
    rf_classifier = pickle.load(f)
    
# load SVG
with open('models_icdc\\svm.model.pkl', 'rb') as f:
    svm_model = pickle.load(f)
    

def emsemble_infer_v1(texts:str|list[str], printable=False, proba=False):
    if isinstance(texts, str): texts = [texts]
    output = (
        lr_classifier.predict_proba(vectorizer.transform(texts)) +
        nb_classifier.predict_proba(vectorizer.transform(texts)) + 
        svm_model.predict_proba(vectorizer.transform(texts)) + 
        prob(model_predict(texts))
    )
    if proba:
        return output
    if printable:
        return [['ben', 'guj', 'hin', 'kan', 'mal', 'ori', 'pan', 'tam', 'tel', 'urd', 'eng'][i] for i in output.argmax(axis=1).tolist()]
    else:
        return output.argmax(axis=1)
    
def emsemble_infer_v2(texts:str|list[str], printable=False, proba=False):
    if isinstance(texts, str): texts = [texts]
    output = (
        prob(lr_classifier.predict_proba(vectorizer.transform(texts)), gap_adjuster=1) + 
        prob(nb_classifier.predict_proba(vectorizer.transform(texts)), gap_adjuster=1) + 
        prob(svm_model.predict_proba(vectorizer.transform(texts)), gap_adjuster=1) + 
        prob(model_predict(texts), gap_adjuster=6)
    )
    if proba: return output
    if printable:
        return [['ben', 'guj', 'hin', 'kan', 'mal', 'ori', 'pan', 'tam', 'tel', 'urd', 'eng'][i] for i in output.argmax(axis=1).tolist()]
    else:
        return output.argmax(axis=1)
    
def emsemble_infer_v3_last(texts:str|list[str], printable=False, proba=False):
    if isinstance(texts, str): texts = [texts]
    output = (
        lr_classifier.predict_proba(vectorizer.transform(texts)) +
        nb_classifier.predict_proba(vectorizer.transform(texts)) +
        prob(model_predict(texts), gap_adjuster=1)
    )
    if proba: return output
    if printable:
        return [['ben', 'guj', 'hin', 'kan', 'mal', 'ori', 'pan', 'tam', 'tel', 'urd', 'eng'][i] for i in output.argmax(axis=1).tolist()]
    else:
        return output.argmax(axis=1)

np.set_printoptions(suppress=True, precision=8)    

print(emsemble_infer_v1(["m mase kono ullekhayogya tapapravaher dasha anubhav kara yyani.", 'tum kya kar rahe ho yaar?', 'can you do somethig for me?'], 
                  proba=True))
print(emsemble_infer_v2(["m mase kono ullekhayogya tapapravaher dasha anubhav kara yyani.", 'tum kya kar rahe ho yaar?', 'can you do somethig for me?'], 
                  proba=True))
print(emsemble_infer_v3_last(["m mase kono ullekhayogya tapapravaher dasha anubhav kara yyani.", 'tum kya kar rahe ho yaar?', 'can you do somethig for me?'], 
                  proba=True))
    
print(emsemble_infer_v1(["m mase kono ullekhayogya tapapravaher dasha anubhav kara yyani.", 'tum kya kar rahe ho yaar?', 'can you do somethig for me?'], 
                  printable=True))
print(emsemble_infer_v2(["m mase kono ullekhayogya tapapravaher dasha anubhav kara yyani.", 'tum kya kar rahe ho yaar?', 'can you do somethig for me?'], 
                  printable=True))
print(emsemble_infer_v3_last(["m mase kono ullekhayogya tapapravaher dasha anubhav kara yyani.", 'tum kya kar rahe ho yaar?', 'can you do somethig for me?'], 
                  printable=True))