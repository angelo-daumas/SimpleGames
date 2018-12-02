from tkinter import *
from random import randint
from collections import deque

class cobrinha():

       class deque(deque):
              def random(self):
                     xi = randint(1,len(self))
                     self.rotate(xi)
                     return self.pop()

       class direction(list):

              @property
              def x(self):
                     return self[0]

              @x.setter
              def x(self, value):
                     self[0] = value

              @property
              def y(self):
                     return self[1]

              @y.setter
              def y(self, value):
                     self[1] = value



              def __mul__(self, number):
                     return (self[0]*number, self[1]*number)


       def __init__(self,master):
              self.master = master
              self.width = 502
              self.height = 502
              self.canvas = Canvas(master,
                                   width = self.width,
                                   height = self.height,
                                   bg = 'black')
              self.canvas.pack()
              self.borda = self.canvas.create_rectangle(2,2,
                                                      self.width,
                                                      self.height)
              self.cobra = []
              self.cabeca = self.canvas.create_rectangle(2,2,
                                                        22,22,
                                                        fill = '#0066FF')
              self.cobra.append(self.cabeca)

              self.direcao = type(self).direction((1, 0))
              self.aumentar()
              self.aumentar()

              self.criarComida()

              self.canvas.bind('<Right>',self.direita)
              self.canvas.bind('<Left>',self.esquerda)
              self.canvas.bind('<Up>',self.cima)
              self.canvas.bind('<Down>',self.baixo)

              self.canvas.focus_set()
              self.turned = False

       def direita(self,evento):
              if self.direcao.x == 0 and not self.turned:
                     self.direcao.x = 1
                     self.direcao.y = 0
                     self.turned = True

       def esquerda(self,evento):
              if self.direcao.x == 0 and not self.turned:
                     self.direcao.x = -1
                     self.direcao.y = 0
                     self.turned = True

       def cima(self,evento):
              if self.direcao.y == 0 and not self.turned:
                     self.direcao.x = 0
                     self.direcao.y = -1
                     self.turned = True

       def baixo(self,evento):
              if self.direcao.y == 0 and not self.turned:
                     self.direcao.x = 0
                     self.direcao.y = 1
                     self.turned = True

       def criarComida(self):
              raio = 5
              xposicoes = type(self).deque(range(7,490,20))
              yposicoes = type(self).deque(range(7,490,20))

              xp = xposicoes.random()
              yp = yposicoes.random()

              while(self.canvas.find_overlapping(xp, xp+1, yp, yp+1)):
                     xp = xposicoes.random()
                     yp = yposicoes.random()



              self.comida = self.canvas.create_oval(xp,yp,
                                                    2*raio+xp,
                                                    2*raio+yp,
                                                    fill = '#00FFFF')


       def aumentar(self):
              x1,y1,x2,y2 = self.canvas.coords(self.cobra[-1])

              x1 = x1-self.direcao.x*20
              x2 = x2-self.direcao.x*20

              seg = self.canvas.create_rectangle(x1,y1,
                                                 x2,y2,
                                                 fill = '#000066')
              self.cobra.append(seg)

       def mover(self):

              for i in range(len(self.cobra)-1, 0, -1):
                               self.canvas.coords(self.cobra[i], *self.canvas.coords(self.cobra[i-1]))

              
              self.canvas.move(self.cabeca, *self.direcao*20)
              x1,y1,x2,y2 = self.canvas.coords(self.cabeca)
              self.turned = False
              self.comer()

              if x2 > self.width:
                     x1 = 2
                     x2 = 22
              if x1 < 2:
                     x1 = self.width - 20
                     x2 = self.width
              if y2 > self.width:
                     y1 = 2
                     y2 = 22
              if y1 < 2:
                     y1 = self.width - 20
                     y2 = self.width

              self.canvas.coords(self.cabeca,x1,y1,x2,y2)
              return not len(self.canvas.find_overlapping(x1+1,y1+1,x2-1,y2-1)) > 1



       def comer(self):
              x1cab,y1cab,x2cab,y2cab = self.canvas.coords(self.cabeca)
              x1com,y1com,x2com,y2com = self.canvas.coords(self.comida)
              if (x1cab<=x1com<x2cab)and (y1cab<=y1com<y2cab):
                     self.aumentar()
                     self.canvas.delete(self.comida)
                     self.criarComida()

       def jogo(self):
              if self.mover():
                     self.canvas.after(75,self.jogo)
              else:
                     self.canvas.delete("all")
                     self.canvas.create_text(self.width/2, self.height/2,
                                             fill="#0066FF",
                                             font="Times 20 italic bold",
                                             text="Game over.")




root = Tk()
jogo = cobrinha(root)
jogo.jogo()
root.mainloop()
