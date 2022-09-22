<h1 align="center">
	Duckboard Calculator
</h1>

<h3 align="center">
	Simple Onboard Calculator
</h3>

<h4 align="center">
	Status: Finished
</h4>

<p align="center">
	<a href="#about">About</a> •
	<a href="#layout">Layout</a> •
	<a href="#qmk">QMK</a> •
  <a href="#tech-stack">Tech Stack</a>
</p>

## About
Reprogrammed my Duckboard numpad to have calculator functionality.
* 4 function calculator (Add, subtract, multiply, divide) using [Tinyexpr](https://github.com/codeplea/tinyexpr).
* OLED display shows current equation/answer.
* Answer stays saved in onboard memory and can be outputted through print_ans key.
  
https://user-images.githubusercontent.com/40015195/186285716-761a81e4-b0c2-4e70-9bcc-a67bb3b70213.mp4

### Layout
  ```
             EXIT_CALC, DIVIDE, MULTIPLY, MINUS,
             7,         8,      9,           
             4,         5,      6,        ADD,
             1,         2,      3,          
  PRINT_ANS, 0,         0,      DECIMAL,  EQUAL),
  ```

### QMK
```QMK
# Compile Keymap
$ qmk compile

# Flash Keymap
$ qmk flash -kb doodboard/duckboard -km doodboard_duckboard.hex
```

## Tech Stack
Keymap written in C. Compiled and flashed using QMK CLI.
<br>
<div align="center">
  <img src="https://img.shields.io/badge/C-05122A?style=flat&logo=c" alt="c Badge" height="25">&nbsp;
  <img src="https://img.shields.io/badge/QMK-05122A?style=flat&logo=qmk" alt="QMK Badge" height="25">&nbsp;
</div>
