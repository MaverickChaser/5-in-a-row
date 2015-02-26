
from PyQt5.QtGui import *
from PyQt5.QtCore import *
from PyQt5.QtWidgets import QAbstractButton, QPushButton, QApplication, \
                                QWidget, QGridLayout, QMessageBox, QSizePolicy

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
        self.setSizePolicy ( QSizePolicy.Expanding, QSizePolicy.Expanding)
        #self.resize(1, 1)
        self.setText(text)
        self.i, self.j = i, j


    def onclick(self, pl):
        if self.text() != "":
            print "SHIT"
            return
        global player, L, R, O
        if pl == "O" and player == "X":
            msgBox = QMessageBox()
            msgBox.setText("AI's turn, bitch!")
            msgBox.exec_()
            return
        self.setStyleSheet("background-color: #2f4f4f; color:red")
        self.setText(player)
        if player == "O":
            player = "X"
            moves.append((self.i, self.j, L, R))
            i, j = self.i-L[0], self.j-L[1]
            apply_move(i, j)
            write_file(i, j)
            O += 1
        else:
            player = "O"

    #def sizeHint(self):
     #   return QSize(20, 20)

player = "X"
ai_turn = 1
last_move = (-2, -2)
N = 40
L, R = [N//2, N//2], [N//2, N//2]
btns = [[0] * N for i in range(N)]
X = 1
O = 1
moves = []

def write_file(i, j):
    human = open("human", "w")
    human.write(str(O) + " " + str(i) + " " + str(j))
    human.close()

def apply_move(i, j):
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

def show_last_move():
    print 
    btns[moves[-1][0]][moves[-1][1]].setStyleSheet("background-color: #2f4f4f; color:green")
    #QApplication.processEvents()
    #for i in range(10000): pass
    #btns[last_move[0]][last_move[1]].setStyleSheet("background-color: #2f4f4f; color:red")

def run():
    global last_move, X
    app = QApplication(sys.argv)
    window = QWidget()
    #window.SetSizeConstraint(QtGui.QLayout.SetFixedSize)  # no resize arrows
    g = QGridLayout(window)

    #g.setSpacing(0)
    for i in range(N):
        for j in range(N):
            b = Button(i, j)
            b.setFixedSize(20, 20)

            btns[i][j] = b
            g.addWidget(b, i, j, 1, 1)
    button = QPushButton("Last move")
    button.clicked.connect(show_last_move)
    g.addWidget(button, N, 0, 1, N, Qt.AlignLeft)
    g.addWidget(QPushButton("Back"), N, 0, 1, N, Qt.AlignRight)
    window.show()
    
    
    os.system("echo -2 -2 -2 > ai && echo -1 -1 -1 > human")
    
    
    msgBox = QMessageBox();
    winner = -1
    
    proc = subprocess.Popen(["./tictac"])
    atexit.register(proc.kill)
    atexit.register(sys.exit)
    
    while winner == -1:
        QApplication.processEvents()
        time.sleep(0.001)
        ai = open("ai", "r")

        s = ai.readline()
        if s == "":
            continue

        num, i, j = list(map(int, s.split()))

        if i == -2:
            if j == -2:
                msgBox.setText("Incorrect move, bitch!");
                msgBox.exec_(); 
            else:
                winner = j
                print ''
        elif num == X:
            X += 1
            moves.append((L[0] + i, L[1] + j, L, R))
            btns[L[0] + i][L[1] + j].onclick("X")
            apply_move(i, j)

        ai.close()
        #human.close() 
        #time.sleep(0.1)
    

    msgBox.setText("Player {0} wins".format(str(winner)))
    msgBox.exec_()
    sys.exit(app.exec_())

if __name__ == "__main__":
    run()