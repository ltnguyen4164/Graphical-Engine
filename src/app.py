import numpy as np
import tkinter as tk
import os
import ctypes
from tkinter import ttk
from tkinter import simpledialog, filedialog

path = os.path.abspath("engine.dll")
engine = ctypes.CDLL(path)

class Line(ctypes.Structure):
    _fields_ = [
        ("x1", ctypes.c_float),
        ("y1", ctypes.c_float),
        ("x2", ctypes.c_float),
        ("y2", ctypes.c_float),
    ]

engine.load_file.argtypes = [ctypes.c_char_p]
engine.load_file.restype = None

engine.get_line_count.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int]
engine.get_line_count.restype = ctypes.c_int

engine.get_lines.argtypes = [
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int,
    ctypes.POINTER(Line)
]
engine.get_lines.restype = None

engine.rotate.argtypes = [ctypes.c_float, ctypes.c_char]
engine.rotate.restype = None
engine.rotate_axis.argtypes = [ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float]
engine.rotate_axis.restype = None

engine.scale.argtypes = [ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float]
engine.scale.restype = None

engine.translate.argtypes = [ctypes.c_float, ctypes.c_float, ctypes.c_float]
engine.translate.restype = None

engine.load_camera_file.argtypes = [ctypes.c_char_p]
engine.load_camera_file.restype = None

engine.get_camera_count.argtypes = []
engine.get_camera_count.restype = ctypes.c_int

engine.get_camera_viewport.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_float)]
engine.get_camera_viewport.restype = None

engine.get_camera_name.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
engine.get_camera_name.restype = None

class cl_world:
    def __init__(self):
        # Read and load camera at startup
        camera_file = os.path.abspath("sample/cameras.txt")
        self.load_camera_file(str(camera_file))
        
        # Initialize the main window
        self.root = tk.Tk()
        self.root.title("Resizable Window")

        # Set the window geometry and make it resizable
        self.root.geometry("650x600")
        self.root.resizable(True, True)

        ################### Top Panel ##########################
        # Create a top frame for the button
        self.top_frame = tk.Frame(self.root)
        self.top_frame.pack(side=tk.TOP, fill=tk.X)

        self.file_frame = tk.Frame(self.top_frame)
        self.file_frame.pack(side=tk.TOP, anchor="center", pady=10)
        tk.Label(self.file_frame, text="Filename:").pack(side=tk.LEFT)
        self.file_path_label = tk.Label(self.file_frame, text="")
        self.file_path_label.pack(side=tk.LEFT, padx=5)

        # Create a button in the top panel
        self.browse_button = tk.Button(self.file_frame, text="Browse", fg="blue", command=self.browse_file_clicked)
        self.browse_button.pack(side=tk.LEFT)
        self.draw_button = tk.Button(self.file_frame, text="Draw", command=self.draw_button_clicked)
        self.draw_button.pack(side=tk.LEFT)

        ################### Rotation Panel #####################
        # Create controls for rotation
        self.rotation_frame = tk.Frame(self.root)
        self.rotation_frame.pack(side=tk.TOP, fill=tk.X, padx=10, pady=5)

        tk.Label(self.rotation_frame, text="Rotation Axis:").pack(side=tk.LEFT)
        self.axis_var = tk.StringVar(value="X")
        tk.Radiobutton(self.rotation_frame, text="X", variable=self.axis_var, value="X", command=self.toggle_AB).pack(side=tk.LEFT)
        tk.Radiobutton(self.rotation_frame, text="Y", variable=self.axis_var, value="Y", command=self.toggle_AB).pack(side=tk.LEFT)
        tk.Radiobutton(self.rotation_frame, text="Z", variable=self.axis_var, value="Z", command=self.toggle_AB).pack(side=tk.LEFT)
        tk.Radiobutton(self.rotation_frame, text="Line AB", variable=self.axis_var, value="AB", command=self.toggle_AB).pack(side=tk.LEFT)

        tk.Label(self.rotation_frame, text="A:").pack(side=tk.LEFT)
        self.point_A = tk.Entry(self.rotation_frame, width=10)
        self.point_A.insert(0, "0.0,0.0,0.0")
        self.point_A.pack(side=tk.LEFT)
        self.point_A.config(state=tk.DISABLED)

        tk.Label(self.rotation_frame, text="B:").pack(side=tk.LEFT)
        self.point_B = tk.Entry(self.rotation_frame, width=10)
        self.point_B.insert(0, "1.0,1.0,1.0")
        self.point_B.pack(side=tk.LEFT)
        self.point_B.config(state=tk.DISABLED)

        tk.Label(self.rotation_frame, text="Degree:").pack(side=tk.LEFT)
        self.degree_var = tk.Spinbox(self.rotation_frame, from_=0, to=360, increment=1, width=5)
        self.degree_var.pack(side=tk.LEFT)
        self.degree_var.delete(0, "end")
        self.degree_var.insert(0, "90")

        rotate_button = tk.Button(self.rotation_frame, text="Rotate", command=self.rotate)
        rotate_button.pack(side=tk.LEFT)
        
        ################### Scale Panel ########################
        # Create controls for scaling
        self.scale_frame = tk.Frame(self.root)
        self.scale_frame.pack(side=tk.TOP, fill=tk.X, padx=10, pady=5)

        tk.Label(self.scale_frame, text="Scale about point:").pack(side=tk.LEFT)
        self.scale_point = tk.Entry(self.scale_frame, width=15)
        self.scale_point.insert(0, "0.0,0.0,0.0")
        self.scale_point.pack(side=tk.LEFT)

        tk.Label(self.scale_frame, text="Scale Ratio:").pack(side=tk.LEFT)
        self.scale_var = tk.StringVar(value="all")
        
        tk.Radiobutton(self.scale_frame, text="All", variable=self.scale_var, value="all", command=self.toggle_scale_all).pack(side=tk.LEFT)
        self.scale_all_var = tk.Entry(self.scale_frame, width=5)
        self.scale_all_var.insert(0, "1")
        self.scale_all_var.pack(side=tk.LEFT)
        self.scale_all_var.config(state=tk.NORMAL)

        tk.Radiobutton(self.scale_frame, text="[Sx,Sy,Sz]", variable=self.scale_var, value="xyz", command=self.toggle_scale_all).pack(side=tk.LEFT)
        self.scale_xyz = tk.Entry(self.scale_frame, width=10)
        self.scale_xyz.insert(0, "1,1,1")
        self.scale_xyz.pack(side=tk.LEFT)
        self.scale_xyz.config(state=tk.DISABLED)

        scale_button = tk.Button(self.scale_frame, text="Scale", command=self.scale)
        scale_button.pack(side=tk.LEFT)
        
        ################### Translation Panel ##################
        # Create controls for translation
        self.translation_frame = tk.Frame(self.root)
        self.translation_frame.pack(side=tk.TOP, fill=tk.X, padx=10, pady=5)

        tk.Label(self.translation_frame, text="Translation ([dx, dy, dz]):").pack(side=tk.LEFT)
        self.translation_var = tk.Entry(self.translation_frame, width=15)
        self.translation_var.insert(0, "1.0, 1.0, 1.0")
        self.translation_var.pack(side=tk.LEFT)

        translate_button = tk.Button(self.translation_frame, text="Translate", command=self.translate)
        translate_button.pack(side=tk.LEFT)
        
        ################### Canvas #############################
        # Create canvas to draw on
        self.canvas = tk.Canvas(self.root, bg="light goldenrod")
        self.canvas.pack(fill=tk.BOTH, expand=True)
        # Bind the resize event to redraw the canvas when window is resized
        self.canvas.bind("<Configure>", self.canvas_resized)

        #################### Bottom Panel #######################
        # Create a bottom frame for displaying messages
        self.bottom_frame = tk.Frame(self.root)
        self.bottom_frame.pack(side=tk.BOTTOM, fill=tk.X)
        # Create a label for showing messages
        self.message_label = tk.Label(self.bottom_frame, text="")
        self.message_label.pack(padx=10, pady=10)
    
    def browse_file_clicked(self):
        self.file_path = filedialog.askopenfilename(filetypes=[("allfiles", "*"), ("pythonfiles", "*.txt")])
        self.file_path_label.config(text=self.file_path)
        self.message_label.config(text=self.file_path)
        self.load_file(self.file_path)

    def draw_button_clicked(self):
        self.draw_objects()

    def toggle_AB(self):
        if self.axis_var.get() == "AB":
            self.point_A.config(state=tk.NORMAL)
            self.point_B.config(state=tk.NORMAL)
        else:
            self.point_A.config(state=tk.DISABLED)
            self.point_B.config(state=tk.DISABLED)
    
    def toggle_scale_all(self):
        if self.scale_var.get() == "all":
            self.scale_all_var.config(state=tk.NORMAL)
            self.scale_xyz.config(state=tk.DISABLED)
        elif self.scale_var.get() == "xyz":
            self.scale_xyz.config(state=tk.NORMAL)
            self.scale_all_var.config(state=tk.DISABLED)
        else:
            self.scale_all_var.config(state=tk.DISABLED)
            self.scale_xyz.config(state=tk.DISABLED)

    def canvas_resized(self, event=None):
        if self.canvas.find_all():
            self.draw_objects(event)

    def load_camera_file(self, filename):
        engine.load_camera_file(filename.encode('utf-8'))

    def draw_camera(self):
        # Clear the canvas if necessary
        self.canvas.update()

        canvas_width = self.canvas.winfo_width()
        canvas_height = self.canvas.winfo_height()

        count = engine.get_camera_count()
        for i in range(count):
            viewport = (ctypes.c_float * 4)()
            engine.get_camera_viewport(i, viewport)

            xmin, ymin, xmax, ymax = viewport

            name_buffer = ctypes.create_string_buffer(100)
            engine.get_camera_name(i, name_buffer, 100)
            camera_name = name_buffer.value.decode()

            x1 = xmin * canvas_width
            x2 = xmax * canvas_width

            y1 = (ymin) * canvas_height
            y2 = (ymax) * canvas_height

            self.canvas.create_rectangle(x1, y1, x2, y2, fill="white", outline="black")
            self.canvas.create_text(x1 + 14, y1 + 7, text=camera_name, fill="black", font=("Arial", 10))
            self.canvas.update()
    
    def load_file(self, filename):
        engine.load_file(filename.encode('utf-8'))

    def draw_objects(self, event=None):
        # Clear the canvas if necessary
        self.canvas.delete("all")
        self.draw_camera()
        
        # Get canvas dimensions
        width = self.canvas.winfo_width()
        height = self.canvas.winfo_height()

        cam_count = engine.get_camera_count()
        for i in range(cam_count):
            viewport_arr = (ctypes.c_float * 4)()
            engine.get_camera_viewport(i, viewport_arr)

            vxmin, vymin, vxmax, vymax = viewport_arr
            self.canvas.create_rectangle(vxmin * width, (vymax) * height, vxmax * width, (vymin) * height, outline="blue")
            
            count = engine.get_line_count(width, height, i)

            lines_array = (Line * count)()
            engine.get_lines(width, height, i, lines_array)

            for line in lines_array:
                self.canvas.create_line(line.x1, line.y1, line.x2, line.y2)
        
        self.canvas.update()

    def rotate(self):
        degree = float(self.degree_var.get())
        axis = self.axis_var.get()

        if axis in ['X', 'Y', 'Z']:
            engine.rotate(degree, axis.encode("utf-8"))
        elif axis == 'AB':
            A_str = self.point_A.get()
            B_str = self.point_B.get()

            ax, ay, az = [float(val.strip()) for val in A_str.split(",")]
            bx, by, bz = [float(val.strip()) for val in B_str.split(",")]

            engine.rotate_axis(degree, ax, ay, az, bx, by, bz)
        
        self.draw_objects()

    def scale(self):
        scale_point_str = self.scale_point.get()
        scale_point = [float(val.strip()) for val in scale_point_str.split(",")]

        if self.scale_var.get() == "all":
            scale_ratio = float(self.scale_all_var.get())
            scale_ratios = [scale_ratio, scale_ratio, scale_ratio]
        elif self.scale_var.get() == "xyz":
            scale_ratio_str = self.scale_xyz.get()
            scale_ratios = [float(val.strip()) for val in scale_ratio_str.split(",")]
        
        engine.scale(scale_point[0], scale_point[1], scale_point[2], scale_ratios[0], scale_ratios[1], scale_ratios[2])

        self.draw_objects()

    def translate(self):
        trans_str = self.translation_var.get()
        trans_val = trans_str.split(",")
        trans_flt = [float(val.strip()) for val in trans_val]
        trans_var = np.array([trans_flt])

        dx, dy, dz = trans_var[0]
        engine.translate(dx, dy, dz)

        self.draw_objects()
    
    def canvas_resized(self, event=None):
        if self.canvas.find_all():
            self.draw_objects(event)