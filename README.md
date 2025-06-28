## 📈 GraphDraw

A fast and interactive SFML-based graphing tool for rendering mathematical functions, table-based graphs, and editable graph points. Supports panning, zooming, and editing via mouse input, with a clean grid UI.

---

## ⚙️ Features

* 📜 Parses and draws math expressions and table-based graphs from file
* 🔍 Zoomable & pannable viewport with dynamic grid scaling
* ✏️ Editable graph mode with mouse-based line creation
* 🎨 Color support via simple mapping in config file
* 💾 Graph file IO (reads and writes to `.graph` file)

---

## 🧠 How It Works

```plaintext
sin(x)
cos(x) cRed
t 0,0;1,1;2,4;
```

* **Math Graphs**:

  * `f(x)=expression` defines a graph (e.g., `sin(x)`)
  * Optional color: add `cColorName` (e.g., `x^2 cGreen`)
* **Table Graphs**:

  * Start with `t ` then a list of `x,y;` pairs
* Lines starting with `/` are comments, `#` marks end of file

---

## 🛠️ Dependencies

* [SFML](https://www.sfml-dev.org/) (Simple and Fast Multimedia Library)
* [muparser](https://github.com/beltoforion/muparser) (Simple and fast math expressions parser)

---

## 🖱️ Controls

| Action              | Mouse / Key           |
| ------------------- | --------------------- |
| Pan                 | Left-click + drag     |
| Zoom                | Scroll wheel          |
| Switch to Edit Mode | Click edit button     |
| Draw Table Graph    | Click to place points |
| Finish Edit Mode    | Click done button     |
| Exit App            | `Esc` key             |

---

## 📁 File Structure

Expected files (relative to working dir):

```plaintext
../../../../files/file.graph
../../../../files/editButton.png
../../../../files/doneButton.png
../../../../files/font.ttf
```

---

## 🧪 Code Notes

* Uses `VertexArray` for all drawable graph data
* View dynamically adjusts all UI elements and grid
* Grid snapping uses "nice" numbers (1, 2, or 5 × 10^n)
* Graph rendering is continuous over visible range with `pointOffset` precision
* Font rendering for axis labels exists but is currently commented out

---

## 🧼 Future Suggestions

* Fix blurry numbers bug
* Add undo/redo support in edit mode
* Implement better function parsing feedback (e.g., invalid expressions)
