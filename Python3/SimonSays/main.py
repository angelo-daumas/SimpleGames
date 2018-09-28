import tkinter as tk
import random
import time

# CONSTANTS
# ======================

COLORS = ['yellow','red','green','blue']
YELLOW = COLORS.index('yellow')
RED    = COLORS.index('red')
GREEN  = COLORS.index('green')
BLUE   = COLORS.index('blue')

BUTTON_SIZE = {'width':10,
               'height':5}

# ======================

class color_button(tk.Button):

    def __init__(self, parent, color, *args, **kwargs):
        kwargs['command'] = self._on_click
        kwargs['bg']      = COLORS[color]
        tk.Button.__init__(self, *args, **kwargs)
        
        self._parent = parent
        self._color  = color

    def _on_click(self):
        self._parent.respostas.append(self._color)

class button_line():
    
    def __init__(self, frame):
        self._master = frame
        self._buttons = []

    def pack(self, **kwargs):
        self._master.pack(**kwargs)
    
    def add_button(self, **kwargs):
        self._buttons.append(tk.Button(self._master, **kwargs))

    def add_color_button(self, parent, color, **kwargs):
        self._buttons.append(color_button(parent, color, self._master, **kwargs))

    def __iter__(self):
        for button in self._buttons:
            yield button

class button_holder():

    def __init__(self, frame):
        self._master = frame
        self._top    = button_line(tk.Frame(frame))
        self._bottom = button_line(tk.Frame(frame))

    def pack(self, **kwargs):
        self._master.pack(**kwargs)
        self._top.pack(side = tk.TOP)
        self._bottom.pack(side = tk.TOP)
        
    @property
    def top(self):
        return self._top

    @property
    def bottom(self):
        return self._bottom

    def __iter__(self):
        yield self.top
        yield self.bottom


class simonsays():

    def create_button(self, frame, text, **kwargs):
        return tk.Button(frame, text=text, command = self._commands[text], **kwargs)

    def __init__(self, master):
        self._commands = {'Inicio': self.new_seq,
                         'Continuar': self.cont_seq,
                         'Verificar': self.verify_seq}

        self._buttons = button_holder(tk.Frame(master))
        self._buttons.pack(side = tk.LEFT)

        self._frame_right = tk.Frame(master)
        self._frame_right.pack(side = tk.RIGHT)

        self._label = tk.Label(self._frame_right, bg = 'black', **BUTTON_SIZE)
        self._label.pack()
        self._inicio = self.create_button(self._frame_right, 'Inicio')
        self._inicio.pack()
        self._continuar = self.create_button(self._frame_right, 'Continuar')
        self._continuar.pack()
        self._verificar = self.create_button(self._frame_right, 'Verificar')
        self._verificar.pack()

        self._buttons.top.add_color_button   (self, YELLOW, **BUTTON_SIZE)
        self._buttons.top.add_color_button   (self, RED,    **BUTTON_SIZE)
        self._buttons.bottom.add_color_button(self, GREEN,  **BUTTON_SIZE)
        self._buttons.bottom.add_color_button(self, BLUE,   **BUTTON_SIZE)

        sides = [tk.LEFT, tk.RIGHT]
        for side in self._buttons:
            index = 0
            for button in side:
                button.pack(side = sides[index])
                index += 1

    def show_seq(self):
        for i in range(len(self.gabarito)):
            cor = self.gabarito[i]
            self._label['bg']= COLORS[cor]
            #self._label['text']= str(i+1)
            self._frame_right.update()
            time.sleep(1)
            self._label['bg']= 'black'
            self._frame_right.update()
            time.sleep(0.1)
        #self._label['text']=''

    def new_seq(self):
        self.verified = False
        self.gabarito = [random.randint(0,3) for i in range(2)]
        self.respostas = []
        self.show_seq()
    
    def cont_seq(self):
        if self.verified:
            self.gabarito.append(random.randint(0,3))
            self.show_seq()
            self.respostas = []
            self.verified = False
        
    def verify_seq(self):
        if self.respostas!=self.gabarito:
            self.respostas = []
        else:
            self.verified = True        


# Main
# ======================

def main():
    root = tk.Tk()
    jogo = simonsays(root)
    root.mainloop()

if __name__ == "__main__":
    main()
