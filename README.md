# snake-sdl
snake game poorly written in C using SDL

## Build
```console
  $ git clone https://github.com/vitohvala/snake-sdl.git
  $ cd snake-sdl
  $ make
  $ ./snake
```
### Run WASM version
```console
  $ python -m http.server 8080 
  $ firefox http://localhost:8080/
```
You'll need:
  * SDL2
  * make
  * C compiler 
  * git
## Controls

| Key                               | Description |
| ---                               | ----------- |
| <kbd>up</kbd> / <kbd>w</kbd>      | Go Up       |
| <kbd>left</kbd> / <kbd>a</kbd>    | Go Left     |
| <kbd>down</kbd> / <kbd>s</kbd>    | Go Down     |
| <kbd>right</kbd> / <kbd>d</kbd>   | Go Right    |
| <kbd>lshift</kbd> && <kbd>z</kbd> | Exit        |
