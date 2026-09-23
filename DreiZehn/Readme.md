# DreiZehn-Script: Scripting Language Syntax & Documentation

## ----- MOVED TO ElfScript Repo -----
## ----- MOVED TO ElfScript Repo -----
## ----- MOVED TO ElfScript Repo -----


---


A lightweight, line by line, embedded scripting language built from scratch in C++. 

[Benchmark Results](Benchmark.md )

```
Ich wollte mir nur mal NaN boxing anschauen und dann hab ich gleich eine kleine Scriptsprache Drumherum gebaut *lol*.
```

## Text Edior

Since it's a uncommon syntax i had good results in my Editor setting Highlight mode:
- lua
- tcsh

## Core Architecture
* **NaN-Boxing Representation:** All variables, primitives (`int32_t`, `double`), and managed pointers (`void*`/`Object*`) are packed into a highly efficient **8-byte single-register** memory footprint.
* **Lexer & Recursive Descent Parser:** Generates a dynamic Abstract Syntax Tree (AST) with mathematical operator precedence.
* **Nested Execution Environment:** Supports local variable scoping (`localEnv`) with parent-lookup optimization, allowing independent memory spaces for loops and function calls.

---

## 1. Data Types & Literals

The language automatically infers types during runtime evaluations.

```script
# Integers and Floating-Point numbers (Doubles)
x = 5
y = 3.1415

# String Literals (Dynamically allocated and tracked by Garbage Collection)
message = "Hello World"
```

---

## 2. Variables & Assignment

Variables are dynamically typed and stored in the active environment. Assigning a value updates the local scope.

```script
speed = 100
multiplier = 2
total_speed = speed * multiplier
```

---

## 3. Mathematical & Logical Expressions

Expressions support standard operator precedence, comparison routing, and recursive parenthesis grouping `()`.

```script
# Math operations
result = (5 + 5) * 2 - 1

# Comparison operators (Return 1 for True, 0 for False)
is_greater = 10 > 5
is_equal = x == 5
```

---

## 4. Control Flow (Conditional Statements)

Conditional logic evaluates any non-zero integer or double as `true`. Block contents are executed sequentially.

```script
if x > 5
    print "Value is greater than 5"
end

if status == 1
    y = x + 10
    print y
end
```

---

## 5. Loops & Iteration (For Statement)

The `for` loop introduces an isolated nested environment. Loop iterators are local and automatically cleaned up upon exit. Supports safe early termination via `break`.

```script
# Simple increment loop (Start to End, inclusive)
for i 1 5
    print i
end

# Loop with early break conditions
for i 1 100
    if i == 5
        print "Target reached, breaking loop."
        break
    end
end
```

---

## 6. User-Defined Functions

Functions parse their AST blocks exactly once and run them on demand inside a private variable scope. Parameters are bound dynamically at call-time. Supports immediate execution halts and expression pipe-backs via `return`.

```script
# Function definition with arguments
fn calculate_bonus score factor
    if score < 50
        return 0 # Early exit with return value
    end
    
    result = score * factor
    return result
end

# Invoking script functions and storing the return value
my_bonus = calculate_bonus 85 2
print my_bonus
```

---

## 7. Built-in Utility Functions

### `print`
Outputs an arbitrary list of space-separated variables, literals, or expressions via the abstract `Tools::printf` pipeline.
```script
print "Result is:" (5 + 5)
```

### `concat`
A flexible string-concatenation utility that merges strings, ints, and doubles into a newly allocated, GC-tracked string object.
```script
log = concat "Iteration: " i " - Value: " x
print log
```

### `run`
Loads, parses, and executes an external script file line-by-line.
```script
run "test_suite.13"
```

---

## 8. Built-in Safety Features
* **Infinite Loop Prevention:** Parser-level locks intercept stalled index trackers and throw non-blocking compiler alerts.
* **String Memory Safety:** The lexer forces automatic emergency lookbehinds on unclosed string sequences (`"hello...`) to prevent state corruption.
* **Garbage Collection (GC):** `Environment::shutdown()` walks through all dynamically tracked memory nodes upon exit to prevent memory leaks in the host C++ application.

## 9. Added Methods by Arrow:

```
> foo = Array.new
> foo->push "Hello Foo"
> print (foo->size) (foo->get 0) 
1 Hello Foo 
> print (foo->pop)
Hello Foo 

```


## x. Basic SDL3 implementation 

Version 0.3: I added SDL3 to test my Script system:

```
SDL_Init SDL_INIT_VIDEO

Window = SDL_CreateWindow "Hello 13" 320 200 SDL_WINDOW_RESIZABLE
Renderer = SDL_CreateRenderer Window "opengl"
Event = SDL_CreateEvent

Running = true
while Running
    while SDL_PollEvent Event
        Type = SDL_GetEventType Event
        if Type == SDL_EVENT_QUIT
            Running = false
        end

        if Type == SDL_EVENT_KEY_DOWN
            Key = SDL_GetEventKey Event
            if Key == SDLK_ESCAPE
                Running = false
            end
        end
    end

    SDL_SetRenderDrawColor Renderer 30 30 40 255
    SDL_RenderClear Renderer

    SDL_SetRenderDrawColor Renderer 255 255 255 255
    SDL_RenderDebugText Renderer 10 10 "Hello SDL3 with events"

    SDL_RenderPresent Renderer
    SDL_Delay 16
end

SDL_DestroyWindow Window
SDL_Quit
```
