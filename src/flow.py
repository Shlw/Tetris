#!/usr/bin/env python3
import tensorflow as tf
import os
from functools import reduce
import random

sess = tf.InteractiveSession()

N = 30
M = 30
L = 5
l=[None]*L
w=[None]*L
b=[None]*L
x = l[0] = tf.placeholder(tf.float32, [None, M])
w[0] = tf.Variable(tf.random_uniform([M, N], -1, 1))
b[0] = tf.Variable(tf.random_uniform([N], -1, 1))
for i in range(1, L):
    l[i] = tf.tanh(tf.matmul(l[i-1], w[i-1]) + b[i-1])
    if i != L-1:
        w[i] = tf.Variable(tf.random_uniform([N, N], -1, 1))
        b[i] = tf.Variable(tf.random_uniform([N], -1, 1))
w[L-1] = tf.Variable(tf.random_uniform([N, 1], -100, 100))
b[L-1] = tf.Variable(tf.random_uniform([1], -100, 100))
y = tf.matmul(l[L-1], w[L-1]) + b[L-1]

y_ = tf.placeholder(tf.float32, [None, 1])
err0 = tf.reduce_mean(tf.squared_difference(y, y_))
acc = tf.sqrt(err0)
err = acc
'''
reg = tf.constant(0.0)
for i in range(0, L-1):
    reg = reg + tf.reduce_sum(tf.square(w[i]))
lbd = 0.07
err = tf.sqrt(err0 + lbd * reg)
'''
train_step = tf.train.GradientDescentOptimizer(0.003).minimize(err)

tf.global_variables_initializer().run()

fdata = open('data.txt','r')
data = [[float(x) for x in i.split()] for i in fdata.readlines()]
random.shuffle(data)
pdata = 0
ptest = 0
test_data = data[-100:]
data = data[:-100]
fdata.close()

def get_data(n):
    global pdata
    ansx = []
    ansy = []
    for i in range(n):
        if pdata >= len(data):
            ans = data[random.randint(0, len(data)-1)]
        else:
            ans = data[pdata]
            pdata += 1
        ansx += [ans[:-1]]
        ansy += [ans[-1:]]
    return ansx, ansy

def get_test(n):
    global ptest
    ansx = []
    ansy = []
    for i in range(n):
        ans = test_data[ptest]
        ptest += 1
        ansx += [ans[:-1]]
        ansy += [ans[-1:]]
    return ansx, ansy
xts, yts = get_data(len(test_data))

def test(noOut):
    accu = sess.run(acc, feed_dict={x:xts, y_:yts})
    noOut = False
    if noOut and accu < 16:
        print('acc=%g'%accu)
        print('N=%d,M=%d,L=%d'%(N,M,L))
        for i in range(L):
            print('w[%d]='%i,sess.run(w[i]))
            print('b[%d]='%i,sess.run(b[i]))
    elif not noOut:
        print('acc=%g'%accu)

for _ in range(1000):
    xs, ys = get_data(20)
    sess.run(train_step, feed_dict={x:xs, y_:ys})
    if _ % 100:
        test(True)
test(False)
