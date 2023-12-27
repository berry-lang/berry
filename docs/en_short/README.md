# English document

## Font dependency

English version of the document requires the following fonts:
* [Fangzhengheitian simplified](https://www.fontyukle.net/font/FZHei-B01S)
* [Fang Zhengshu Song Simplified](http://fonts7.com/fonts/F/59972.html)
* [Foundertype imitation Song simplified](http://fonts7.com/fonts/F/59883.html)
* [Fangzheng Kaiti Simplified](https://www.fontyukle.net/font/FZKai-Z03S)

These fonts are free for commercial use.

## Compile

### Ready to work

It is recommended to use the TeX Live suite. This document is compiled with xelatex. First, you need to install the required fonts and run the `fc-cache -fv` command to refresh the font buffer. If latex cannot find the above four fonts on Windows, you can try the following:

1. Find your texive installation Road King, here we take `C:\texlive\2018\` as an example.
2. Open the folder `C:\texlive\2018\texmf-dist\fonts\truetype\`, and create a new folder `fangzheng` (any name) under this directory.
3. Copy the above four font files to this folder.
4. Run `fc-cache -fv`.

### Compile

If you use TeX Live's default TeXworks editor, you need to open `index.tex` and select xelatex to compile it. If xelatex is the most commonly used compiler, it is recommended that you set it as the default compiler.

If you use VS Code as the editor, it is recommended to install the LaTeX Workshop plugin. You need to set the plugin to use xelatex to compile. In this environment, you can open any tex file and press Ctrl + Alt + B to compile.

Please refer to related materials for the usage of other editors.
