
from PyQt5.QtGui import *
from PyQt5.QtCore import *
from PyQt5.QtWidgets import QAbstractButton, QPushButton, QApplication, \
                                QWidget, QGridLayout, QMessageBox

import sys
import os
import time
import subprocess
import atexit

class PicButton(QAbstractButton):
    def __init__(self, pixmap, parent=None):
        super(PicButton, self).__init__(parent)
        self.pixmap = pixmap
        self.clicked.connect(self.onclick)
        

    def onclick(self):
        if x == 1:
            self.pixmap = X
        else:
            self.pixmap = O

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.drawPixmap(event.rect(), self.pixmap)


    def sizeHint(self):
        return QSize(0, 0)

class Button(QPushButton):
    def __init__(self, i, j, text="", parent=None):
        super(Button, self).__init__(parent)
        self.clicked.connect(lambda f : self.onclick("O"))
        #self.setSizePolicy ( QSizePolicy.Expanding, QSizePolicy.Expanding)
        #self.resize(1, 1)
        self.setText(text)
        self.i, self.j = i, j


    def onclick(self, pl):
        print "---> ", self.i, self.j 
        global player, L, R
        if pl == "O" and player == "X":
            msgBox = QMessageBox()
            msgBox.setText("AI's turn, bitch!")
            msgBox.exec_()
            return
        self.setStyleSheet("background-color: #2f4f4f; color:red")
        self.setText(player)
        if player == "O":
            
            player = "X" 
            i,j=self.i-L[0], self.j-L[1]
            applyMove(i, j)
            writefile(i, j)
            print i, j
        else:
            player = "O"

    def sizeHint(self):
        return QSize(20, 20)

player = "X"
ai_turn = 1

def writefile(i, j):
    human = open("human", "w")
    human.write(str(i) + " " + str(j))
    human.close()

def applyMove(i, j):
    global L, R
    h, w = R[0] - L[0], R[1] - L[1]
    if (i == 0):
        L[0]-=1;
    
    if (i == h):
        R[0]+=1;
    
    if (j == 0):
        L[1]-=1;
    
    if (j == w):
        R[1]+=1;


N = 40
L, R = [N//2, N//2], [N//2, N//2]

def run():
    app = QApplication(sys.argv)
    window = QWidget()
    g = QGridLayout(window)


    g.setSpacing(10)
    btns = [[0] * N for i in range(N)]

    for i in range(N):
        for j in range(N):
            b = Button(i, j)
            btns[i][j] = b
            g.addWidget(b, i, j, 1, 1)
    window.show()
    
    
    os.system("echo -2 -2 > ai && echo -1 -1 > human")
    
    proc = subprocess.Popen(["./tictac"])
    atexit.register(proc.kill)
    atexit.register(sys.exit)
    
    #os.system("./tictac")

    msgBox = QMessageBox();

    last = (-2, -2)
    winner = -1

    
    while winner == -1:
        QApplication.processEvents()
        time.sleep(0.1)
        ai = open("ai", "r+")

        s = ai.readline()
        if s=="":
            
            continue
        i, j = list(map(int, s.split()))

        ai.seek(0)
        ai.write('')

        if i == -2:
            if j == -2:
                msgBox.setText("Incorrect move, bitch!");
                msgBox.exec_(); 
            else:
                winner = j
                print ''
        else:
            btns[L[0] + i][L[1] + j].onclick("X")
            applyMove(i, j)
            print "-- ", i, j
            print "new ", L

        ai.truncate()
        #sai.close()
        #human.close() 
        #time.sleep(0.1)
    
        
    msgBox.setText("Player {0} wins".format(str(winner)))
    msgBox.exec_()
    sys.exit(app.exec_())

if __name__ == "__main__":
    run()