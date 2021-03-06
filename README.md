# OCR - By Iron Minds

An Optical Character Recognition app made in C with SDL2.

Contains a GUI and a CLI to train the neural network or to
generate the dataset.

![](readme/menu.png)
![](readme/pic.png)
![](readme/result.png)

## Usage
### Build
Run make to compile this project to bin/ocr.
Furthermore, there are clean and run rules to remove temporary files and run
the program without any argument.

See training section for details about training sessions and dataset
generation.

### Run
After having built the project, the app is located at bin/ocr.

### Examples
The res folder contains some example images.
You should try with res/cctext.bmp (some random text),
res/ij.bmp (text with Is and Js) or even res/hello.bmp.

Note that the current network uses only lower case letters and period.

### Training
To build the dataset use make dataset (before make).

Some requirements are necessary, use this command to install :

```sh
python -m pip install -r requirements.txt
```

You also have to enable execution of the main script

```sh
chmod +x data/generate.sh
```

## Structure
+ README - What you're currently reading.

+ AUTHORS - Who we are.

+ res - Folder for the ressources of the project. For now it's the reference
        images.

+ src - Folder for the source of the project. It contains our code :
+ main.c = The all target, the program.

+ ai.c = Contains the AI functions.
+ ai.h = Header for the ai.c with explanations of the functions.

+ data.c = Contains the dataset functions.
+ data.h = Header for the data.c with explanations of the functions.

+ error.h = Used for detecting errors.

+ gui.c = Contains the GUI functions.
+ gui.h = Header for the gui.c with explanations of the functions.

+ ocr.c = Contains the wrapper functions.
+ ocr.h = Header for the ocr.c with explanations of the functions.

+ save.c = Contains the saving functions.
+ save.h = Header for the save.c with explanations of the functions.

+ train.c = Contains the training functions.
+ train.h = Header for the train.c with explanations of the functions.

+ matrix.c = Contains the matrix functions.
+ matrix.h = Header for the matrix.c with explanations of the functions.

+ tools.c = Contains all the tools for image manipulations.
+ tools.h = Header for the tools.c with explanations of the functions.

+ initializer.c = Contains the functions for initializing matrices.
+ initializer.h = Header for the initializer.c.

+ layers.h/.c = Neural network layers.

+ losses.h/.c = Loss functions (layers) for the neural net.

+ optimizer.h/.c = Neural network optimizer
    (updates weights of the network).

+ bin - Folder that contains the executable file (can be removed by using the
        make clean command).

+ obj - Folder that contains the obj. files (can be removed by using the make
        clean command).

+ model - Default network weights (used in the GUI).

+ Makefile - File for GNU Make.
        Simply use the command make to compile the code.
