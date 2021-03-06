import pandas as pd
import numpy as np
from sklearn.metrics import roc_auc_score

from sklearn.metrics import roc_curve
from sklearn.metrics import auc
import matplotlib
matplotlib.use('Agg') #for use without ssh -Y
import matplotlib.pyplot as plt

import tensorflow as tf
# for tf v2, fgo into v1 compatibility mode
if tf.__version__.startswith("2."):
    tf = tf.compat.v1
#tf.disable_eager_execution()

sess = tf.Session()

from keras import backend as K
K.set_session(sess)


#define model
def DefineModel(size=20):

    #Define model with Keras's functional API, not sequential, to combine network types
    from keras.layers import Input,Conv2D,Dense,concatenate,Flatten
    from keras.models import Model
    size=20
    inputClusterImages = Input(shape=(size,size,1))
    inputHitVariables = Input(shape=(1,))

    #CNN on cluster images
    CNN = Conv2D(32, kernel_size=(4,4),padding='same',activation='relu')(inputClusterImages)
    CNN = Conv2D(8, kernel_size=(4,4),padding='same',activation='relu')(CNN)
    CNN = Conv2D(4, kernel_size=(4,4),padding='same',activation='relu')(CNN)
    CNN = Flatten()(CNN)

    CNN = Model(inputs=inputClusterImages,outputs=CNN)
    
    #NN on the Hit variables
    NN = Dense(5,activation='relu')(inputHitVariables)
    NN = Dense(5,activation='relu')(NN)
    NN = Model(inputs=inputHitVariables,outputs=NN)

    combined_outputs = concatenate([CNN.output,NN.output])
    combination_NN = Dense(2,activation='softmax',name='Output')(combined_outputs)

    full_classifier = Model(inputs=[CNN.input,NN.input],outputs=combination_NN)
    full_classifier.compile(loss='categorical_crossentropy', optimizer="adam", metrics = ["accuracy"])
    full_classifier.summary()

    return full_classifier


def SaveModel(model):
    print([node.op.name for node in model.outputs])
    
    constant_graph = tf.graph_util.convert_variables_to_constants(
                sess, sess.graph.as_graph_def(), ["Output/Softmax"])
    tf.train.write_graph(constant_graph, "", "constantgraph.pb", as_text=False)

def to_image(df,size=20):
    pixels = ["pixel_{0}".format(i) for i in range(size**2)]
    return  np.expand_dims(np.expand_dims(df[pixels], axis=-1).reshape(-1,size,size), axis=-1)

def ConvertH5ModelToPB(filename):
    K.set_session(sess)
    sess.run(tf.global_variables_initializer())
    mod = tf.keras.models.load_model(filename)
    SaveModel(mod)

#get data
def GetData(filename,testTrainFrac=.5):
    from keras.utils import to_categorical
    df = pd.read_hdf(filename)

    #need to reindex for some reason...
    df = df.reindex(index=np.arange(df.shape[0]),columns=df.keys())
    
    print(sum(df["isSharedHit"]==1),sum(df["isSharedHit"]!=1))

    df_train = df.sample(frac=testTrainFrac)
    df_test = df.drop(df_train.index)

    images_train = to_image(df_train)
    images_test = to_image(df_test)

    otherVariables_train = df_train["totalADCcount"]/1e4
    otherVariables_test = df_test["totalADCcount"]/1e4
    
    train_data = [images_train, otherVariables_train]
    test_data = [images_test, otherVariables_test]
    
    train_labels = to_categorical(df_train["isSharedHit"]==1)
    test_labels = to_categorical(df_test["isSharedHit"]==1)

    print("training data: s={}, b={}".format(sum(train_labels[:,1]),sum(train_labels[:,0])))
    print("testing data: s={}, b={}".format(sum(test_labels[:,1]),sum(test_labels[:,0])))

    return train_data, test_data, train_labels, test_labels
    
def TrainModel(classifier,data,labels,epochs=10,validation_split=0.1):
    classifier.fit(data, labels, epochs=epochs, validation_split=validation_split)


def MakeROC(classifier,discriminants,labels):
    fpr_keras, tpr_keras, thresholds_keras = roc_curve(labels[:,1], discriminants[:,1])
    auc = np.trapz(tpr_keras,fpr_keras) 
    print("ROC curve area: {:,.3f}".format(auc))
    
    plt.plot(fpr_keras, tpr_keras, label='Keras (area = {:.3f})'.format(auc))
    plt.xlabel('False positive rate')
    plt.ylabel('True positive rate')
    plt.title('ROC curve (area = {:.3f})'.format(auc))
    plt.savefig("ROC.png")
    plt.close()


def PlotDiscriminants(train_probs,train_Y,test_probs,test_Y):
    train_signal_probs = train_probs[train_Y[:,0]==1]
    train_bkg_probs = train_probs[train_Y[:,0]!=1]

    test_signal_probs = test_probs[test_Y[:,0]==1]
    test_bkg_probs = test_probs[test_Y[:,0]!=1]


    plt.hist(test_signal_probs, color = 'b', label = 'Signal (test)', range = (0,1), bins = 30,histtype='step')
    plt.hist(test_bkg_probs, color = 'r', label = 'Background (test)', range = (0,1), bins = 30,histtype='step')
    plt.hist(train_signal_probs,linestyle='--', alpha = 0.5, color = 'b', label = 'Signal (train)', range = (0,1), bins = 30,histtype='step')
    plt.hist(train_bkg_probs,linestyle='--', color = 'r', alpha = 0.5, label = 'Background (train)', range = (0,1), bins = 30,histtype='step')
    plt.legend(loc='best')
    plt.xlabel('Discriminant')
    plt.title('CNN Signal and Background Discriminants')
    plt.savefig("disc.png")
    plt.close()


def EvaluateModel(classifier,data,labels):
    discriminants = classifier.predict(data)
    MakeROC(classifier,discriminants,labels)
    return discriminants[:,1]

def SaveScores(filename,classifier,classifier_name="score"):
    from keras.utils import to_categorical
    df = pd.read_hdf(filename)
    images = to_image(df)
    print(df.shape)
    otherVariables = np.zeros((df.shape[0],1))
    inputs = [images,otherVariables]
    discriminants = classifier.predict(inputs)[:,1]
    print(discriminants.shape)

    df[classifier_name] = discriminants
    df.to_hdf("evaluated_"+filename,"eval")


def Run():
    train_X, test_X, train_Y, test_Y = GetData("/uscms_data/d3/bbonham/TrackerProject/Output_of_Postprocess/AllHits/NormalizedCharge/output_final.h5")
    model = DefineModel()
    TrainModel(model,train_X,train_Y,epochs=30)
    SaveModel(model)
    train_probs = EvaluateModel(model,train_X,train_Y)
    test_probs = EvaluateModel(model,test_X,test_Y)
    PlotDiscriminants(train_probs,train_Y, test_probs,test_Y)

    #SaveScores("/uscms_data/d3/bbonham/TrackerProject/Output_of_Postprocess/AllHits/NormalizedCharge/output_final.h5",model)
    
Run()
